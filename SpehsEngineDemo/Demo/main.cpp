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
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/HexColor.h"
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
	//window1.setResizable(false);
	//window1.setBorderless(true);
	window1.setName("SpehsEngineDemo");
	window1.setWidth(1600);
	window1.setHeight(900);
	se::graphics::Renderer renderer(window1, se::graphics::RendererFlag::VSync
										   | se::graphics::RendererFlag::MSAA2
//										   | se::graphics::RendererFlag::OpenGL
										   );

	se::graphics::Scene scene;
	se::graphics::Camera camera;
	se::graphics::View view(scene, camera);
	window1.add(view);
	//camera.setProjection(se::graphics::Projection::Orthographic);
	//camera.setZoom(0.01f);

	se::graphics::Scene hudScene;
	se::graphics::Camera hudCamera;
	se::graphics::View hudView(hudScene, hudCamera);
	window1.add(hudView);
	hudView.setClearFlags(se::graphics::ViewClearFlag::ClearDepth);
	hudCamera.setUp({ 0.0f, 0.0f, -1.0f });
	hudCamera.setPosition({ 0.0f, 1.0f, 0.0f });
	hudCamera.setDirection({ 0.0f, -1.0f, 0.0f });
	hudCamera.setProjection(se::graphics::Projection::Orthographic);

	se::graphics::Window window2;
	if (!renderer.checkRendererFlag(se::graphics::RendererFlag::OpenGL)
	&&  !renderer.checkRendererFlag(se::graphics::RendererFlag::Vulkan)
		) // TODO: This can't be true!
	{
		renderer.add(window2);
	}
	//window2.forceKeepAspectRatio(true);
	window2.setWidth(1600);
	window2.setHeight(1000);
	window2.setX(20);
	window2.setY(20);
	window2.setName("SpehsEngineDemo 2");
	se::graphics::Camera observerCamera1;
	se::graphics::Camera observerCamera2;
	se::graphics::View observerView1(scene, observerCamera1);
	se::graphics::View observerView2(scene, observerCamera2);
	observerView1.setSize(se::graphics::ViewSize(1.0f, 0.5f));
	observerView1.setClearColor(se::DimGray);
	observerView2.setSize(se::graphics::ViewSize(1.0f, 0.5f));
	observerView2.setOffset(se::graphics::ViewSize(0.0f, 0.5f));
	observerView2.setClearColor(se::DarkGray);
	observerView1.setMSAAEnabled(false);
	observerView2.setMSAAEnabled(false);
	window2.add(observerView1);
	window2.add(observerView2);
	bool view2Active = true;

	observerCamera1.setPosition(glm::vec3(20.0f,
										 20.0f,
										 20.0f));
	observerCamera1.setTarget(glm::vec3(0.0f));
	observerCamera2.setPosition(glm::vec3(-20.0f,
										  20.0f,
										  -20.0f));
	observerCamera2.setTarget(glm::vec3(0.0f));


	CameraController cameraController(window1, camera, eventSignaler);

	se::graphics::ShaderManager shaderManager;

	class ShapeObject
	{
	public:

		ShapeObject(se::graphics::Scene& _scene, se::graphics::ShaderManager& _shaderManager)
			: shape(se::rng::random(3, 11))
		{
			shape.setShader(_shaderManager.find("color"));
			_scene.add(shape);
			shape.setScale(glm::vec3(se::rng::random(0.5f, 2.0f)));
			shape.setRenderMode(se::graphics::RenderMode::Dynamic);
			shape.setPrimitiveType(se::graphics::PrimitiveType::Triangles);
			init();
		}
		void update(const se::time::Time _deltaTime)
		{
			velocity = glm::max(0.0f, velocity - 1.0f * _deltaTime.asSeconds());
			angularVelocity = glm::max(0.0f, angularVelocity - 3.0f * _deltaTime.asSeconds());

			shape.setPosition(shape.getPosition() + direction * velocity * _deltaTime.asSeconds());
			shape.setRotation(glm::rotate(shape.getRotation(), angularVelocity * _deltaTime.asSeconds(), axis));

			if (velocity < 0.0001f && angularVelocity < 0.0001f)
				init();
		}

	private:

		void init()
		{
			direction = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
			axis = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
			velocity = se::rng::random(3.0f, 6.0f);
			angularVelocity = se::rng::random(5.0f, 15.0f);

			//shape.setRenderMode((se::graphics::RenderMode)se::rng::random(0, 2));
			//shape.setPrimitiveType((se::graphics::PrimitiveType)se::rng::random(0, 2));
		}

		se::graphics::Shape shape;
		glm::vec3 direction;
		glm::vec3 axis;
		float velocity;
		float angularVelocity;
	};
	std::vector<std::unique_ptr<ShapeObject>> objects;

	se::graphics::Shape hudShape(4);
	hudShape.setShader(shaderManager.find("color"));
	hudShape.setRenderMode(se::graphics::RenderMode::Dynamic);
	hudShape.setPrimitiveType(se::graphics::PrimitiveType::Triangles);
	hudShape.setColor(se::hexColor(se::Orange));
	hudShape.setScale(glm::vec3(20.0f));
	hudShape.setPosition({ -window1.getWidth() * 0.5f + 20.0f, 0.0f, -window1.getHeight() * 0.5f + 50.0f });
	hudScene.add(hudShape);

	se::Console console;

	const auto initTime = se::time::now() - initStart;
	se::log::info("Init time: " + std::to_string(initTime.asSeconds()) + " seconds", se::log::GREEN);

	int frameN = 0;
	se::time::Time lastObjectSpawned = se::time::Time::zero;
	se::time::Time lastObjectDeleted = se::time::Time::zero;
	se::time::Time lastWindow2Hidden = se::time::Time::zero;
	while (true)
	{
		if (objects.size() < 100 &&
			se::time::now() - lastObjectSpawned > se::time::fromSeconds(0.4f))
		{
			objects.emplace_back(std::make_unique<ShapeObject>(scene, shaderManager));
			lastObjectSpawned = se::time::now();
		}
		else if (objects.size() > 0 &&
				 se::time::now() - lastObjectDeleted > se::time::fromSeconds(1.2f))
		{
			objects.erase(objects.begin() + se::rng::random<size_t>(0, objects.size() - 1));
			lastObjectDeleted = se::time::now();

			view2Active = !view2Active;
			if (view2Active)
			{
				window2.add(observerView2);
				window2.setName("SpehsEngineDemo 2 (observerView2 active)");
			}
			else
			{
				window2.remove(observerView2);
				window2.setName("SpehsEngineDemo 2 (observerView2 inactive)");
			}

			//const auto displaySize = renderer.getDisplaySize();
			//window1.setX(se::rng::random(0, displaySize.x - window1.getWidth()));
			//window1.setY(se::rng::random(0, displaySize.y - window1.getHeight()));
		}

		if (window2.isQuitRequested())
		{
			window2.ignoreQuitRequest();
			if (window2.isShown())
			{
				window2.hide();
				lastWindow2Hidden = se::time::now();
			}
		}
		else if (!window2.isShown() && se::time::timeSince(lastWindow2Hidden) > se::time::fromSeconds(3.0f))
		{
			window2.show();
		}

		deltaTimeSystem.deltaTimeSystemUpdate();
		//inifile.update();

		eventCatcher.pollEvents();
		eventSignaler.signalEvents(eventCatcher);
		inputManager.update(eventCatcher);

		cameraController.update(deltaTimeSystem.deltaTime);

		for (auto&& object : objects)
			object->update(deltaTimeSystem.deltaTime);

		se::graphics::Renderer::debugTextPrintf(1, 1, "frame: %i", frameN++);
		renderer.render();

		if (inputManager.isQuitRequested())
			break;
	}

	return 0;
}