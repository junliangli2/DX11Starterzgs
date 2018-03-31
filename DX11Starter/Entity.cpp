#include "Entity.h"

using namespace DirectX;

Entity::~Entity()
{
	delete material;
}

Entity::Entity()
{

}

Entity::Entity(Mesh* meshObj, Material* materialObj)
{
	mesh = meshObj;
	material = materialObj;
	samplerState = material->GetSamplerState();
	shaderRV = material->GetShaderRV();
	normalRV = material->GetNormalRV();
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	InitLights();
}

void Entity::InitLights()
{
	directionLight1.AmbientColor = XMFLOAT4(0.1, 0.1, 0.8, 1.0);
	directionLight1.DiffuseColor = XMFLOAT4(.4F, .5f, .4f, 1.0);
	directionLight1.Direction = XMFLOAT3(0, 11, -4);


	directionLight2.AmbientColor = XMFLOAT4(0.8, 0.2, 0.1, 1.0);
	directionLight2.DiffuseColor = XMFLOAT4(0, .4f, 0, 0);
	directionLight2.Direction = XMFLOAT3(0, 0, 1);

}

void Entity::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	UpdateWorldMatrix();
}

void Entity::SetRotation(float x, float y, float z)
{
	rotation = XMFLOAT3(x, y, z);;
	UpdateWorldMatrix();
}

void Entity::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	UpdateWorldMatrix();
}

void Entity::UpdateWorldMatrix()
{
	XMMATRIX trans = XMMatrixTranslation(position.x,position.y,position.z);
	XMMATRIX rotX = XMMatrixRotationX(rotation.x);
	XMMATRIX rotY = XMMatrixRotationY(rotation.y);
	XMMATRIX rotZ = XMMatrixRotationZ(rotation.z);
	XMMATRIX sMatrix = XMMatrixScaling(scale.x,scale.y,scale.z);
	
	XMMATRIX world = trans*rotX*rotY*rotZ*sMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
}

void Entity::PrepareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4 shadowView, XMFLOAT4X4 shadowProj)
{
	material->GetVertexShader()->SetMatrix4x4("world", worldMatrix);
	material->GetVertexShader()->SetMatrix4x4("view", view);
	material->GetVertexShader()->SetMatrix4x4("projection", projection);
	material->GetVertexShader()->SetMatrix4x4("shadowView", shadowView);
	material->GetVertexShader()->SetMatrix4x4("shadowProjection", shadowProj);
	material->GetPixelShader()->SetData(
		"light",  // The name of the (eventual) variable in the shader
		&directionLight1,   // The address of the data to copy
		sizeof(DirectionalLight)); // The size of the data to copy
	material->GetPixelShader()->SetData(
		"light2",  // The name of the (eventual) variable in the shader
		&directionLight2,   // The address of the data to copy
		sizeof(DirectionalLight)); // The size of the data to copy
 

	
	material->GetPixelShader()->SetSamplerState("basicSampler", samplerState);
	material->GetPixelShader()->SetShaderResourceView("diffuseTexture", shaderRV);
	material->GetPixelShader()->SetShaderResourceView("NormalTexture", normalRV);
	material->GetVertexShader()->CopyAllBufferData();
	material->GetVertexShader()->SetShader();
	 
}

XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return worldMatrix;

}

Mesh* Entity::GetMesh()
{
	return mesh;
}

XMFLOAT3 Entity::GetPosition()
{
	return position;
}

XMFLOAT3 Entity::GetRotation()
{
	return rotation;
}

XMFLOAT3 Entity::GetScale()
{
	return scale;
}

void Entity::DrawShadow(ID3D11DeviceContext * context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer * temp = mesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
}
