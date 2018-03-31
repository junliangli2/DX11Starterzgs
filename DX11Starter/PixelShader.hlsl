
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 posForShadow : POSITION1;
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	float3 tangent		: TANGENT;

};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer constantBuffer1 : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
};

Texture2D diffuseTexture  : register(t0); 
Texture2D ShadowMap       : register(t1);
Texture2D NormalTexture  : register(t2);
SamplerState basicSampler : register(s0);
SamplerComparisonState ShadowSampler  : register(s1);


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	input.normal = normalize(input.normal);
input.tangent = normalize(input.tangent);

float3 normalFromMap = NormalTexture.Sample(basicSampler, input.uv).rgb;
float3 N = input.normal;
float3 T = normalize(input.tangent - N * dot(input.tangent, N));
float3 B = cross(T, N);
float3x3 TBN = float3x3(T, B, N);

input.normal = normalize(mul(normalFromMap, TBN));


float3 lightDirection = normalize(-light.Direction);
float lightAmount = saturate(dot(input.normal, lightDirection));

float3 lightDirection2 = normalize(-light2.Direction);
float lightAmount2 = saturate(dot(input.normal, lightDirection2));
//return float4(1,0,0,1);

float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;
shadowUV.y = 1.0f - shadowUV.y; //Flip the Y's

								//Calculate this pixel's actual depth from the light
float depthFromLight = input.posForShadow.z / input.posForShadow.w;

float shadowAmount = ShadowMap.SampleCmpLevelZero(
	ShadowSampler,		// Special "comparison" sampler
	shadowUV,			//where in the shadow map to look
	depthFromLight);	//The depth to compare againist






return surfaceColor * (light.DiffuseColor*lightAmount + light.AmbientColor + light2.DiffuseColor*lightAmount2 + light2.AmbientColor)*shadowAmount;






}