#pragma once

#include "Demo/Demos/DemoApplication.h"
#include "Demo/Utility/CameraController.h"
#include "Demo/Utility/ParticleSystem.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Shape.h"


class ShapeObject
{
public:

	ShapeObject(se::graphics::Scene& _scene, std::shared_ptr<se::graphics::Material> _material,
				std::shared_ptr<se::graphics::Material> _trailMaterial, se::graphics::ShapeGenerator& _shapeGenerator);
	void update(const se::time::Time _deltaTime);

private:

	void init();

	se::graphics::ShapeParameters shapeParams;
	se::graphics::ShapeGenerator& shapeGenerator;
	se::graphics::Shape shape;
	se::graphics::Line trail;
	glm::vec3 direction;
	glm::vec3 axis;
	float velocity;
	float angularVelocity;
};


class GraphicsPlayground final : public DemoApplication
{
public:

	GraphicsPlayground(DemoContext& _context);

	void init() override;
	bool update() override;

private:

	bool observerWindowSupported = false;
	se::graphics::Window observerWindow;
	se::graphics::Camera observerCamera1;
	se::graphics::Camera observerCamera2;
	se::graphics::View observerView1;
	se::graphics::View observerView2;
	bool view2Active = false;

	CameraController cameraController;
	ParticleSystem particleSystem;

	//
	se::graphics::Text testText;
	std::shared_ptr<se::graphics::AmbientLight> ambientLight;
	std::shared_ptr<se::graphics::PointLight> pointLight;
	std::shared_ptr<se::graphics::SpotLight> spotLight;
	std::shared_ptr<se::graphics::SpotLight> cameraLight;
	std::vector<std::unique_ptr<ShapeObject>> objects;
	std::shared_ptr<se::graphics::Material> colorMaterial;
	std::shared_ptr<se::graphics::Material> flatMaterial;
	se::graphics::InstanceBuffer<se::graphics::TransformInstanceData> shapeInstances;
	se::graphics::Shape skybox;
	se::graphics::Shape boxShape;
	se::graphics::Shape planeShape;
	se::graphics::Shape testShape;
	se::graphics::Line originX;
	se::graphics::Line originZ;
	se::graphics::Line originY;
	se::graphics::Model testModel;
	se::graphics::Model testModel2;
	se::graphics::Model animModel;
	se::graphics::Model jumpModel;
	se::graphics::Model ballTestModel;
	se::graphics::Model demonModel;
	se::graphics::Model simpleModel;
	se::graphics::Shape instanceShape;

	//
	int frameN = 0;
	se::time::Time lastObjectSpawned = se::time::Time::zero;
	se::time::Time lastObjectDeleted = se::time::Time::zero;
	se::time::Time lastWindow2Hidden = se::time::Time::zero;
	se::time::Time frameTimer = se::time::Time::zero;
};
