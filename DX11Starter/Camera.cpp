#include "Camera.h"


// For the DirectX Math library
using namespace DirectX;

Camera::~Camera()
{
}

Camera::Camera()
{
	position=XMFLOAT3(0, 0, -5);
	direction = XMFLOAT3(0, 0, 1);
	xAxis = 0.0f;
	yAxis = 0.0f;
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)1280 / 720,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
	Update();

}

void Camera::SetProjectionMatrix(float width, float height)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::UpdateXAxis(float x)
{
	xAxis += x;
	Update();
}

void Camera::UpdateYAxis(float y)
{
	yAxis += y;
	Update();
}

XMFLOAT3 Camera::getpositionvec()
{
	return position;
}

XMFLOAT3 Camera::getdirectionvec()
{
	return direction;
}

void Camera::Update()
{
	XMFLOAT3 forward(0, 0, 1);
	XMFLOAT3 up(0,1,0);
	XMVECTOR forwardVec = XMLoadFloat3(&forward);
	XMVECTOR upVec = XMLoadFloat3(&up);
	XMVECTOR directionVec = XMVector3Rotate(forwardVec, XMQuaternionRotationRollPitchYaw(xAxis,yAxis,0));
	//XMVECTOR directionVec = XMVectorSet(0, 0, 1,0);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	if (GetAsyncKeyState('W') & 0x8000)
	{
		positionVec = positionVec + directionVec * 0.001f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		positionVec = positionVec - directionVec * 0.001f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		positionVec = positionVec - XMVector3Cross(upVec,directionVec) * 0.001f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		positionVec = positionVec + XMVector3Cross(upVec, directionVec) * 0.001f;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		positionVec = positionVec + upVec * 0.001f;

	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		positionVec = positionVec - upVec * 0.001f;
	}
	
	XMStoreFloat3(&position, positionVec);
	XMMATRIX asd;
	asd = XMMatrixLookToLH(positionVec, directionVec, upVec);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(asd));
}