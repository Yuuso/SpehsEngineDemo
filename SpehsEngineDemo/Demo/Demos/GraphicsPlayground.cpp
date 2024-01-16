#include "stdafx.h"
#include "Demo/Demos/GraphicsPlayground.h"

#include "Demo/Utility/Materials.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Debug/ImGfx.h"
#include "SpehsEngine/Graphics/Animator.h"
#include "SpehsEngine/Graphics/DefaultFonts.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/DefaultShaders.h"
#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/ModelAsset.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Texture.h"

using namespace se::gfx;


ShapeObject::ShapeObject(Scene& _scene, std::shared_ptr<Material> _material,
						 std::shared_ptr<Material> _trailMaterial, ShapeGenerator& _shapeGenerator)
	: shapeGenerator(_shapeGenerator)
{
	shape.setMaterial(_material);
	shape.enableRenderFlags(RenderFlag::BillboardSpherical);
	trail.setMaterial(_trailMaterial);
	trail.enableRenderFlags(RenderFlag::BlendAlpha);
	trail.setRenderState(false);
	_scene.add(shape);
	_scene.add(trail);

	shapeParams.generateNormals = true;
	shapeParams.generateTangents = true;

	se::Color objectColor = se::randomBrightColor();
	shape.setColor(objectColor);
	objectColor.a = 0.5f;
	trail.setColor(objectColor);
	shape.setScale(glm::vec3(se::rng::random(0.5f, 2.0f)));
	//shape.setRenderMode(RenderMode::Static);
	//shape.setPrimitiveType(PrimitiveType::Triangles);
	init();
	trail.addPoint(shape.getPosition());
}
void ShapeObject::update(const se::time::Time _deltaTime)
{
	velocity = glm::max(0.0f, velocity - 1.0f * _deltaTime.asSeconds());
	angularVelocity = glm::max(0.0f, angularVelocity - 3.0f * _deltaTime.asSeconds());

	shape.setPosition(shape.getPosition() + direction * velocity * _deltaTime.asSeconds());
	if (!shape.checkRenderFlags(RenderFlag::BillboardSpherical) &&
		!shape.checkRenderFlags(RenderFlag::BillboardCylindrical))
		shape.setRotation(glm::rotate(shape.getRotation(), angularVelocity * _deltaTime.asSeconds(), axis));

	trail.getPoint(trail.numPoints() - 1) = shape.getPosition();

	if (velocity < 0.0001f && angularVelocity < 0.0001f)
		init();
}
void ShapeObject::init()
{
	direction = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
	axis = glm::normalize(se::rng::cube(glm::vec3(-1.0f), glm::vec3(1.0f)));
	velocity = se::rng::random(3.0f, 6.0f);
	angularVelocity = se::rng::random(5.0f, 15.0f);

	//shape.setRenderMode((RenderMode)se::rng::random(0, 1));
	shape.setPrimitiveType((PrimitiveType)se::rng::random(1, 3), shapeGenerator);
	shape.generate((ShapeType)se::rng::random(3, 13), shapeParams, &shapeGenerator);
	shape.disableRenderFlags(RenderFlag::CullBackFace);

	trail.addPoint(shape.getPosition());
}


GraphicsPlayground::GraphicsPlayground(DemoContext& _context)
	: DemoApplication(_context)
	, observerView1(_context.scene, observerCamera1)
	, observerView2(_context.scene, observerCamera2)
	, cameraController(_context.mainWindow, _context.camera, _context.eventSignaler)
{}

void GraphicsPlayground::init()
{
	// TODO: There has to be a way to fix this!
	observerWindowSupported =
		demoContext.renderer.getBackend() == RendererBackend::Direct3D11;

	if (observerWindowSupported)
	{
		observerWindow.hide();
		demoContext.renderer.add(observerWindow);
		observerWindow.setWidth(1600);
		observerWindow.setHeight(1000);
		observerWindow.setX(20);
		observerWindow.setY(20);
		observerWindow.setName("Observer Window");
		observerView1.setSize(ViewSize(1.0f, 0.5f));
		observerView1.setClearColor(se::DimGray);
		observerView2.setSize(ViewSize(1.0f, 0.5f));
		observerView2.setOffset(ViewSize(0.0f, 0.5f));
		observerView2.setClearColor(se::DarkGray);
		observerView1.setMSAAEnabled(false);
		observerView2.setMSAAEnabled(false);
		observerWindow.add(observerView1);
		observerWindow.add(observerView2);

		observerCamera1.setPosition(glm::vec3(20.0f, 20.0f, 20.0f));
		observerCamera1.setTarget(glm::vec3(0.0f));
		observerCamera2.setPosition(glm::vec3(-20.0f, 20.0f, -20.0f));
		observerCamera2.setTarget(glm::vec3(0.0f));

		view2Active = true;
	}

	//////////
	// Resources

	auto makeTexture =
		[&](std::string_view _name)
		{
			return demoContext.assetManager.emplace<Texture>(_name, getTexturePath(_name));
		};
	auto makeModel =
		[&](std::string_view _name)
		{
			return demoContext.assetManager.emplace<ModelAsset>(_name, getModelPath(_name));
		};

	demoContext.assetManager.emplace<Shader>("test", getShaderPath("vs_test.bin"), getShaderPath("fs_test.bin"));
	demoContext.assetManager.emplace<Shader>("test_anim", getShaderPath("vs_test_anim.bin"), getShaderPath("fs_test.bin"));

	//// TODO: Cubemap textures don't work with temp textures!
	//auto skyboxColor = makeTexture("skybox.ktx");
	//skyboxColor->waitUntilReady();
	////

	auto testColor = makeTexture("test_color.png");
	auto testNormal = makeTexture("test_normal.png");
	auto demonColor = makeTexture("balldemon.png");
	auto stoneColor = makeTexture("stone_color.png");
	auto stoneNormal = makeTexture("stone_normal.png");

	auto testFont = demoContext.assetManager.emplace<Font>("testFont", getFontPath("open-sans.regular.ttf"), FontSize(32, FontSizeType::Pixel), defaultCharacterSet);
	auto embeddedFont = demoContext.assetManager.find<Font>(getDefaultFontName(DefaultFontType::Default));

	auto testModelData = makeModel("test.fbx");
	auto animModelData = makeModel("walking_cube.gltf");
	auto jumpModelData = makeModel("jumping_cube.glb");
	auto demonModelData = makeModel("ball_demon.gltf");
	auto rotorTestModelData = makeModel("rotor_test.glb");
	auto icosphereModelData = makeModel("icosphere.fbx");
	auto simpleModelData = makeModel("simple.gltf");

	TextureModes genModes;
	genModes.sampleMin = TextureSamplingMode::Point;
	genModes.sampleMag = TextureSamplingMode::Point;
	genModes.sampleMip = TextureSamplingMode::Point;

	TextureInput textureInput{ 2, 2 };
	textureInput.push(se::Color{ se::Red });
	textureInput.push(se::Color{ se::Green });
	textureInput.push(se::Color{ se::Blue });
	textureInput.push(se::Color{ se::Purple });
	auto genTexture = demoContext.assetManager.emplace<Texture>("genTest", textureInput, genModes);

	textureInput = { 1, 1 };
	textureInput.push(128, 128, 255, 255);
	auto flatNormalTexture = demoContext.assetManager.emplace<Texture>("flatNormal", textureInput, genModes);

	textureInput = { 1, 1 };
	textureInput.push(255, 255, 255, 255);
	auto whiteTexture = demoContext.assetManager.emplace<Texture>("whiteColor", textureInput, genModes);

	textureInput = { 1, 1 };
	textureInput.push(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	auto blackTexture = demoContext.assetManager.emplace<Texture>("blackTexture", textureInput, genModes);

	colorMaterial = createMaterial(DefaultMaterialType::FlatColor, demoContext.assetManager);
	flatMaterial = createMaterial(DefaultMaterialType::FlatTexture, demoContext.assetManager);
	flatMaterial->setTexture(genTexture);

	std::shared_ptr<Material> phongMaterial = createMaterial(DefaultMaterialType::Phong, demoContext.assetManager);
	phongMaterial->setTexture(testColor, PhongTextureType::Color);
	phongMaterial->setTexture(testNormal, PhongTextureType::Normal);

	std::shared_ptr<Material> testMaterial = createTestMaterial(demoContext.assetManager);
	testMaterial->setTexture(testColor, PhongTextureType::Color);
	testMaterial->setTexture(testNormal, PhongTextureType::Normal);

	std::shared_ptr<Material> demonMaterial = createTestMaterial(demoContext.assetManager);
	demonMaterial->setTexture(demonColor, PhongTextureType::Color);
	demonMaterial->setTexture(flatNormalTexture, PhongTextureType::Normal);

	std::shared_ptr<Material> stoneMaterial = createTestMaterial(demoContext.assetManager);
	stoneMaterial->setTexture(stoneColor, PhongTextureType::Color);
	stoneMaterial->setTexture(stoneNormal, PhongTextureType::Normal);

	std::shared_ptr<Material> flatPhongMaterial = createMaterial(DefaultMaterialType::Phong, demoContext.assetManager);
	flatPhongMaterial->setTexture(whiteTexture, PhongTextureType::Color);
	flatPhongMaterial->setTexture(flatNormalTexture, PhongTextureType::Normal);

	std::shared_ptr<Material> textMaterial = createMaterial(DefaultMaterialType::Text, demoContext.assetManager);
	textMaterial->setFont(testFont);

	//std::shared_ptr<Material> skyboxMaterial = createMaterial(DefaultMaterialType::Skybox, demoContext.assetManager);
	//skyboxMaterial->setTexture(skyboxColor);


	//////////
	// Objects, Lights & Primitives

	TextStyle style;
	testText.setMaterial(textMaterial);
	testText.insert("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	testText.insert("abcdrfghijklmnopqrstuvwxyz\n");
	style.color = se::Color(se::Red);
	testText.setStyle(style);
	testText.insert("1234567890\n");
	testText.insert(U"Á\n");
	testText.setScale(glm::vec3(0.02f));
	testText.setPosition(glm::vec3(0.0f, -24.99f, 0.0f));
	testText.enableRenderFlags(RenderFlag::DepthTestLess);
	demoContext.scene.add(testText);

	ambientLight = std::make_shared<AmbientLight>(se::Color(se::White), 0.05f);
	demoContext.scene.add(*ambientLight);

	pointLight = std::make_shared<PointLight>();
	pointLight->setRadius(0.0f, 40.0f);
	demoContext.scene.add(*pointLight);

	auto pointLightDebugPrimitive = pointLight->getDebugPrimitive();
	pointLightDebugPrimitive->setMaterial(colorMaterial);
	demoContext.scene.add(*pointLightDebugPrimitive.get());

	spotLight = std::make_shared<SpotLight>();
	spotLight->setRadius(0.0f, 75.0f);
	spotLight->setPosition({ 0.0f, 0.0f, 0.0f });
	demoContext.scene.add(*spotLight);

	auto spotLightDebugPrimitive = spotLight->getDebugPrimitive();
	spotLightDebugPrimitive->setMaterial(colorMaterial);
	demoContext.scene.add(*spotLightDebugPrimitive.get());

	cameraLight = std::make_shared<SpotLight>();
	cameraLight->setCone(glm::radians(35.0f), glm::radians(40.0f));
	cameraLight->setRadius(0.0f, 10000.0f);
	demoContext.scene.add(*cameraLight);



	ShapeParameters skyboxShapeParams;
	skyboxShapeParams.generateNormals = false;
	skyboxShapeParams.invertNormals = true;
	skyboxShapeParams.generateTangents = false;

	//skybox.generate(ShapeType::Box, skyboxShapeParams, &demoContext.shapeGenerator);
	//skybox.disableRenderFlags(RenderFlag::CullBackFace);
	//skybox.disableRenderFlags(RenderFlag::WriteDepth);
	//skybox.disableRenderFlags(RenderFlag::DepthTestLess);
	//skybox.enableRenderFlags(RenderFlag::DepthTestLessOrEqual);
	//skybox.setMaterial(skyboxMaterial);
	//// NOTE: rotating skybox does not work with static render mode!
	////skybox.setRenderMode(RenderMode::Static);
	////skybox.setPosition(camera.getPosition());
	//demoContext.scene.add(skybox);


	ShapeParameters boxShapeParams;
	boxShapeParams.generateNormals = true;
	boxShapeParams.invertNormals = true;
	boxShapeParams.generateTangents = true;

	boxShape.generate(ShapeType::Cube, boxShapeParams, &demoContext.shapeGenerator);
	boxShape.disableRenderFlags(RenderFlag::CullBackFace);
	boxShape.enableRenderFlags(RenderFlag::CullFrontFace);
	boxShape.setMaterial(testMaterial);
	boxShape.setScale(glm::vec3(50.0f));
	boxShape.setPosition({ 0.0f, 0.0f, 0.0f });
	boxShape.setColor(se::Color());
	boxShape.setRenderMode(RenderMode::Batched);
	demoContext.scene.add(boxShape);

	ShapeParameters defaultShapeParams;
	defaultShapeParams.generateNormals = true;
	defaultShapeParams.generateTangents = true;

	planeShape.generate(ShapeType::Square, defaultShapeParams, &demoContext.shapeGenerator);
	planeShape.disableRenderFlags(RenderFlag::CullBackFace);
	planeShape.setMaterial(stoneMaterial);
	planeShape.setScale(glm::vec3(20.0f));
	demoContext.scene.add(planeShape);

	testShape.generate(ShapeType::Sphere, defaultShapeParams, &demoContext.shapeGenerator);
	testShape.setMaterial(flatPhongMaterial);
	testShape.setScale(glm::vec3(3.0f));
	testShape.setPosition({ 10.0f, -7.0f, 0.0f });
	demoContext.scene.add(testShape);

	originX.addPoint(glm::vec3(0.0f));
	originX.addPoint(glm::vec3(1.0f, 0.0f, 0.0f));
	originX.setColor(se::Color(se::HexColor::Red));
	originX.setMaterial(colorMaterial);
	demoContext.scene.add(originX);
	originZ.addPoint(glm::vec3(0.0f));
	originZ.addPoint(glm::vec3(0.0f, 0.0f, 1.0f));
	originZ.setColor(se::Color(se::HexColor::Blue));
	originZ.setMaterial(colorMaterial);
	demoContext.scene.add(originZ);
	originY.addPoint(glm::vec3(0.0f));
	originY.addPoint(glm::vec3(0.0f, 1.0f, 0.0f));
	originY.setColor(se::Color(se::HexColor::Green));
	originY.setMaterial(colorMaterial);
	demoContext.scene.add(originY);

	testModel.loadModelAsset(testModelData);
	testModel.setMaterial(testMaterial);
	demoContext.scene.add(testModel);

	InstanceBuffer<TransformInstanceData> modelInstances;
	{
		constexpr int size = 6;
		modelInstances.resize(size * size);
		size_t index = 0;
		for (int x = -size / 2; x < size / 2; x++)
		{
			for (int z = -size / 2; z < size / 2; z++)
			{
				TransformInstanceData data;
				data.setPosition(glm::vec3((float)x * 3.0f, 0.0f, (float)z * 3.0f));
				modelInstances.set(index++, data);
			}
		}
	}

	testModel2.loadModelAsset(icosphereModelData);
	testModel2.setMaterial(phongMaterial);
	testModel2.setPosition(glm::vec3(4.0f, -20.0f, 4.0f));
	testModel2.setInstances(modelInstances.getBuffer());
	testModel2.setPrimitiveType(PrimitiveType::Points);
	demoContext.scene.add(testModel2);

	animModel.loadModelAsset(animModelData);
	animModel.setMaterial(testMaterial);
	animModel.setPosition(glm::vec3(0.0f, -20.0f, 0.0f));
	animModel.getAnimator().start("Walk");
	demoContext.scene.add(animModel);

	jumpModel.loadModelAsset(jumpModelData);
	jumpModel.setMaterial(phongMaterial);
	jumpModel.setColor(se::Color(se::HexColor::Coral));
	jumpModel.setPosition(glm::vec3(15.0f, -25.0f, 15.0f));
	jumpModel.getAnimator().start("Jump");
	jumpModel.setInstances(modelInstances.getBuffer());
	demoContext.scene.add(jumpModel);

	ballTestModel.loadModelAsset(rotorTestModelData);
	ballTestModel.setMaterial(phongMaterial);
	ballTestModel.setPosition(glm::vec3(-15.0f, -25.0f, -15.0f));
	ballTestModel.getAnimator().start("rotor_move.L");
	demoContext.scene.add(ballTestModel);

	demonModel.loadModelAsset(demonModelData);
	//demonModel.setMaterial(demonMaterial);
	demonModel.setMaterial(phongMaterial);
	demonModel.setPosition(glm::vec3(-15.0f, -25.0f, 15.0f));
	demonModel.setInstances(modelInstances.getBuffer());
	demoContext.scene.add(demonModel);

	simpleModel.loadModelAsset(simpleModelData);
	simpleModel.setMaterial(phongMaterial);
	simpleModel.setPosition(glm::vec3(-15.0f, -25.0f, 0.0f));
	simpleModel.getAnimator().start("Wobble");
	demoContext.scene.add(simpleModel);

	{
		constexpr int size = 30;
		shapeInstances.resize(size * size);
		size_t index = 0;
		for (int x = -size / 2; x < size / 2; x++)
		{
			for (int z = -size / 2; z < size / 2; z++)
			{
				TransformInstanceData data;
				data.setPosition(glm::vec3((float)x * 3.0f, -100.0f, (float)z * 3.0f));
				shapeInstances.set(index++, data);
			}
		}
	}

	instanceShape.generate(ShapeType::Cube, defaultShapeParams, &demoContext.shapeGenerator);
	instanceShape.setMaterial(phongMaterial);
	demoContext.scene.add(instanceShape);
	instanceShape.setInstances(shapeInstances.getBuffer());

	demoContext.showWindowDefault();
}

bool GraphicsPlayground::update()
{
	const double timeNowSeconds = se::time::now().asSeconds<double>();

	{
		ImGfx::sphere()
			.color(se::Color(se::Red))
			.position({ 0.0f, 6.0f + (float)cos(timeNowSeconds), 0.0f })
			.scale(glm::vec3(0.6f))
			.primitiveType(PrimitiveType::Lines);

		ImGfx::icon("test_color.png")
			.position({ 4.0f, 7.0f, 0.0f })
			.color(se::Color(se::LightGoldenrodYellow));

		ImGfx::line({}, { 10.0f, 0.0f, 0.0f })
			.color(se::Color(se::GreenYellow).withAlpha(0.5f))
			.renderFlags(RenderFlag::BlendAlpha);

		for (size_t i = 0; i < 100; i++)
		{
			ImGfx::linePoint({ 0.1f * (float)i, 15.0f + (float)cos(timeNowSeconds * 2.0 + (float)i * 0.2f), 0.0f }, "wave")
				.color(se::Color(se::Violet));
		}

		ImGfx::text("ImGfx::text testing...")
			.position({0.0f, 7.0f, 5.0f})
			.color(se::Color(se::DodgerBlue))
			.scale(glm::vec3(0.03f));

		ImGfx::model("icosphere.fbx")
			.position({ 6.0f, 8.0f, 6.0f })
			.material(flatMaterial)
			.color(se::Color(se::Orange))
			.scale(glm::vec3(2.0f));
	}

	if (objects.size() < 30 &&
		se::time::now() - lastObjectSpawned > se::time::fromSeconds(0.1f))
	{
		objects.push_back(std::make_unique<ShapeObject>(demoContext.scene, flatMaterial, colorMaterial, demoContext.shapeGenerator));
		lastObjectSpawned = se::time::now();
	}
	else if (objects.size() > 10 && se::time::now() - lastObjectDeleted > se::time::fromSeconds(5.0f))
	{
		objects.erase(objects.begin() + se::rng::random<size_t>(0, objects.size() - 1));
		lastObjectDeleted = se::time::now();

		if (observerWindowSupported)
		{
			view2Active = !view2Active;
			if (view2Active)
			{
				observerWindow.add(observerView2);
				observerWindow.setName("Observer Window (view 2 active)");
			}
			else
			{
				observerWindow.remove(observerView2);
				observerWindow.setName("Observer Window (view 2 inactive)");
			}
		}
	}

	{
		//pointLight.setPosition(glm::vec3(0.0f, -6.0f + (float)abs(sin(timeNowSeconds * 0.5)) * 6.0f, 0.0f));
		//pointLight.setRadius(pointLight.getInnerRadius(), 15.0f + 10.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
		//sunLight.setDirection(glm::vec3((float)sin(timeNowSeconds * 0.2) * 0.5f, -1.0f, (float)cos(timeNowSeconds * 0.2) * 0.5f));

		const glm::vec3 direction = glm::vec3((float)sin(timeNowSeconds * 0.4) * 0.5f, (float)cos(timeNowSeconds * 0.2) * 1.0f, (float)cos(timeNowSeconds * 0.3) * 0.5f);
		spotLight->setDirection(direction);
		const float cone = glm::radians(10.0f + 50.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
		spotLight->setCone(cone - glm::radians(1.0f), cone);

		planeShape.setPosition({ (float)sin(timeNowSeconds * 0.3) * 10.0f, -10.0f, (float)cos(timeNowSeconds * 0.3) * 10.0f });

		testModel.setPosition({ (float)cos(timeNowSeconds * 0.2), (float)sin(timeNowSeconds * 0.2), (float)cos(timeNowSeconds * 0.2) });
		testModel.setScale(glm::vec3(0.5f + fabsf((float)sin(timeNowSeconds * 0.3))));
		testModel.setRotation(glm::quatLookAt(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)));

		skybox.setRotation(glm::rotate(skybox.getRotation(), 0.01f * demoContext.deltaTimeSystem.deltaSeconds, glm::vec3(1.0f, 0.2f, 0.0f)));

		for (size_t i = 0; i < shapeInstances.size(); i++)
		{
			if (se::rng::weightedCoin(0.01))
			{
				TransformInstanceData data = shapeInstances.get(i);
				data.setRotation(se::rng::rotation());
				shapeInstances.set(i, data);
			}
		}
	}

	if (observerWindowSupported)
	{
		if (observerWindow.isQuitRequested())
		{
			observerWindow.ignoreQuitRequest();
			if (observerWindow.isShown())
			{
				observerWindow.hide();
				lastWindow2Hidden = se::time::now();
			}
		}
		else if (!observerWindow.isShown() && se::time::timeSince(lastWindow2Hidden) > se::time::fromSeconds(3.0f))
		{
			observerWindow.show();
		}
	}

	cameraController.update(demoContext.deltaTimeSystem.deltaTime);

	if (demoContext.inputManager.isKeyPressed((unsigned)se::input::Key::X))
	{
		cameraLight->setIntensity(cameraLight->getIntensity() == 0.0f ? 1.0f : 0.0f);
	}
	cameraLight->setPosition(demoContext.camera.getPosition());
	cameraLight->setDirection(demoContext.camera.getDirection());

	for (auto&& object : objects)
	{
		object->update(demoContext.deltaTimeSystem.deltaTime);
	}

	{
		const se::time::Time frameTime = se::time::now() - frameTimer;
		//Renderer::debugTextPrintf(1, 3, "frame time: %i", (int)frameTime.asMilliseconds());
		frameTimer = se::time::now();
	}

	constexpr se::time::Time runFade = se::time::fromMilliseconds(400);
	constexpr se::time::Time idleFade = se::time::fromMilliseconds(200);
	if (demoContext.inputManager.isKeyDown((unsigned)se::input::Key::SPACE))
	{
		if (!demonModel.getAnimator().isPlaying("Run"))
		{
			demonModel.getAnimator().stopAll(runFade);
			demonModel.getAnimator().start("Run", runFade);
			demonModel.getAnimator().setSpeed("Run", 2.0f);
		}
	}
	else if (!demonModel.getAnimator().isPlaying("Idle"))
	{
		demonModel.getAnimator().stopAll(idleFade);
		demonModel.getAnimator().start("Idle", idleFade);
	}

	if (demoContext.inputManager.isKeyPressed((unsigned)se::input::Key::F8))
	{
		demoContext.mainWindow.takeScreenShot("screenshot_" + std::to_string(frameN));
	}

	if (demoContext.inputManager.isKeyPressed((unsigned)se::input::Key::F5))
	{
		se::log::info("Reloading assets...", se::log::TextColor::BLUE);
		demoContext.assetManager.reload();
	}

	frameN++;
	return true;
}
