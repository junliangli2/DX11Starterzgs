#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "WICTextureLoader.h"
using namespace DirectX;
using namespace std;

class Material
{

public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11Device* device, ID3D11DeviceContext* context, wchar_t* fileName, wchar_t* fileName2);
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11Device* device, ID3D11DeviceContext* context, wchar_t* fileName);
	Material();
	
	~Material();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderRV();
	ID3D11ShaderResourceView* GetNormalRV();
	ID3D11SamplerState* GetSamplerState();
	ID3D11Texture2D*getsdm();
	ID3D11DepthStencilView*getdsv();
	ID3D11ShaderResourceView*getssrv();
private:
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* shaderRV;
	ID3D11ShaderResourceView* normalRV;
	ID3D11SamplerState* samplerState;
	ID3D11Texture2D* pShadowMap;
	ID3D11DepthStencilView*	pShadowMapDepthView;
	ID3D11ShaderResourceView* pShadowMapSRView;
	D3D11_VIEWPORT shadowMapViewport;
	SimpleVertexShader* vs1;
	float shadowMapBias;
};

