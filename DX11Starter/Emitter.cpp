#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter()
{
}

Emitter::Emitter(
	int numParticles,
	int emitRate,
	float particleLifetime,
	float startSize,
	float endSize,
	DirectX::XMFLOAT4 startColor,
	DirectX::XMFLOAT4 endColor,
	DirectX::XMFLOAT3 startVelocity,
	DirectX::XMFLOAT3 emitterPosition,
	DirectX::XMFLOAT3 emitterAcceleration,
	SimpleVertexShader * vShader,
	SimplePixelShader * pShader,
	ID3D11ShaderResourceView* texture,
	ID3D11Device * device)
{
	//Set initial properties of Emitter
	vertexShader = vShader;
	pixelShader = pShader;
	particleTexture = texture;

	maxParticles = numParticles;
	this->particleLifetime = particleLifetime;
	this->startSize = startSize;
	this->endSize = endSize;
	this->startColor = startColor;
	this->endColor = endColor;
	this->emmissionRate = emitRate;
	this->secondsPerParticle = 1.0f / emitRate;
	initialVelocity = startVelocity;
	this->emitterPosition = emitterPosition;
	emitterParticleAcceleration = emitterAcceleration;

	timeSinceEmission = 0;
	aliveParticleCount = 0;
	firstAliveParticle = 0;
	firstDeadParticle = 0;

	//make the particle array
	particles = new Particle[maxParticles];	

	//Create particle Vertices, intitialize UVs here, and they will never change
	particleVertices = new ParticleVertex[maxParticles * 4]; //There are 4 vertices that make up a particle
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		particleVertices[i].uv = DirectX::XMFLOAT2(0, 0);
		particleVertices[i + 1].uv = DirectX::XMFLOAT2(1, 0);
		particleVertices[i + 2].uv = DirectX::XMFLOAT2(1, 1);
		particleVertices[i + 3].uv = DirectX::XMFLOAT2(0, 1);
	}

	//Create the Dynamic Vertex Buffer
	D3D11_BUFFER_DESC dvbDesc = {}; 
	dvbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dvbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dvbDesc.Usage = D3D11_USAGE_DYNAMIC;

	dvbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;

	device->CreateBuffer(&dvbDesc, 0, &vertexBuffer);

	//Create particle Indices Data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	//Create Particle index Buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;
}


Emitter::~Emitter()
{
	delete[] particles;
	delete[] particleVertices;
	vertexBuffer->Release();
	indexBuffer->Release();
}

void Emitter::SpawnParticle()
{
	if (aliveParticleCount == maxParticles)
		return;

	//Reset the first dead particle
	particles[firstDeadParticle].age = 0;
	particles[firstDeadParticle].size = startSize;
	particles[firstDeadParticle].color = startColor;
	particles[firstDeadParticle].position = emitterPosition;
	particles[firstDeadParticle].startVel = initialVelocity;

	//increment and wrap
	firstDeadParticle++;
	firstDeadParticle %= maxParticles;

	aliveParticleCount++;
}

void Emitter::CopyParticleDataToGPU(ID3D11DeviceContext * context)
{
	//Get updated copy of particle vertices
	if (firstAliveParticle < firstDeadParticle)
	{
		for (int i = firstAliveParticle; i < firstDeadParticle; i++)
		{
			CopyToParticleVertex(i);
		}
	}
	else
	{
		//From firstAlive to max particles
		for (int i = firstAliveParticle; i < maxParticles; i++)
			CopyToParticleVertex(i);

		//From 0 to first dead
		for (int i = 0; i < firstDeadParticle; i++)
			CopyToParticleVertex(i);
	}

	//All Particles sent to GPU in one memcopy call
	D3D11_MAPPED_SUBRESOURCE mappedVB = {};
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);

	memcpy(mappedVB.pData, particleVertices, sizeof(ParticleVertex) * 4 * maxParticles);

	context->Unmap(vertexBuffer, 0);

}

void Emitter::CopyToParticleVertex(int particleIndex)
{
	//4 vertices per particle
	int i = particleIndex * 4;

	//Positions
	particleVertices[i].position = particles[particleIndex].position;
	particleVertices[i + 1].position = particles[particleIndex].position;
	particleVertices[i + 2].position = particles[particleIndex].position;
	particleVertices[i + 3].position = particles[particleIndex].position;

	//Size
	particleVertices[i].size = particles[particleIndex].size;
	particleVertices[i + 1].size = particles[particleIndex].size;
	particleVertices[i + 2].size = particles[particleIndex].size;
	particleVertices[i + 3].size = particles[particleIndex].size;

	//Color
	particleVertices[i].color = particles[particleIndex].color;
	particleVertices[i + 1].color = particles[particleIndex].color;
	particleVertices[i + 2].color = particles[particleIndex].color;
	particleVertices[i + 3].color = particles[particleIndex].color;
}


void Emitter::Update(float deltaTime)
{
	//update all particles
	if (firstAliveParticle < firstDeadParticle && aliveParticleCount > 0)
	{
		for (int i = firstAliveParticle; i < firstDeadParticle; i++)
			ParticleUpdate(deltaTime, i);
	}
	else
	{
		for (int i = firstAliveParticle; i < maxParticles; i++)
			ParticleUpdate(deltaTime, i);
		for (int i = 0; i < firstDeadParticle; i++)
			ParticleUpdate(deltaTime, i);
	}

	//Add to the time
	timeSinceEmission += deltaTime;

	//It is time to emit particles
	while (timeSinceEmission > secondsPerParticle)
	{
		SpawnParticle();
		timeSinceEmission -= secondsPerParticle;
	}
}

void Emitter::ParticleUpdate(float deltaTime, int index)
{
	if (particles[index].age >= particleLifetime)
		return;

	//Update Particle and check to see if it should die
	particles[index].age += deltaTime; 
	if (particles[index].age >= particleLifetime) 
	{
		firstAliveParticle++;
		firstAliveParticle %= maxParticles;
		aliveParticleCount--;
		return;
	}

	//Calculate for future use
	float agePercent = particles[index].age / particleLifetime;

	//interpolate particle color
	DirectX::XMStoreFloat4(
		&particles[index].color,
		DirectX::XMVectorLerp(
			DirectX::XMLoadFloat4(&startColor),
			DirectX::XMLoadFloat4(&endColor),
			agePercent));

	//Lerp Size
	particles[index].size = startSize + agePercent * (endSize - startSize);

	//Adjust the Position
	DirectX::XMVECTOR startPos = DirectX::XMLoadFloat3(&emitterPosition);
	DirectX::XMVECTOR startVel = DirectX::XMLoadFloat3(&particles[index].startVel);
	DirectX::XMVECTOR accel = DirectX::XMLoadFloat3(&emitterParticleAcceleration);
	float t = particles[index].age;

	//Use constant acceleration function
	DirectX::XMStoreFloat3(
		&particles[index].position,
		accel * t * t / 2.0f + startVel * t + startPos);
}

void Emitter::SetPosition(DirectX::XMFLOAT3 newPos)
{
	emitterPosition = newPos;
}

void Emitter::Render(ID3D11DeviceContext * context, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	//copy particles to GPU
	CopyParticleDataToGPU(context);

	//Set up buffers
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	vertexShader->SetMatrix4x4("view", view);
	vertexShader->SetMatrix4x4("projection", proj);
	vertexShader->SetShader();
	vertexShader->CopyAllBufferData();

	pixelShader->SetShaderResourceView("particleTexture", particleTexture);
	pixelShader->SetShader();
	pixelShader->CopyAllBufferData();

	//Draw the correct parts of the buffer
	if (firstAliveParticle < firstDeadParticle)
	{
		context->DrawIndexed(aliveParticleCount * 6, firstAliveParticle * 6, 0);
	}
	else
	{
		//0 -> dead
		context->DrawIndexed(firstDeadParticle * 6, 0, 0);

		//alive -> max
		context->DrawIndexed((maxParticles - firstAliveParticle) * 6, firstAliveParticle * 6, 0);
	}
}


