#include "stdafx.h"
#include "Demo/DemoContext.h"

#include "Demo/DefaultResourcePathFinders.h"
#include "SpehsEngine/Core/ScopeTimer.h"

using namespace se::graphics;


DemoContext::DemoContext()
	: mainWindow(true)
	, renderer(mainWindow, RendererFlag::VSync | RendererFlag::MSAA2, RendererBackend::Direct3D11)
	, view(scene, camera)
	, imguiBackend(eventSignaler, 0, renderer)
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

	// Resource Management
	auto graphicsResourceLoader = se::graphics::makeResourceLoader(8);
	shaderManager.setResourcePathFinder(std::make_shared<ShaderPathFinder>());
	shaderManager.setResourceLoader(graphicsResourceLoader);
	shaderManager.createDefaultShaders();
	textureManager.setResourcePathFinder(std::make_shared<TexturePathFinder>());
	textureManager.setResourceLoader(graphicsResourceLoader);
	fontManager.setResourcePathFinder(std::make_shared<FontPathFinder>());
	fontManager.setResourceLoader(graphicsResourceLoader);
	fontManager.createDefaultFonts();
	modelDataManager.setResourcePathFinder(std::make_shared<ModelPathFinder>());
	modelDataManager.setResourceLoader(graphicsResourceLoader);


	///////////////
	// Audio

	auto audioPathFinder = std::make_shared<AudioPathFinder>();
	auto audioResourceLoader = se::audio::makeResourceLoader(4);
	audioManager.setResourceLoader(audioResourceLoader);
	audioManager.setResourcePathFinder(audioPathFinder);

	se::log::info("DemoContext init time: " + std::to_string(initTimer.get().asSeconds()) + " seconds", se::log::GREEN);
}
void DemoContext::reset()
{
	shaderManager.purgeUnused();
	textureManager.purgeUnused();
	fontManager.purgeUnused();
	modelDataManager.purgeUnused();
	shapeGenerator.clear();
	audioManager.purgeUnused();

	inputManager.ignoreQuitRequest();
	mainWindow.ignoreQuitRequest();
}
bool DemoContext::update()
{
	deltaTimeSystem.update();

	shaderManager.update();
	textureManager.update();
	fontManager.update();
	modelDataManager.update();

	eventCatcher.pollEvents();
	eventSignaler.signalEvents(eventCatcher);
	inputManager.update(eventCatcher);

	audioEngine.setListenerDirection(camera.getDirection());
	audioEngine.setListenerPosition(camera.getPosition());
	audioEngine.setListenerUp(camera.getUp());
	audioEngine.update();

	imguiBackend.render();
	renderer.render();

	return !inputManager.isQuitRequested() && !mainWindow.isQuitRequested();
}
