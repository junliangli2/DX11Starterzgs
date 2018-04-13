#include "Game.h"
#include "Vertex.h"
#include <iostream>
#include<vector>
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
 
float qwe1=0;
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
	
	delete entity0;
	delete entity1;
	delete entity2;
	delete entity3;
	delete m_mesh0;
	delete m_mesh1;
	delete m_mesh2;
	delete camera;
	pShadowMap->Release();
	pShadowMapDepthView->Release();
	pShadowMapSRView->Release();
	shadowSamplerState->Release();
	shadowRastState->Release();
	delete shadowVS;
	delete pixelShader;
	delete vertexShader;
	delete particlePShader;
	delete particleVShader;
	delete campfireEmitter;
	particleBlendState->Release();
	particleDepthState->Release();
	particleSRV->Release();
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
	CreateParticles();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

	//Create Texttures
	material0 = new Material(vertexShader, pixelShader, device, context, L"GrassTest.jpg", L"terrainnormal.jpg");
	material1 = new Material(vertexShader, pixelShader,device,context,L"rock.jpg", L"rockNormals.jpg");
	material2 = new Material(vertexShader, pixelShader, device, context, L"T2.bmp", L"rockNormals.jpg");
	material3 = new Material(vertexShader, pixelShader, device, context, L"rock.jpg", L"rockNormals.jpg");

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
	entity0 = new Entity(m_mesh0, material0);
	
	entity1 = new Entity(m_mesh1,material1);
	entity2 = new Entity(m_mesh2,material2);
	entity3 = new Entity(m_mesh1,material3);
	entity3->SetPosition(-3,-3,0);
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

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Game::CreateParticles()
{
	campfireEmitter = new Emitter(
		1,
		20,
		0.4f,
		0.1f,
		2.0f,
		XMFLOAT4(1, 0.1f, 0.1f, 1.0f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0.0f),		// End color
		XMFLOAT3(0.0f, 1.2f, 0.0f),				// Start velocity
		XMFLOAT3(1.0f, 0.3f, -2),				// Start position
		XMFLOAT3(),				// Start acceleration
		particleVShader,
		particlePShader,
		particleSRV,
		device);
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
	entity0->SetPosition(-100.0f, -10.0f, -20.0f);
	entity1->SetPosition(0, 0 , 0 );
	
	  
 
	campfireEmitter->Update(deltaTime);
 
	//update the camera
	camera->Update();
	 
	if (flaga == true) {
		roty = roty + shame1;
		flaga = false;
	}
	if (flagb == true) {
		roty = roty + shame1;
		flagb = false;
	}
	if (flagc == true) {
		rotx = rotx + shame;
		flagc = false;
	}
	if (flagd == true) {
		rotx = rotx + shame;
		flagd = false;
	}
 
	entity2->SetPosition(camera->getpositionvec().x, camera->getpositionvec().y, camera->getpositionvec().z);
 
	entity2->SetRotation( rotx,-90+roty, rotz);
	 
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
	
	ID3D11SamplerState* sample;
	D3D11_SAMPLER_DESC sampleDesc = {}; //Holds options for sampling

										//Describes how to handle addresses outside 0-1 UV range
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//Describes how to handle sampling between pixels
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX; //Mipmaps (if applicable)

	HRESULT sampleResult = device->CreateSamplerState(&sampleDesc, &sample);
	ID3D11SamplerState* particleSample;
	sampleDesc = {}; //Holds options for sampling

					 //Describes how to handle addresses outside 0-1 UV range
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;	//Describes how to handle sampling between pixels
	sampleDesc.MaxAnisotropy = 16;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX; //Mipmaps (if applicable)
	 
	sampleResult = device->CreateSamplerState(&sampleDesc, &particleSample);
		float blendArray[4] = { 1,1,1,1 };
		context->OMSetBlendState(particleBlendState, blendArray, 0xffffffff); 
		context->OMSetDepthStencilState(particleDepthState, 0);			 
 
		particlePShader->SetSamplerState("trilinear", particleSample);
		campfireEmitter->Render(context, viewMatrix, projectionMatrix);

		sample->Release();
		particleSample->Release();
		context->OMSetBlendState(0, blendArray, 0xffffffff);
		context->OMSetDepthStencilState(0, 0);
		
	

 
	
	entity1->PrepareMaterial(viewMatrix, projectionMatrix, shadowViewMatrix, shadowProjectionMatrix);
	pixelShader->SetShaderResourceView("ShadowMap", pShadowMapSRView);
	pixelShader->SetSamplerState("ShadowSampler", shadowSamplerState);

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	ID3D11Buffer* vertexBuffer=entity1->GetMesh()->GetVertexBuffer();
	ID3D11Buffer* indexBuffer= entity1->GetMesh()->GetIndexBuffer();
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
	entity2->PrepareMaterial(viewMatrix, projectionMatrix,shadowViewMatrix,shadowProjectionMatrix);

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





	if (GetAsyncKeyState(VK_LBUTTON)) {
		
		
		
		
		
		
		if (etts.size() == 0) {
			Mesh *m_mesh100 = new Mesh("sphere.obj", device);
			Material * mtrl = new Material(vertexShader, pixelShader, device, context, L"shell.jpg", L"bnormal.png");
			Entity *entity123 = new Entity(m_mesh100, mtrl, XMFLOAT3(camera->getdirectionvec()));
			entity123->SetScale(.01f, .01f, .01f);
			entity123->SetPosition(camera->getpositionvec().x, camera->getpositionvec().y, camera->getpositionvec().z);
			etts.push_back(entity123);
		}

		else if(GetTickCount() - etts.back()->time > 500) {
			Mesh *m_mesh100 = new Mesh("sphere.obj", device);
			Material * mtrl = new Material(vertexShader, pixelShader, device, context, L"shell.jpg", L"bnormal.png");
			Entity *entity123 = new Entity(m_mesh100, mtrl, XMFLOAT3(camera->getdirectionvec()));
			entity123->SetScale(.01f, .01f, .01f);
			entity123->SetPosition(camera->getpositionvec().x, camera->getpositionvec().y, camera->getpositionvec().z);
			etts.push_back(entity123);
			}
	



	}

	if (etts.size() > 0) {
		for (int i = 0; i < etts.size(); i++) {
			if (GetAsyncKeyState(VK_SHIFT)) {
				etts.at(i)->move(1000);
			}
			else { etts.at(i)->move(100); }
			
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


	}




	pixelShader->SetShaderResourceView("ShadowMap", 0);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);





 
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	// TERRAIN DRAW
	
	swapChain->Present(0, 0);
}

void Game::UpdateCameraAxis(int x,int y)
{
	float deltaX = (float)(x - prevMousePos.x);
	float deltaY = (float)(y - prevMousePos.y);
	float deltaXAxis = deltaY / 300;
	float deltaYAxis = deltaX / 300;
 

	if (flagc == false && flagd == false) {
		shame = deltaXAxis;
		if (deltaXAxis > 0) {

			flagc = true;
		}
		else if (deltaXAxis < 0) {
			flagd = true;


		}
	}


	if (flaga==false&&flagb==false) {
		shame1 = deltaYAxis;
		if (deltaYAxis > 0) {
			
			flaga = true;
		}
		else if (deltaYAxis < 0) {
			flagb = true;


		}
	}
	camera->UpdateXAxis(deltaXAxis);
	camera->UpdateYAxis(deltaYAxis);
}

void Game::CreateMaterials()
{
	//Asset Manager Loading
	//Create Sampler State
	
	 

	//Create the sampler object
	
 
	//Create Texture


	//import particle texture
 
	CreateWICTextureFromFile(device, context,L"fireParticle.jpg", 0, &particleSRV);
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
	prevMousePos.x = x;
	prevMousePos.y = y;

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
	if ((prevMousePos.x<-10000)||(prevMousePos.y<-10000))
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