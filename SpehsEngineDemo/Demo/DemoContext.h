#pragma once

#include "SpehsEngine/Audio/AudioEngine.h"
#include "SpehsEngine/Core/AssetManager.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Debug/ImmediateModeGraphics.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/Input/EventCatcher.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/InputManager.h"


struct DemoContext
{
	DemoContext();
	~DemoContext();

	void reset();
	bool update();
	void render();
	void showWindowDefault();


	se::AssetManager assetManager;
	se::time::DeltaTimeSystem deltaTimeSystem;

	// Input
	se::input::EventCatcher eventCatcher;
	se::input::InputManager inputManager;
	se::input::EventSignaler eventSignaler;

	// Graphics
	se::gfx::Window mainWindow;
	se::gfx::Renderer renderer;
	se::gfx::Scene scene;
	se::gfx::Camera camera;
	se::gfx::View view;
	se::gfx::ShapeGenerator shapeGenerator;
	se::debug::ImmediateModeGraphics imGraphics;

	// ImGui
	se::imgui::BackendWrapper imguiBackend;

	// Audio
	se::audio::AudioEngine audioEngine;
};
