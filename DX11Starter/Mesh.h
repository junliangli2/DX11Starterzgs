#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include <fstream>
#include<string.h>

using namespace DirectX;
class Mesh
{
public:
	Mesh();
	Mesh(Vertex* vertexArray,int vertexCount,UINT* indexArray, int indexCount, ID3D11Device* device);
	 
	Mesh(char* fileName, ID3D11Device * device);
	~Mesh();
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	void SetBuffers(Vertex* vertexArray, int vertexCount, UINT* indexArray, int indexCount, ID3D11Device* device);
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	void calculateNormals();
private:
	// Buffers to hold actual geometry data
	char* name;
	int terrainWidth = 257;
	int terrainHeight = 257;
	float heightScale = 15.0;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	struct VectorType
	{
		float x, y, z;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	HeightMapType* heightMap = 0;
	ModelType* terrainModel = 0;

	//An integer specifying how many indices are in the mesh's index buffer
	int m_indexCount;

};
