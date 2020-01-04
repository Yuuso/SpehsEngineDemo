#include "stdafx.h"

#include "SpehsEngine/Core/Console.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/OS.h"
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
	window2.add(view);

	se::graphics::ShaderManager shaderManager;

	se::graphics::Shape plane;
	plane.shader = shaderManager.find("color");
	scene.add(plane);

	se::Console console;

	const auto initTime = se::time::now() - initStart;
	se::log::info("Init time: " + std::to_string(initTime.asSeconds()) + " seconds", se::log::GREEN);

	int frameN = 0;
	while (true)
	{
		deltaTimeSystem.deltaTimeSystemUpdate();
		//inifile.update();

		input.update();
		eventCatcher.pollEvents();
		eventSignaler.signalEvents(eventCatcher);
		inputManager.update(eventCatcher);

		scene.update();

		renderer.debugTextPrintf(1, 1, "frame: %i", frameN++);
		renderer.render();

		if (inputManager.isQuitRequested())
			break;
	}

	//scene.remove(plane);
	//window.remove(view);

	return 0;
}