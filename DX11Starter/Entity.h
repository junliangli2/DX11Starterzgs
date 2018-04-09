#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include "DirectXMath.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"


using namespace DirectX;

class Entity
{
public:
	Entity();
	Entity(Mesh* meshObj, Material* materialObj);
	Entity&operator = (const Entity &t)
	{
	}
	~Entity();


	XMFLOAT4X4 GetWorldMatrix();
	Mesh* GetMesh();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	void DrawShadow(ID3D11DeviceContext *context);
	void InitLights();
	void SetPosition(float x,float y,float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void UpdateWorldMatrix();
	void PrepareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4 shadowView, XMFLOAT4X4 shadowProj);

private:
	//transformation
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	//matrices
	XMFLOAT4X4 worldMatrix;

	//Mesh
	Mesh* mesh;

	//lights
	DirectionalLight directionLight1;
	DirectionalLight directionLight2;
	//Material
	Material* material;
	ID3D11ShaderResourceView* shaderRV;
	ID3D11ShaderResourceView* normalRV;
	ID3D11SamplerState* samplerState;
};
