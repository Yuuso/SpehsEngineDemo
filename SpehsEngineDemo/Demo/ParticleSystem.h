#pragma once

#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include <memory>
#include <vector>


class ParticleSystem
{
public:

	ParticleSystem(se::graphics::Scene& _scene, se::graphics::ShaderManager& _shaderManager, se::graphics::TextureManager& _textureManager, se::graphics::ShapeGenerator& _shapeGen);

	void update(const se::time::Time _deltaTime);

private:

	struct Particle
	{
		glm::vec3 velocity;
		float anglularVelocity;
		se::time::Time lifeTime;
		se::graphics::BillboardInstanceData instanceData;
	};

	se::graphics::InstanceBuffer<se::graphics::BillboardInstanceData> instanceBuffer;
	std::shared_ptr<se::graphics::FlatTextureMaterial> material;
	se::graphics::Shape shape;

	std::vector<Particle> particles;
	const glm::vec3 spawnPosition = glm::vec3(0.0f, -25.0f, -50.0f);
	se::time::Time spawnTimer = se::time::Time::zero;
};
