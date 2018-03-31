#include "Material.h"


// For the DirectX Math library
using namespace DirectX;

Material::~Material()
{

 
	shaderRV->Release();
	samplerState->Release();
	normalRV->Release();
}

Material::Material()
{

}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetShaderRV()
{
	return shaderRV;
}

ID3D11ShaderResourceView * Material::GetNormalRV()
{
	return normalRV;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return samplerState;
}

ID3D11Texture2D * Material::getsdm()
{
	return nullptr;
}

ID3D11DepthStencilView * Material::getdsv()
{
	return nullptr;
}

ID3D11ShaderResourceView * Material::getssrv()
{
	return nullptr;
}

Material::Material(SimpleVertexShader* vertexS, SimplePixelShader* pixelS, ID3D11Device* device, ID3D11DeviceContext* context, wchar_t* fileName, wchar_t* fileName2)
{
	//ID3D11ShaderResourceView* srv;
	CreateWICTextureFromFile(device,context,fileName,0,&shaderRV);
		D3D11_SAMPLER_DESC desc;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 0;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[4]=(0.0f,0.0f,0.0f,0.0f);
	desc.MinLOD = 0;
	//ID3D11SamplerState* state;

	CreateWICTextureFromFile(device, context, fileName2, 0, &normalRV);
 
 

	device->CreateSamplerState(&desc, &samplerState);
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 1024;
	viewport.Height = 1024;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	//setsimplerstate

	vertexShader = vertexS;
 
	 
	pixelShader = pixelS;
	
}

 