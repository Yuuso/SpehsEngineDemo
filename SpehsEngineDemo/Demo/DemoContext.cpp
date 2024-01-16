#include "stdafx.h"
#include "Demo/DemoContext.h"

#include "Demo/DefaultResourcePathFinders.h"
#include "SpehsEngine/Core/ScopeTimer.h"
#include "SpehsEngine/Debug/ImGfx.h"
#include "SpehsEngine/Graphics/DefaultFonts.h"
#include "SpehsEngine/Graphics/DefaultShaders.h"


DemoContext::DemoContext()
	: mainWindow(true)
	, renderer(mainWindow,
			   se::gfx::RendererFlag::VSync | se::gfx::RendererFlag::MSAA4,
			   se::gfx::RendererBackend::Direct3D11)
	, view(scene, camera)
	, imguiBackend(eventSignaler, 0, mainWindow)
	, imGraphics(view, assetManager, shapeGenerator)
{
	se::time::ScopeTimer initTimer;

	///////////////
	// Graphics

	// Main window
	mainWindow.setName("SpehsEngineDemo");

	// Main view
	mainWindow.add(view);

	// Camera settings
	camera.setFar(50000.0f);

	// Asset Management
	assetManager.setAsyncTaskManager(std::make_shared<se::AsyncTaskManager>());
	se::gfx::createDefaultFonts(assetManager);
	se::gfx::createDefaultShaders(assetManager);

	imGraphics.init();
	ImGfx::init(imGraphics);

	se::log::info("DemoContext init time: " + std::to_string(initTimer.get().asSeconds()) + " seconds", se::log::GREEN);
}
DemoContext::~DemoContext()
{
	ImGfx::deinit();
}
void DemoContext::reset()
{
	assetManager.purgeUnused();
	shapeGenerator.clear();

	imGraphics.cleanup();

	inputManager.ignoreQuitRequest();
	mainWindow.ignoreQuitRequest();
}
bool DemoContext::update()
{
	deltaTimeSystem.update();
	assetManager.update();

	audioEngine.setListenerDirection(camera.getDirection());
	audioEngine.setListenerPosition(camera.getPosition());
	audioEngine.setListenerUp(camera.getUp());
	audioEngine.update();

	eventCatcher.pollEvents();
	inputManager.update(eventCatcher);
	eventSignaler.signalEvents(eventCatcher);

	return !inputManager.isQuitRequested() && !mainWindow.isQuitRequested();
}
void DemoContext::render()
{
	imguiBackend.render();
	renderer.render();
	imGraphics.endFrame();
}
void DemoContext::showWindowDefault()
{
	mainWindow.setBorderless(false);
	mainWindow.setWidth(1600);
	mainWindow.setHeight(900);
	mainWindow.setCenteredX();
	mainWindow.setCenteredY();
	mainWindow.show();
}
