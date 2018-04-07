#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "DirectXMath.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	//Camera(Mesh* meshObj);
	~Camera();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();
	void SetProjectionMatrix(float width, float height);
	void Update();
	void UpdateXAxis(float x);
	void UpdateYAxis(float y);
	XMFLOAT3 getpositionvec();
	XMFLOAT3 getdirectionvec();
private:
	//transformation
	XMFLOAT3 position;
	XMFLOAT3 direction;
	

	float xAxis;
	float yAxis;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
};
