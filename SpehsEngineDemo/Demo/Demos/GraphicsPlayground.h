#pragma once

#include "Demo/Demos/DemoApplication.h"
#include "Demo/Utility/CameraController.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Shape.h"


class ShapeObject
{
public:

	ShapeObject(se::gfx::Scene& _scene, std::shared_ptr<se::gfx::Material> _material,
				std::shared_ptr<se::gfx::Material> _trailMaterial, se::gfx::ShapeGenerator& _shapeGenerator);
	void update(const se::time::Time _deltaTime);

private:

	void init();

	se::gfx::ShapeParameters shapeParams;
	se::gfx::ShapeGenerator& shapeGenerator;
	se::gfx::Shape shape;
	se::gfx::Line trail;
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
	se::gfx::Window observerWindow;
	se::gfx::Camera observerCamera1;
	se::gfx::Camera observerCamera2;
	se::gfx::View observerView1;
	se::gfx::View observerView2;
	bool view2Active = false;

	CameraController cameraController;

	//
	se::gfx::Text testText;
	std::shared_ptr<se::gfx::AmbientLight> ambientLight;
	std::shared_ptr<se::gfx::PointLight> pointLight;
	std::shared_ptr<se::gfx::SpotLight> spotLight;
	std::shared_ptr<se::gfx::SpotLight> cameraLight;
	std::vector<std::unique_ptr<ShapeObject>> objects;
	std::shared_ptr<se::gfx::Material> colorMaterial;
	std::shared_ptr<se::gfx::Material> flatMaterial;
	se::gfx::InstanceBuffer<se::gfx::TransformInstanceData> shapeInstances;
	se::gfx::Shape skybox;
	se::gfx::Shape boxShape;
	se::gfx::Shape planeShape;
	se::gfx::Shape testShape;
	se::gfx::Line originX;
	se::gfx::Line originZ;
	se::gfx::Line originY;
	se::gfx::Model testModel;
	se::gfx::Model testModel2;
	se::gfx::Model animModel;
	se::gfx::Model jumpModel;
	se::gfx::Model ballTestModel;
	se::gfx::Model demonModel;
	se::gfx::Model simpleModel;
	se::gfx::Shape instanceShape;

	//
	int frameN = 0;
	se::time::Time lastObjectSpawned = se::time::Time::zero;
	se::time::Time lastObjectDeleted = se::time::Time::zero;
	se::time::Time lastWindow2Hidden = se::time::Time::zero;
	se::time::Time frameTimer = se::time::Time::zero;
};
