#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "Emitter.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "DDSTextureLoader.h"
class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
	
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();
	void CreateCamera();
	void Createshadowmap();
	void CreateParticles();
	void UpdateCameraAxis(int x,int y);
	void CreateMaterials();
	void CreateSkybox();
	void MoveCharacters(float deltaTime);
	void CheckCollision();
	void CheckShooted(float deltaTime);

	//Managers
	 

	//Shaders
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	SimpleVertexShader* particleVShader;
	SimplePixelShader* particlePShader;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;


	ID3D11ShaderResourceView* skySRV;

	ID3D11RasterizerState* skyRastState;
	ID3D11DepthStencilState* skyDepthState;
	ID3D11SamplerState* skySampler;

	Camera* camera;
	
	//Mesh
	Mesh *m_mesh1;
	Mesh *m_mesh2;
	Mesh *m_mesh0;
	Mesh *m_mesh3;
	Mesh *m_mesh4;
	Mesh *m_mesh5;
	Mesh *m_mesh6; //wolf
	Mesh *m_mesh7;
	Mesh *m_mesh8; 

	//Entities
	Entity* entity0;
	Entity* entity1;
	Entity* entity2;
	Entity* entity3;
	Entity* entity4;
	Entity* entity5;
	Entity* entity6; //wolf
	Entity* entity7; 
	Entity* entity8; 

	//Materials
	Material* material0;
	Material* material1;
	Material* material2;
	Material* material3;
	Material* material4;
	Material* material5;
	Material* material6; //wolf
	Material* material7; 
	Material* material8; 

	ID3D11Texture2D* pShadowMap;
	ID3D11DepthStencilView*	pShadowMapDepthView;
	ID3D11ShaderResourceView* pShadowMapSRView;
	ID3D11SamplerState* shadowSamplerState;
	ID3D11RasterizerState* shadowRastState;
	 
	//particles
	Emitter* campfireEmitter;
	Emitter* gunSmokeEmitter;
	Emitter* explosionEmitter;
	ID3D11BlendState* particleBlendState;
	ID3D11DepthStencilState* particleDepthState;
	

	SimpleVertexShader* shadowVS;
	D3D11_VIEWPORT shadowMapViewport;
	XMFLOAT4X4 shadowProjectionMatrix;
	XMFLOAT4X4 shadowViewMatrix;
	ID3D11ShaderResourceView* rockSRV;
	ID3D11ShaderResourceView* rockNormalSRV;
	ID3D11SamplerState* particleSample;

	ID3D11SamplerState* sample;
	ID3D11ShaderResourceView* particleSRV;
	ID3D11ShaderResourceView* smokeSRV;
	ID3D11ShaderResourceView* explosionSRV;
	float shadowMapBias;



	// The matrices to go from model space to screen space
	//DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
  


	std::unique_ptr<SpriteBatch> ammo;
	std::unique_ptr<SpriteFont> font;
	ID3D11ShaderResourceView* ammoTexture;

	POINT prevMousePos;
	 
	//
	bool isFiring = false;
	bool WolfIsShooted = false;
	bool CatIsShooted = false;
	bool ElfIsShooted = false;
};

