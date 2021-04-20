#include "stdafx.h"
#include "ParticleSystem.h"

#include "SpehsEngine/Core/RNG.h"


using namespace se::graphics;

ParticleSystem::ParticleSystem(se::graphics::Scene& _scene, DefaultShaderManager& _shaderManager, se::graphics::TextureManager& _textureManager, ShapeGenerator& _shapeGen)
{
	instanceBuffer = std::make_shared<InstanceBuffer>(InstanceBufferType::Billboard);
	material = std::make_unique<FlatTextureMaterial>(_shaderManager);
	auto texture = _textureManager.create("particle_texture", "wonky_smoke_atlas.png");
	material->setTexture(texture);

	ShapeParameters params;
	params.uvScale = 0.5f;
	shape.generate(ShapeType::Square, params, &_shapeGen);
	shape.enableRenderFlags(RenderFlag::BillboardSpherical);
	shape.enableRenderFlags(RenderFlag::Blending);
	shape.disableRenderFlags(RenderFlag::DepthWrite);
	shape.setScale(glm::vec3(3.0f));
	shape.setInstances(instanceBuffer);
	shape.setMaterial(material);
	_scene.add(shape);

	shape.setPosition(spawnPosition);
}

void ParticleSystem::update(const se::time::Time _deltaTime)
{
	spawnTimer -= _deltaTime;
	if (spawnTimer <= se::time::Time::zero)
	{
		particles.push_back(Particle());
		Particle& particle = particles.back();

		particle.instanceData.position = glm::vec3(se::rng::random(-5.0f, 5.0f), 0.0f, se::rng::random(-5.0f, 5.0f));
		particle.instanceData.scale = glm::vec2(se::rng::random(0.8f, 1.2f));
		particle.instanceData.uvOffset = glm::vec2(se::rng::coin() ? 0.0f : 0.5f, se::rng::coin() ? 0.0f : 0.5f);
		particle.velocity = glm::vec3(se::rng::random(-2.0f, 2.0f), 0.0f, se::rng::random(-2.0f, 2.0f));
		particle.anglularVelocity = se::rng::random(-1.0f, 1.0f);
		particle.lifeTime = se::time::fromSeconds(se::rng::random(5.0, 9.0));

		instanceBuffer->grow(1);

		spawnTimer = se::time::fromSeconds(se::rng::random(0.05, 0.2));
	}

	for (size_t i = 0; i < particles.size(); /*i++*/)
	{
		particles[i].lifeTime -= _deltaTime;
		if (particles[i].lifeTime <= se::time::Time::zero)
		{
			particles.erase(particles.begin() + i);
			instanceBuffer->erase(i);
			continue;
		}

		Particle& particle = particles[i];
		particle.instanceData.position += particle.velocity * _deltaTime.asSeconds();
		particle.instanceData.rotationAngle += particle.anglularVelocity * _deltaTime.asSeconds();
		instanceBuffer->set(i, particle.instanceData);

		i++;
	}
}
