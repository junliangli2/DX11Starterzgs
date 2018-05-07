#include "Game.h"
#include "Vertex.h"
#include <iostream>
#include<vector>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include"SimpleMath.h"
#include <Windows.h>
#include <mmsystem.h>
using namespace std;

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//

vector<Entity*> etts;
float ammos = 5;
float time;
bool reload = false;
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)		   // Show extra stats (fps) in title bar?
{

	 
 

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}
float shame;
float shame1;
bool flaga = false;
bool flagb = false;
bool flagc = false;
bool flagd = false;
// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	ammoTexture->Release();
	delete entity0;
	delete entity1;
	delete entity2;
	delete entity3;
	delete entity4;
	delete entity5;
	delete entity6;
	delete entity7;
	delete entity8;
	delete m_mesh0;
	delete m_mesh1;
	delete m_mesh2;
	delete m_mesh3;
	delete m_mesh4;
	delete m_mesh5;
	delete m_mesh6;
	delete m_mesh7;
	delete m_mesh8;
	delete camera;
	pShadowMap->Release();
	pShadowMapDepthView->Release();
	pShadowMapSRView->Release();
	shadowSamplerState->Release();
	shadowRastState->Release();
	skySRV->Release();
	skyDepthState->Release();
	skyRastState->Release();
	skySampler->Release();
	delete shadowVS;
	delete pixelShader;
	delete vertexShader;
	delete skyVS;
	delete skyPS;
	delete particlePShader;
	delete particleVShader;
	delete campfireEmitter;
	delete gunSmokeEmitter;
	particleBlendState->Release();
	particleDepthState->Release();
	particleSRV->Release();
	smokeSRV->Release();
	particleSample->Release();
	if (etts.size() > 0) {
		for (int i = 0; i < etts.size(); i++) {
			delete etts.at(i)->GetMesh();
			delete etts.at(i);
		}

}
	//device->Release();
	
	//delete material1;
	//delete material2;
	//delete material3;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateCamera();
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();
	Createshadowmap();
	CreateMaterials();
	
	

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	
	PlaySound("bensound.wav",NULL, SND_LOOP | SND_ASYNC);
	

	CreateSkybox();
	CreateParticles();

	CreateWICTextureFromFile(device, context, L"ams.png", 0, &ammoTexture);
	ammo.reset(new DirectX::SpriteBatch(context));
	font.reset(new DirectX::SpriteFont(device,L"Arial.spritefont"));
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}




// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
    vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	shadowVS = new SimpleVertexShader(device, context);
	shadowVS->LoadShaderFile(L"VertexShader1.cso");
	
	//Load particle shaders
	particleVShader = new SimpleVertexShader(device, context);
	particleVShader->LoadShaderFile(L"ParticleVertexShader.cso");

	particlePShader = new SimplePixelShader(device, context);
	particlePShader->LoadShaderFile(L"ParticlePixelShader.cso");


	//Load sky shaders

	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");

	//Create Texttures
	material0 = new Material(vertexShader, pixelShader, device, context, L"GrassTest.jpg", L"terrainnormal.jpg");
	material1 = new Material(vertexShader, pixelShader,device,context,L"rock.jpg", L"rockNormals.jpg");
	material2 = new Material(vertexShader, pixelShader, device, context, L"T2.bmp", L"rockNormals.jpg");
	material3 = new Material(vertexShader, pixelShader, device, context, L"rock.jpg", L"rockNormals.jpg");
	material4 = new Material(vertexShader, pixelShader, device, context, L"houseA.jpg", L"houseANM.jpg");
	material5 = new Material(vertexShader, pixelShader, device, context, L"house1.bmp", L"house1_n.bmp");
	material6 = new Material(vertexShader, pixelShader, device, context, L"Wolf_Body.jpg", L"Wolf_Body_NRM.jpg");
	material7 = new Material(vertexShader, pixelShader, device, context, L"auto.jpg", L"auto_NRM.jpg");
	material8 = new Material(vertexShader, pixelShader, device, context, L"Umbreon.png", L"Umbreon_NRM.jpg");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{

	XMMATRIX P = XMLoadFloat4x4(&camera->GetProjectionMatrix());
	XMStoreFloat4x4(&projectionMatrix, P); // Transpose for HLSL!

	
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw

	Vertex vertices1[] =
	{
		{ XMFLOAT3(+2.6f, +1.8f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
		{ XMFLOAT3(+2.8f, +1.5f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
		{ XMFLOAT3(+1.0f, +1.2f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
	};
	Vertex vertices2[] =
	{
		{ XMFLOAT3(-0.5f, +2.0f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
		{ XMFLOAT3(-0.5f, +0.8f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
		{ XMFLOAT3(-2.5f, +0.7f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
		{ XMFLOAT3(-2.5f, +2.0f, +0.0f), XMFLOAT3(+0.0f,+0.0f,-1.0f),XMFLOAT2(+0.0f,+0.0f) },
	};
	//Vertex vertices3[] =
	//{
	//	{ XMFLOAT3(-0.6f, -0.6f, +0.0f), green },
	//	{ XMFLOAT3(-0.0f, -1.0f, +0.0f), blue },
	//	{ XMFLOAT3(-0.5f, -2.0f, +0.0f), red },
	//	{ XMFLOAT3(-1.0f, -1.0f, +0.0f), red },
	//};

	// Set up the indices, which tell us which vertices to use and in which order
	UINT indices1[] = { 0, 1, 2 };
	UINT indices2[] = { 0, 1, 3, 1, 2, 3 };
	//int indices3[] = { 0, 1, 3, 1, 2, 3 };

	//Create mesh objects
	//m_mesh1 = new Mesh(vertices1, 3, indices1, 3, device);
	//m_mesh2 = new Mesh(vertices2, 4, indices2, 6, device);
	m_mesh1 = new Mesh("cone.obj", device);
	m_mesh2 = new Mesh("gaiguode.obj", device);
	m_mesh0 = new Mesh("terrain-heightmap.bmp", device);
	m_mesh3 = new Mesh("houseA_obj.obj", device);
	m_mesh4 = new Mesh("house.obj", device);
	m_mesh5 = new Mesh("cube.obj", device);
	m_mesh6 = new Mesh("Wolf.obj", device);
	m_mesh7 = new Mesh("Puss_in_Boots.obj", device);
	m_mesh8 = new Mesh("UmbreonLowPoly.obj", device);
	
	entity0 = new Entity(m_mesh0, material0);
	
	entity1 = new Entity(m_mesh1,material1);
	entity2 = new Entity(m_mesh2,material2);
	entity3 = new Entity(m_mesh1,material3);
	entity3->SetPosition(-3,-3,0);
	entity4 = new Entity(m_mesh3, material4);
	entity5 = new Entity(m_mesh4, material5);
	entity6 = new Entity(m_mesh6, material6);
	entity7 = new Entity(m_mesh7, material7);
	entity8 = new Entity(m_mesh8, material8);


	entity0->SetPosition(-100.0f, -10.0f, -20.0f);
	entity1->SetPosition(0, 0, 0);
	entity4->SetScale(0.05f, 0.05f, 0.05f);
	entity4->SetPosition(-5, -8.3f, 25.0f);
	entity4->SetRotation(.1f, 63, -.06f);
	entity5->SetScale(0.05f, 0.05f, 0.05f);
	entity5->SetPosition(2, -8.3f, 22);
	entity5->SetRotation(0, 90, 0);
	entity6->SetScale(1.0f, 1.0f, 1.0f);
	entity6->SetPosition(-3.2, -9.0, 30);
	//entity6->SetRotation(0, 90, 0);
	entity7->SetScale(0.8f, 0.8f, 0.8f);
	entity7->SetPosition(3.2, -8.7, 30);
	entity7->SetRotation(0, 90, 0);
	entity8->SetScale(0.3f, 0.3f, 0.3f);
	entity8->SetPosition(-2.5, -8.6, 30);
	//entity8->SetRotation(0, 90, 0);

}

void Game::CreateCamera()
{
	camera = new Camera();

}
float rot = 0;
void Game::Createshadowmap()
{
	
	D3D11_TEXTURE2D_DESC decsdm = {};
	decsdm.Width = 1024;
	decsdm.Height = 1024	;
	decsdm.MipLevels = 1;
	decsdm.ArraySize = 1;
	decsdm.Format = DXGI_FORMAT_R32_TYPELESS;
	decsdm.SampleDesc.Count = 1;
	decsdm.SampleDesc.Quality = 0;
	decsdm.Usage = D3D11_USAGE_DEFAULT;
	decsdm.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	decsdm.CPUAccessFlags = 0;
	decsdm.MiscFlags = 0;
	device->CreateTexture2D(&decsdm, 0, &pShadowMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC descdsv = {};
	descdsv.Format = DXGI_FORMAT_D32_FLOAT;
	descdsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descdsv.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(pShadowMap, &descdsv, &pShadowMapDepthView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
	srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MipLevels = 1;
	srvdesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(pShadowMap, &srvdesc, &pShadowMapSRView);
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSamplerState);

	 
	D3D11_RASTERIZER_DESC rast = {};
	rast.FillMode = D3D11_FILL_SOLID;
	rast.CullMode = D3D11_CULL_BACK;
	rast.DepthClipEnable = true;
	rast.DepthBias = 1000;  
	rast.DepthBiasClamp = 0.0f;
	rast.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&rast, &shadowRastState);

	

}

void Game::CreateParticles()
{
	campfireEmitter = new Emitter(
		1, // numParticles
		20, //emitRate
		1.0f, //particleLifetime
		1.0f, //startSize
		1.0f, //startSize
		XMFLOAT4(1, 0.1f, 0.1f, 1.0f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0.0f),		// End color
		XMFLOAT3(0.0f, 0.0f, 0.0f),				// Start velocity
		XMFLOAT3(0.0f, 0.0f, 0.0f),				// Start position
		XMFLOAT3(),				// Start acceleration
		particleVShader,
		particlePShader,
		particleSRV,
		device);

	gunSmokeEmitter = new Emitter(
		1, // numParticles
		20, //emitRate
		0.5f, //particleLifetime
		1.0f, //startSize
		1.0f, //startSize
		XMFLOAT4(1, 1.0f, 1.0f, 1.0f),	// Start color
		XMFLOAT4(1, 1.0f, 1.0f, 1.0f),		// End color
		XMFLOAT3(0.0f, 0.0f, 0.0f),				// Start velocity
		XMFLOAT3(0.0f, 0.0f, 0.0f),				// Start position
		XMFLOAT3(),				// Start acceleration
		particleVShader,
		particlePShader,
		smokeSRV,
		device);
}

void Game::CreateSkybox()
{
	CreateDDSTextureFromFile(device, L"SunnyCubeMap.dds", 0, &skySRV);

	
	D3D11_SAMPLER_DESC skysamplerDesc = {}; 
	skysamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	skysamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	skysamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	skysamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	skysamplerDesc.MaxAnisotropy = 16;
	skysamplerDesc.MaxLOD = D3D11_FLOAT32_MAX; 

											
	device->CreateSamplerState(&skysamplerDesc, &skySampler);

	// Create states for sky rendering
	D3D11_RASTERIZER_DESC rs = {};
	rs.CullMode = D3D11_CULL_FRONT;
	rs.FillMode = D3D11_FILL_SOLID;
	device->CreateRasterizerState(&rs, &skyRastState);

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &skyDepthState);
}
void Game::MoveCharacters(float deltaTime)
{
	//entity7->cat
	XMFLOAT3 entity7Direction = XMFLOAT3(-6.4*deltaTime*0.1, 1 * deltaTime*0.1, -7 * deltaTime*0.1);
	XMFLOAT3 entity7TempPos = entity7->GetPosition();
	entity7TempPos = XMFLOAT3(entity7TempPos.x + entity7Direction.x, entity7TempPos.y + entity7Direction.y, entity7TempPos.z + entity7Direction.z);
	if (entity7TempPos.x < -10 && entity7TempPos.z < 14.91) {
		entity7->SetPosition(3.2, -8.7, 30);
	}
	else {
		entity7->SetPosition(entity7TempPos.x, entity7TempPos.y, entity7TempPos.z);
	}
	//cout << entity7TempPos.x<<","<< entity7TempPos.y << "," << entity7TempPos.z<<endl;

	//entity6->wolf
	XMFLOAT3 entity6Direction = XMFLOAT3(1.4*deltaTime*0.1, 1.1 * deltaTime*0.1, -10 * deltaTime*0.1);
	XMFLOAT3 entity6TempPos = entity6->GetPosition();
	entity6TempPos = XMFLOAT3(entity6TempPos.x + entity6Direction.x, entity6TempPos.y + entity6Direction.y, entity6TempPos.z + entity6Direction.z);
	if (entity6TempPos.x > -1.127 && entity6TempPos.z < 15.19) {
		entity6->SetPosition(-3.2, -9.0, 30);
	}
	else {
		entity6->SetPosition(entity6TempPos.x, entity6TempPos.y, entity6TempPos.z);
		
	}
	

	//entity8
	XMFLOAT3 entity8Direction = XMFLOAT3(0, 1.4 * deltaTime*0.1, -15 * deltaTime*0.1);
	XMFLOAT3 entity8TempPos = entity8->GetPosition();
	entity8TempPos = XMFLOAT3(entity8TempPos.x + entity8Direction.x, entity8TempPos.y + entity8Direction.y, entity8TempPos.z + entity8Direction.z);
	if (entity8TempPos.y > -7.235 && entity8TempPos.z < 15.38) {
		entity8->SetPosition(-2.5, -8.6, 30);
	}
	else {
		entity8->SetPosition(entity8TempPos.x, entity8TempPos.y, entity8TempPos.z);
	}
	//cout << entity8TempPos.x << "," << entity8TempPos.y << "," << entity8TempPos.z << endl;
}
void Game::CheckCollision()
{
	if (etts.size() > 0) {
		for (int i = 0; i < etts.size(); i++) {
			Entity *bullet = etts.at(i);
			if (fabs(bullet->GetPosition().x - entity6->GetPosition().x) < 1 && fabs(bullet->GetPosition().y - entity6->GetPosition().y) < 1) {
				WolfIsShooted = true;
			}

			if (fabs(bullet->GetPosition().x - entity7->GetPosition().x) < 1 && fabs(bullet->GetPosition().y - entity7->GetPosition().y) < 1) {
				CatIsShooted = true;
			}

			if (fabs(bullet->GetPosition().x - entity8->GetPosition().x) < 1 && fabs(bullet->GetPosition().y - entity8->GetPosition().y) < 1) {
				ElfIsShooted = true;
			}
				
		}
	}
}
// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	//XMMATRIX P = XMMatrixPerspectiveFovLH(
	//	0.25f * 3.1415926535f,	// Field of View Angle
	//	(float)width / height,	// Aspect ratio
	//	0.1f,				  	// Near clip plane distance
	//	100.0f);			  	// Far clip plane distance
	//XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
	camera-> SetProjectionMatrix((float)width, (float)height);
	XMMATRIX P = XMLoadFloat4x4(&camera->GetProjectionMatrix());
	XMStoreFloat4x4(&projectionMatrix, P); // Transpose for HLSL!
}
float rotx = 0;
float roty = 0;
float rotz = 0;		
// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{




	XMMATRIX shProj = XMMatrixOrthographicLH(60, 60, 0.1f, 300.0f);
	XMMATRIX shView = XMMatrixLookAtLH(
		XMVectorSet(0, 20,-20, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shView));
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shProj));
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	
	
	  
	if (isFiring == true) {
		static bool endFire = false;
		static bool endSmoke = false;
		static float firetimer = 0.2f;
		if (!endFire) {
			
			if (firetimer < 0) {
				firetimer = 0.2f;
				campfireEmitter->SetPosition(XMFLOAT3(0, -20, 0));//invisible
				campfireEmitter->Update(0);
				//isFiring = false;
				endFire = true;
			}
			else {
				firetimer = firetimer - deltaTime;
				campfireEmitter->SetPosition(XMFLOAT3(camera->getpositionvec().x, camera->getpositionvec().y + 0.2, camera->getpositionvec().z + 10));
				campfireEmitter->Update(deltaTime);
			}
		}
		
		static float smoketimer = 0.5f;
		if (!endSmoke) {
			
			if (smoketimer < 0) {
				smoketimer = 0.5f;
				gunSmokeEmitter->SetPosition(XMFLOAT3(0, -30, 0));//invisible
				gunSmokeEmitter->Update(0);
				endSmoke = true;
			}
			else {
				smoketimer = smoketimer - deltaTime;
				gunSmokeEmitter->SetPosition(XMFLOAT3(camera->getpositionvec().x-0.1f, camera->getpositionvec().y + 0.7, camera->getpositionvec().z + 10));
				gunSmokeEmitter->Update(deltaTime);
			}
		}
		
		if (endSmoke && endFire) {
			isFiring = false;
			endSmoke = false;
			endFire = false;
		}
		
	}
	//update the camera
	camera->Update();
	 
 
 
	entity2->SetPosition(camera->getpositionvec().x+.03f, camera->getpositionvec().y, camera->getpositionvec().z+.08f);
	entity2->SetRotation( 0,-89.55, .1f);
	
	MoveCharacters(deltaTime);
	CheckCollision();

	XMMATRIX V = XMLoadFloat4x4(&camera->GetViewMatrix());
	XMStoreFloat4x4(&viewMatrix, V); // Transpose for HLSL!
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	context->OMSetRenderTargets(0, 0, pShadowMapDepthView);
	context->ClearDepthStencilView(pShadowMapDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRastState);
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 1024;
	viewport.Height = 1024;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);
	context->PSSetShader(0, 0, 0);

	entity1->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity1->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity1->GetMesh()->GetIndexCount(), 0, 0);
	entity2->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity2->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity2->GetMesh()->GetIndexCount(), 0, 0);
	entity3->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity3->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity3->GetMesh()->GetIndexCount(), 0, 0);
	entity4->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity4->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity4->GetMesh()->GetIndexCount(), 0, 0);
	entity5->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity5->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity5->GetMesh()->GetIndexCount(), 0, 0);
	entity6->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity6->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity6->GetMesh()->GetIndexCount(), 0, 0);
	entity7->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity7->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity7->GetMesh()->GetIndexCount(), 0, 0);
	entity8->DrawShadow(context);
	shadowVS->SetMatrix4x4("world", entity8->GetWorldMatrix());
	shadowVS->CopyAllBufferData();
	context->DrawIndexed(entity8->GetMesh()->GetIndexCount(), 0, 0);
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);
	// Finally do the actual drawing




	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);









	entity1->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);
	pixelShader->SetShaderResourceView("ShadowMap", pShadowMapSRView);
	pixelShader->SetSamplerState("ShadowSampler", shadowSamplerState);

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	ID3D11Buffer* vertexBuffer = entity1->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer = entity1->GetMesh()->GetIndexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity1->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices


	entity0->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);

	ID3D11Buffer* vertexBuffer0 = entity0->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer0 = entity0->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer0, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer0, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity0->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	//draw the second one
	entity2->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);

	ID3D11Buffer* vertexBuffer2 = entity2->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer2 = entity2->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer2, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity2->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices

	//draw the thrid one
	entity3->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);


	ID3D11Buffer* vertexBuffer3 = entity3->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer3 = entity3->GetMesh()->GetIndexBuffer();


	context->IASetVertexBuffers(0, 1, &vertexBuffer3, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer3, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity3->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices


	//////////////////////////
	entity4->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);


	ID3D11Buffer* vertexBuffer4 = entity4->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer4 = entity4->GetMesh()->GetIndexBuffer();


	context->IASetVertexBuffers(0, 1, &vertexBuffer4, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer4, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity4->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	//-----------------------------------
	entity5->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);


	ID3D11Buffer* vertexBuffer5 = entity5->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer5 = entity5->GetMesh()->GetIndexBuffer();


	context->IASetVertexBuffers(0, 1, &vertexBuffer5, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer5, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity5->GetMesh()->GetIndexCount(),
		0,
		0);
	////////
	if (GetAsyncKeyState('R')&&reload==false) {
		time = GetTickCount();
		reload = true;
	}
	if(reload==true&&GetTickCount()-time>2000){
		reload = false;
		ammos = 5;
	
	
	
	}



	if (GetAsyncKeyState(VK_LBUTTON) && ammos > 0 && reload==false) {
		
		if (etts.size() == 0) {
			ammos--;
			Mesh *m_mesh100 = new Mesh("sphere.obj", device);
			Material * mtrl = new Material(vertexShader, pixelShader, device, context, L"shell.jpg", L"bnormal.png");
			Entity *entity123 = new Entity(m_mesh100, mtrl, XMFLOAT3(camera->getdirectionvec()));
			entity123->SetScale(.01f, .01f, .01f);
			entity123->SetPosition(camera->getpositionvec().x, camera->getpositionvec().y, camera->getpositionvec().z);
			etts.push_back(entity123);
			mciSendString("play shoot.wav", NULL, 0, NULL);
			isFiring = true;
		}

		else if (GetTickCount() - etts.back()->time > 500) {
			ammos--;
			Mesh *m_mesh100 = new Mesh("sphere.obj", device);
			Material * mtrl = new Material(vertexShader, pixelShader, device, context, L"shell.jpg", L"bnormal.png");
			Entity *entity123 = new Entity(m_mesh100, mtrl, XMFLOAT3(camera->getdirectionvec()));
			entity123->SetScale(.01f, .01f, .01f);
			entity123->SetPosition(camera->getpositionvec().x, camera->getpositionvec().y, camera->getpositionvec().z);
			etts.push_back(entity123);
			mciSendString("play shoot.wav", NULL, 0, NULL);
			isFiring = true;
		}




	}

	if (etts.size() > 0) {
		for (int i = 0; i < etts.size(); i++) {
		 
		  etts.at(i)->move(100); 
			etts.at(i)->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);
			ID3D11Buffer* vertexBuffer30 = etts.at(i)->GetMesh()->GetVertexBuffer();
			ID3D11Buffer* indexBuffer30 = etts.at(i)->GetMesh()->GetIndexBuffer();
			context->IASetVertexBuffers(0, 1, &vertexBuffer30, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer30, DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(
				etts.at(i)->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
				0,     // Offset to the first index we want to use
				0);    // O


		}
		if (GetTickCount() - etts.at(0)->time > 1000) {
			Entity *todelete = etts.at(0);
			etts.erase(etts.begin());

			delete todelete->GetMesh();
			delete todelete;

		}

	}

	//entity6
	entity6->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);

	ID3D11Buffer* vertexBuffer6 = entity6->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer6 = entity6->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer6, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer6, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity6->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	//end entity6

	//entity7
	entity7->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);

	ID3D11Buffer* vertexBuffer7 = entity7->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer7 = entity7->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer7, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer7, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity7->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	//end entity7
	
	//entity8
	entity8->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);

	ID3D11Buffer* vertexBuffer8 = entity8->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer8 = entity8->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer8, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer8, DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	context->DrawIndexed(
		entity8->GetMesh()->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
	//end entity8

	pixelShader->SetShaderResourceView("ShadowMap", 0);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	// After I draw any and all opaque entities, I want to draw the sky


	ID3D11Buffer* skyVB = m_mesh5->GetVertexBuffer();
	ID3D11Buffer* skyIB = m_mesh5->GetIndexBuffer();

	// Set buffers in the input assembler
	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	// Set up the sky shaders
	skyVS->SetMatrix4x4("view", camera->GetViewMatrix());
	skyVS->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	skyPS->SetShaderResourceView("SkyTexture", skySRV);
	skyPS->SetSamplerState("BasicSampler", skySampler);
	skyPS->SetShader();

	// Set up the render states necessary for the sky
	context->RSSetState(skyRastState);
	context->OMSetDepthStencilState(skyDepthState, 0);
	context->DrawIndexed(m_mesh5->GetIndexCount(), 0, 0);



	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	ammo->Begin(); font->DrawString(ammo.get(), (L"Score:" + to_wstring(ammos)).c_str(), SimpleMath::Vector2(1000, 50));
	if (ammos > 0) {
		
	ammo->Draw(ammoTexture, SimpleMath::Vector2(10, 10));
}
	if (ammos > 1) {
	ammo->Draw(ammoTexture, SimpleMath::Vector2(50, 10));
}
	if (ammos > 2) {
	ammo->Draw(ammoTexture, SimpleMath::Vector2(90, 10));
}
	if (ammos > 3) {
	ammo->Draw(ammoTexture, SimpleMath::Vector2(130, 10));
}
	if (ammos > 4) {
		ammo->Draw(ammoTexture, SimpleMath::Vector2(170, 10));
	}ammo->End();

	//particles
	float blendArray[4] = { 1,1,1,1 };
		
	context->OMSetBlendState(particleBlendState, blendArray, 0xffffffff);
	context->OMSetDepthStencilState(particleDepthState, 0);

	particlePShader->SetSamplerState("trilinear", particleSample);
	gunSmokeEmitter->Render(context, viewMatrix, projectionMatrix);
	campfireEmitter->Render(context, viewMatrix, projectionMatrix);
	
		
		
	context->OMSetBlendState(0, blendArray, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
		

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	// TERRAIN DRAW
	
	swapChain->Present(0, 0);
}

void Game::UpdateCameraAxis(int x,int y)
{
	float deltaX = x - prevMousePos.x;
	float deltaY = y - prevMousePos.y;
	float deltaXAxis = deltaY / 720;
	float deltaYAxis = deltaX / 720;
	camera->UpdateXAxis(deltaXAxis);
	camera->UpdateYAxis(deltaYAxis);
 
}

void Game::CreateMaterials()
{
	//import particle texture
 
	CreateWICTextureFromFile(device, context,L"gunFire.jpg", 0, &particleSRV);
	CreateWICTextureFromFile(device, context, L"smoke.jpg", 0, &smokeSRV);

	// Create a sampler state for texture sampling
	D3D11_SAMPLER_DESC psamplerDesc = {};
	psamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	psamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	psamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	psamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	psamplerDesc.MaxAnisotropy = 16;
	psamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Ask the device to create a state
	device->CreateSamplerState(&psamplerDesc, &particleSample);

	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);


	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);

}



#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
 

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if ((prevMousePos.x<-10000) || (prevMousePos.y<-10000))
	{
		prevMousePos.x = x;
		prevMousePos.y = y;
	}
	// Add any custom code here...
	UpdateCameraAxis(x, y);
	XMMATRIX V = XMLoadFloat4x4(&camera->GetViewMatrix());
	XMStoreFloat4x4(&viewMatrix, V); // Transpose for HLSL!
									 // Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion