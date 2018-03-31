
// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
	// No world matrix necessary for the sky box
	matrix world;
	matrix view;
	matrix projection;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;

};

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;

};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output
	VertexToPixel output;

	matrix worldViewProj = mul(mul(world, view), projection);

	// Then we convert our 3-component position vector to a 4-component vector
	// and multiply it by our final 4x4 matrix.
	//
	// The result is essentially the position (XY) of the vertex on our 2D 
	// screen and the distance (Z) from the camera (the "depth" of the pixel)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	// Which direction is the vertex?
	// Assume that the vert's raw position in space is also
	// it's direction from the origin (this works if the model is
	// actually centered around the origin)


	return output;
}