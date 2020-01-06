#include "stdafx.h"

#include "CameraController.h"
#include "SpehsEngine/Core/Console.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/OS.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Input/EventCatcher.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Input/InputManager.h"

int main()
{
	const auto initStart = se::time::now();

	se::CoreLib core;
	se::InputLib input;
	se::GraphicsLib graphics;

	//se::Inifile inifile("demo");
	//inifile.read();
	//se::Inivar<unsigned>& windowWidth = inifile.get("video", "window_width", 1600u);
	//se::Inivar<unsigned>& windowHeight = inifile.get("video", "window_height", 900u);
	//inifile.write();

	se::time::DeltaTimeSystem deltaTimeSystem;

	se::input::EventCatcher eventCatcher;
	se::input::InputManager inputManager;
	se::input::EventSignaler eventSignaler;

	se::graphics::Window window1;
	se::graphics::Window window2;
	se::graphics::Renderer renderer(window1);
	renderer.add(window2);

	se::graphics::Scene scene;
	se::graphics::Camera camera;
	se::graphics::View view(scene, camera);
	window1.add(view);
	se::graphics::Camera observerCamera;
	se::graphics::View observerView(scene, observerCamera);
	window2.add(observerView);

	observerCamera.setPosition(glm::vec3(20.0f,
										 20.0f,
										 20.0f));
	observerCamera.setTarget(glm::vec3(0.0f));

	CameraController cameraController(camera, eventSignaler);

	se::graphics::ShaderManager shaderManager;

	class ShapeObject
	{
	public:

		ShapeObject(se::graphics::Scene& _scene, se::graphics::ShaderManager& _shaderManager)
			: shape(se::rng::random(3, 11))
		{
			init();
			shape.setShader(_shaderManager.find("color"));
			_scene.add(shape);
		}
		void update(const se::time::Time _deltaTime)
		{
			velocity = glm::max(0.0f, velocity - 1.0f * _deltaTime.asSeconds());
			angularVelocity = glm::max(0.0f, angularVelocity - 3.0f * _deltaTime.asSeconds());

			shape.setPosition(shape.getPosition() + direction * velocity * _deltaTime.asSeconds());
			shape.setRotation(glm::rotate(shape.getRotation(), angularVelocity * _deltaTime.asSeconds(), axis));

			if (velocity < 0.001f && angularVelocity < 0.001f)
				init();
		}

	private:

		void init()
		{
			direction = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
			axis = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
			velocity = se::rng::random(2.0f, 4.0f);
			angularVelocity = se::rng::random(5.0f, 15.0f);
		}

		se::graphics::Shape shape;
		glm::vec3 direction;
		glm::vec3 axis;
		float velocity;
		float angularVelocity;
	};
	std::vector<std::unique_ptr<ShapeObject>> objects;

	se::Console console;

	const auto initTime = se::time::now() - initStart;
	se::log::info("Init time: " + std::to_string(initTime.asSeconds()) + " seconds", se::log::GREEN);

	int frameN = 0;
	se::time::Time lastObjectSpawned = se::time::Time::zero;
	while (true)
	{
		if (objects.size() < 10
			&& se::time::now() - lastObjectSpawned > se::time::fromSeconds(1.4f))
		{
			objects.emplace_back(std::make_unique<ShapeObject>(scene, shaderManager));
			lastObjectSpawned = se::time::now();
		}

		deltaTimeSystem.deltaTimeSystemUpdate();
		//inifile.update();

		input.update();
		eventCatcher.pollEvents();
		eventSignaler.signalEvents(eventCatcher);
		inputManager.update(eventCatcher);

		cameraController.update(deltaTimeSystem.deltaTime);

		for (auto&& object : objects)
			object->update(deltaTimeSystem.deltaTime);

		scene.update();

		renderer.debugTextPrintf(1, 1, "frame: %i", frameN++);
		renderer.render();

		if (inputManager.isQuitRequested())
			break;
	}

	return 0;
}