#pragma once
#include <DirectXMath.h>


struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColor;	    // The position of the vertex
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;
};