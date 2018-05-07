#pragma once
#include "Particle.h"
#include "SimpleShader.h"
#include <d3d11.h>
#include <vector>

class Emitter
{
public:
	Emitter();
	Emitter(
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
		SimpleVertexShader* vShader,
		SimplePixelShader* pShader,
		ID3D11ShaderResourceView* texture,
		ID3D11Device* device);
	~Emitter();

	//update particles and emitter
	void Update(float deltaTime); 

	//renders all particles in emitter
	void Render(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	void SetPosition(DirectX::XMFLOAT3 newPos);

	//update a single particle
	void ParticleUpdate(float deltaTime, int index); 
	void CopyToParticleVertex(int particleIndex);
	void SpawnParticle();
	void CopyParticleDataToGPU(ID3D11DeviceContext* context);
	void SetVelocity(DirectX::XMFLOAT3 newVelocity);
	void ResetEmitter();
	

private:

	//Properties
	DirectX::XMFLOAT3 initialVelocity;	//velocity to emit particles at
	DirectX::XMFLOAT3 emitterParticleAcceleration;	//acceleration to apply to particles
	DirectX::XMFLOAT3 emitterPosition;	//position to emit particles from

	//Particle Properties
	float startSize;
	float endSize;
	float particleLifetime; //how long a particle should be alive for
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	

	//Particles Array
	int firstAliveParticle;	//index to the current oldest living particle that is about to dead 
	int firstDeadParticle;	//index to the current longest dead particle that is about to alive
	int aliveParticleCount;
	Particle* particles;
	ParticleVertex* particleVertices;
	

	//Emission variables
	bool loopable = false;
	bool active = false;
	int maxParticles = 40; 
	int emmissionRate = 1; //Particles emitted per second
	float secondsPerParticle; //how long it takes to emit every particle
	float timeSinceEmission; //how long since emitter emitted a particle

	//Rendering Variables
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	SimpleVertexShader* vertexShader; //vertex Shader for particles to use
	SimplePixelShader* pixelShader;	//pixel Shader for particles to use
	ID3D11ShaderResourceView* particleTexture; //Texture that emitter particle will use
};

