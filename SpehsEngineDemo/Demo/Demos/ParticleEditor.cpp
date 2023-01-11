#include "stdafx.h"
#include "ParticleEditor.h"

#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Uniform.h"

using namespace se::graphics;


ParticleEditor::ParticleEditor(DemoContext& _context)
	: DemoApplication(_context)
	, cameraController(_context.mainWindow, _context.camera, _context.eventSignaler)
{}
void ParticleEditor::init()
{
	//particleSystem.addEmitter<se::xpr::Emitter_CpuBillboard>("");
	//particleSystem.start();

	{
		auto shader = demoContext.shaderManager.create("world_plane", "vs_world_plane.bin", "fs_world_plane.bin");
		auto material = std::make_shared<Material>();
		material->setShader(shader, ShaderVariant::Default);
		material->setName("world_plane");

		worldPlane.generate(ShapeType::Plane, {}, &demoContext.shapeGenerator);
		worldPlane.setMaterial(material);
		worldPlane.setColor(se::mixColor(se::Color(se::SteelBlue), se::Color(se::Gray), 0.35f));
		worldPlane.setScale({ 100000.0f, 1.0f, 100000.0f });
		demoContext.scene.add(worldPlane);
	}

	demoContext.showWindowDefault();
}
bool ParticleEditor::update()
{
	const se::time::Time timeNow = se::time::now();

	{
		const se::time::Time frameTime = timeNow - frameTimer;
		Renderer::debugTextPrintf(1, 1, "frame: %ims", (int)frameTime.asMilliseconds());
		frameTimer = timeNow;

		if (demoContext.inputManager.isKeyPressed((unsigned)se::input::Key::F5))
		{
			se::log::info("Reloading shaders...", se::log::TextColor::BLUE);
			demoContext.shaderManager.reload();
		}
		if (demoContext.inputManager.isKeyPressed((unsigned)se::input::Key::F4))
		{
			se::log::info("Reloading textures and models...", se::log::TextColor::BLUE);
			demoContext.textureManager.reload();
			demoContext.modelDataManager.reload();
		}

		cameraController.update(demoContext.deltaTimeSystem.deltaTime);
	}

	//particleSystem.update();

	return true;
}
