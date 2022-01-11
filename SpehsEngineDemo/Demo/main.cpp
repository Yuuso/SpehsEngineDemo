#include "stdafx.h"

#include "CameraController.h"
#include "DefaultResourcePathFinders.h"
#include "GUITest.h"
#include "ParticleSystem.h"
#include "Materials.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Core/HexColor.h"
#include "SpehsEngine/Core/Inifile.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/OS.h"
#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Core/Thread.h"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Model.h"
#include "SpehsEngine/Graphics/ModelDataManager.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/Graphics/View.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/GUI/GUILib.h"
#include "SpehsEngine/Input/EventCatcher.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"


int main()
{
	const auto initStart = se::time::now();

	se::CoreLib core;
	se::InputLib input;
	se::GraphicsLib graphics;
	se::GUILib gui;

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
										   //, se::graphics::RendererBackend::OpenGLES
									);

	se::graphics::Scene scene;
	se::graphics::Camera camera;
	se::graphics::View view(scene, camera);
	window1.add(view);
	camera.setFar(50000.0f);
	//camera.setProjection(se::graphics::Projection::Orthographic);
	//camera.setZoom(0.01f);

	se::graphics::Window window2;
	if (renderer.getRendererBackend() != se::graphics::RendererBackend::OpenGL &&
		renderer.getRendererBackend() != se::graphics::RendererBackend::Vulkan) // TODO: There has to be a way to fix this!
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

	std::shared_ptr<se::graphics::ResourceLoader> resourceLoader = se::graphics::makeResourceLoader(8);

	se::graphics::ShaderManager shaderManager;
	se::graphics::TextureManager textureManager;
	se::graphics::FontManager fontManager;
	se::graphics::ModelDataManager modelDataManager;

	std::shared_ptr<ShaderPathFinder> shaderPathFinder = std::make_shared<ShaderPathFinder>();
	shaderManager.setResourcePathFinder(shaderPathFinder);
	shaderManager.setResourceLoader(resourceLoader);
	shaderManager.createDefaultShaders();

	std::shared_ptr<TexturePathFinder> texturePathFinder = std::make_shared<TexturePathFinder>();
	textureManager.setResourcePathFinder(texturePathFinder);
	textureManager.setResourceLoader(resourceLoader);

	std::shared_ptr<FontPathFinder> fontPathFinder = std::make_shared<FontPathFinder>();
	fontManager.setResourcePathFinder(fontPathFinder);
	fontManager.setResourceLoader(resourceLoader);
	fontManager.createDefaultFonts();

	std::shared_ptr<ModelPathFinder> modelPathFinder = std::make_shared<ModelPathFinder>();
	modelDataManager.setResourcePathFinder(modelPathFinder);
	modelDataManager.setResourceLoader(resourceLoader);

	shaderManager.create("test", "vs_test.bin", "fs_test.bin");
	shaderManager.create("test_anim", "vs_test_anim.bin", "fs_test.bin");

	auto skyboxColor = textureManager.create("skybox", "skybox.ktx");

	auto testColor = textureManager.create("testColor", "test_color.png");
	auto testNormal = textureManager.create("testNormal", "test_normal.png");

	auto demonColor = textureManager.create("balldemon", "balldemon.png");

	auto stoneColor = textureManager.create("stoneColor", "stone_color.png");
	auto stoneNormal = textureManager.create("stoneNormal", "stone_normal.png");

	auto testFont = fontManager.create("test", "open-sans.regular.ttf", se::graphics::FontSize(32, se::graphics::FontSizeType::Pixel), se::graphics::defaultCharacterSet);
	auto embeddedFont = fontManager.find("AnonymousPro-Regular");

	auto testModelData = modelDataManager.create("test", "test.fbx");
	auto animModelData = modelDataManager.create("anim", "walking_cube.gltf");
	auto jumpModelData = modelDataManager.create("jump", "jumping_cube.glb");
	auto demonModelData = modelDataManager.create("demon", "ball_demon.gltf");
	auto rotorTestModelData = modelDataManager.create("rotortest", "rotor_test.glb");
	auto icosphereModelData = modelDataManager.create("ico", "icosphere.fbx");
	auto simpleModelData = modelDataManager.create("simple", "simple.gltf");

	se::graphics::TextureModes genModes;
	genModes.sampleMin = se::graphics::TextureSamplingMode::Point;
	genModes.sampleMag = se::graphics::TextureSamplingMode::Point;
	genModes.sampleMip = se::graphics::TextureSamplingMode::Point;

	se::graphics::TextureInput textureInput;
	textureInput.width = 2;
	textureInput.height = 2;
	textureInput.data = {	255,	0,		0,		255,
							0,		255,	0,		255,
							0,		0,		255,	255,
							255,	0,		255,	255, };
	auto genTexture = textureManager.create("genTest", textureInput, genModes);

	textureInput.width = 1;
	textureInput.height = 1;
	textureInput.data = { 128, 128, 255, 255 };
	auto flatNormalTexture = textureManager.create("flatNormal", textureInput, genModes);

	textureInput.width = 1;
	textureInput.height = 1;
	textureInput.data = { 255, 255, 255, 255 };
	auto whiteTexture = textureManager.create("whiteColor", textureInput, genModes);

	se::graphics::TextureInput blackTextureInput;
	blackTextureInput.width = 1;
	blackTextureInput.height = 1;
	blackTextureInput.data = {0, 0, 0, 0};
	auto blackTexture = textureManager.create("blackTexture", blackTextureInput, genModes);

	std::shared_ptr<se::graphics::Material> colorMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::FlatColor, shaderManager);

	std::shared_ptr<se::graphics::Material> flatMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::FlatTexture, shaderManager);
	flatMaterial->setTexture(genTexture);

	std::shared_ptr<se::graphics::Material> phongMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::Phong, shaderManager);
	phongMaterial->setTexture(testColor, se::graphics::PhongTextureType::Color);
	phongMaterial->setTexture(testNormal, se::graphics::PhongTextureType::Normal);

	std::shared_ptr<se::graphics::Material> testMaterial = createTestMaterial(shaderManager);
	testMaterial->setTexture(testColor, se::graphics::PhongTextureType::Color);
	testMaterial->setTexture(testNormal, se::graphics::PhongTextureType::Normal);

	std::shared_ptr<se::graphics::Material> demonMaterial = createTestMaterial(shaderManager);
	demonMaterial->setTexture(demonColor, se::graphics::PhongTextureType::Color);
	demonMaterial->setTexture(flatNormalTexture, se::graphics::PhongTextureType::Normal);

	std::shared_ptr<se::graphics::Material> stoneMaterial = createTestMaterial(shaderManager);
	stoneMaterial->setTexture(stoneColor, se::graphics::PhongTextureType::Color);
	stoneMaterial->setTexture(stoneNormal, se::graphics::PhongTextureType::Normal);

	std::shared_ptr<se::graphics::Material> flatPhongMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::Phong, shaderManager);
	flatPhongMaterial->setTexture(whiteTexture, se::graphics::PhongTextureType::Color);
	flatPhongMaterial->setTexture(flatNormalTexture, se::graphics::PhongTextureType::Normal);

	std::shared_ptr<se::graphics::Material> textMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::Text, shaderManager);
	textMaterial->setFont(testFont);

	std::shared_ptr<se::graphics::Material> embeddedTextMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::Text, shaderManager);
	embeddedTextMaterial->setFont(embeddedFont);

	std::shared_ptr<se::graphics::Material> skyboxMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::Skybox, shaderManager);
	skyboxMaterial->setTexture(skyboxColor);


	//testFont->waitUntilReady();

	GUITest guitest(window1, shaderManager, textureManager, fontManager, eventSignaler);

	se::graphics::Text testText;
	se::graphics::TextStyle style;
	testText.setMaterial(textMaterial);
	//testText.insert("\n");
	//testText.insert("|\n");
	testText.insert("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	testText.insert("abcdrfghijklmnopqrstuvwxyz\n");
	style.color = se::Color(se::Red);
	testText.setStyle(style);
	testText.insert("1234567890\n");
	testText.insert(U"Á\n");
	testText.setScale(glm::vec3(0.02f));
	testText.setPosition(glm::vec3(0.0f, -24.99f, 0.0f));
	testText.enableRenderFlags(se::graphics::RenderFlag::DepthTestLess);
	scene.add(testText);

	//se::graphics::Text embeddedFontText;
	//embeddedFontText.setMaterial(embeddedTextMaterial);
	//embeddedFontText.insert("abcdrfghijklmnopqrstuvwxyz\n");
	//scene.add(embeddedFontText);

	se::graphics::AmbientLight ambientLight(se::Color(se::White), 0.05f);
	scene.add(ambientLight);

	se::graphics::PointLight pointLight;
	pointLight.setRadius(0.0f, 40.0f);
	scene.add(pointLight);

	std::shared_ptr<se::graphics::Primitive> pointLightDebugPrimitive = pointLight.getDebugPrimitive();
	pointLightDebugPrimitive->setMaterial(colorMaterial);
	scene.add(*pointLightDebugPrimitive.get());

	//se::graphics::DirectionalLight sunLight;
	//sunLight.setIntensity(0.2f);
	//scene.add(sunLight);

	se::graphics::SpotLight spotLight;
	spotLight.setRadius(0.0f, 75.0f);
	spotLight.setPosition({ 0.0f, 0.0f, 0.0f });
	scene.add(spotLight);

	std::shared_ptr<se::graphics::Primitive> spotLightDebugPrimitive = spotLight.getDebugPrimitive();
	spotLightDebugPrimitive->setMaterial(colorMaterial);
	scene.add(*spotLightDebugPrimitive.get());


	se::graphics::SpotLight cameraLight;
	cameraLight.setCone(glm::radians(35.0f), glm::radians(40.0f));
	cameraLight.setRadius(0.0f, 10000.0f);
	//cameraLight.setIntensity(0.8f);
	scene.add(cameraLight);


	class ShapeObject
	{
	public:

		ShapeObject(se::graphics::Scene& _scene,
					std::shared_ptr<se::graphics::Material> _material,
					std::shared_ptr<se::graphics::Material> _trailMaterial,
					se::graphics::ShapeGenerator& _shapeGenerator)
			: shapeGenerator(_shapeGenerator)
		{
			shape.setMaterial(_material);
			shape.enableRenderFlags(se::graphics::RenderFlag::BillboardSpherical);
			trail.setMaterial(_trailMaterial);
			trail.enableRenderFlags(se::graphics::RenderFlag::BlendAlpha);
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
			//shape.setRenderMode(se::graphics::RenderMode::Static);
			//shape.setPrimitiveType(se::graphics::PrimitiveType::Triangles);
			init();
			trail.addPoint(shape.getPosition());
		}
		void update(const se::time::Time _deltaTime)
		{
			velocity = glm::max(0.0f, velocity - 1.0f * _deltaTime.asSeconds());
			angularVelocity = glm::max(0.0f, angularVelocity - 3.0f * _deltaTime.asSeconds());

			shape.setPosition(shape.getPosition() + direction * velocity * _deltaTime.asSeconds());
			if (!shape.checkRenderFlags(se::graphics::RenderFlag::BillboardSpherical) &&
				!shape.checkRenderFlags(se::graphics::RenderFlag::BillboardCylindrical))
				shape.setRotation(glm::rotate(shape.getRotation(), angularVelocity * _deltaTime.asSeconds(), axis));

			trail.getPoint(trail.numPoints() - 1) = shape.getPosition();

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

			//shape.setRenderMode((se::graphics::RenderMode)se::rng::random(0, 1));
			shape.setPrimitiveType((se::graphics::PrimitiveType)se::rng::random(1, 3), shapeGenerator);
			shape.generate((se::graphics::ShapeType)se::rng::random(3, 13), shapeParams, &shapeGenerator);
			shape.disableRenderFlags(se::graphics::RenderFlag::CullBackFace);

			trail.addPoint(shape.getPosition());
		}

		se::graphics::ShapeParameters shapeParams;
		se::graphics::ShapeGenerator& shapeGenerator;
		se::graphics::Shape shape;
		se::graphics::Line trail;
		glm::vec3 direction;
		glm::vec3 axis;
		float velocity;
		float angularVelocity;
	};
	std::vector<std::unique_ptr<ShapeObject>> objects;

	se::graphics::ShapeGenerator shapeGenerator;
	se::graphics::ShapeParameters defaultShapeParams;
	defaultShapeParams.generateNormals = true;
	defaultShapeParams.generateTangents = true;


	se::graphics::ShapeParameters skyboxShapeParams;
	skyboxShapeParams.generateNormals = false;
	skyboxShapeParams.invertNormals = true;
	skyboxShapeParams.generateTangents = false;
	se::graphics::Shape skybox;
	skybox.generate(se::graphics::ShapeType::Box, skyboxShapeParams, &shapeGenerator);
	skybox.disableRenderFlags(se::graphics::RenderFlag::CullBackFace);
	skybox.disableRenderFlags(se::graphics::RenderFlag::WriteDepth);
	skybox.disableRenderFlags(se::graphics::RenderFlag::DepthTestLess);
	skybox.enableRenderFlags(se::graphics::RenderFlag::DepthTestLessOrEqual);
	skybox.setMaterial(skyboxMaterial);
	// NOTE: rotating skybox does not work with static render mode!
	//skybox.setRenderMode(se::graphics::RenderMode::Static);
	//skybox.setPosition(camera.getPosition());
	scene.add(skybox);

	// TODO: Cubemap textures don't work with temp textures!
	skyboxColor->waitUntilReady();


	se::graphics::ShapeParameters boxShapeParams;
	boxShapeParams.generateNormals = true;
	boxShapeParams.invertNormals = true;
	boxShapeParams.generateTangents = true;
	se::graphics::Shape boxShape;
	boxShape.generate(se::graphics::ShapeType::Cube, boxShapeParams, &shapeGenerator);
	boxShape.disableRenderFlags(se::graphics::RenderFlag::CullBackFace);
	boxShape.enableRenderFlags(se::graphics::RenderFlag::CullFrontFace);
	boxShape.setMaterial(testMaterial);
	boxShape.setScale(glm::vec3(50.0f));
	boxShape.setPosition({ 0.0f, 0.0f, 0.0f });
	boxShape.setColor(se::Color());
	boxShape.setRenderMode(se::graphics::RenderMode::Static);
	scene.add(boxShape);

	se::graphics::Shape planeShape;
	planeShape.generate(se::graphics::ShapeType::Square, defaultShapeParams, &shapeGenerator);
	planeShape.disableRenderFlags(se::graphics::RenderFlag::CullBackFace);
	planeShape.setMaterial(stoneMaterial);
	planeShape.setScale(glm::vec3(20.0f));
	scene.add(planeShape);

	se::graphics::Shape testShape;
	testShape.generate(se::graphics::ShapeType::Sphere, defaultShapeParams, &shapeGenerator);
	testShape.setMaterial(flatPhongMaterial);
	testShape.setScale(glm::vec3(3.0f));
	testShape.setPosition({ 10.0f, -7.0f, 0.0f });
	scene.add(testShape);

	se::graphics::Line originX;
	originX.addPoint(glm::vec3(0.0f));
	originX.addPoint(glm::vec3(1.0f, 0.0f, 0.0f));
	originX.setColor(se::Color(se::HexColor::Red));
	originX.setMaterial(colorMaterial);
	scene.add(originX);
	se::graphics::Line originZ;
	originZ.addPoint(glm::vec3(0.0f));
	originZ.addPoint(glm::vec3(0.0f, 0.0f, 1.0f));
	originZ.setColor(se::Color(se::HexColor::Blue));
	originZ.setMaterial(colorMaterial);
	scene.add(originZ);
	se::graphics::Line originY;
	originY.addPoint(glm::vec3(0.0f));
	originY.addPoint(glm::vec3(0.0f, 1.0f, 0.0f));
	originY.setColor(se::Color(se::HexColor::Green));
	originY.setMaterial(colorMaterial);
	scene.add(originY);

	se::graphics::Model testModel;
	testModel.loadModelData(testModelData);
	testModel.setMaterial(testMaterial);
	scene.add(testModel);

	se::graphics::InstanceBuffer<se::graphics::TransformInstanceData> modelInstances;
	{
		constexpr int size = 6;
		modelInstances.resize(size * size);
		size_t index = 0;
		for (int x = -size / 2; x < size / 2; x++)
		{
			for (int z = -size / 2; z < size / 2; z++)
			{
				se::graphics::TransformInstanceData data;
				data.setPosition(glm::vec3((float)x * 3.0f, 0.0f, (float)z * 3.0f));
				modelInstances.set(index++, data);
			}
		}
	}

	se::graphics::Model testModel2;
	testModel2.loadModelData(icosphereModelData);
	testModel2.setMaterial(phongMaterial);
	testModel2.setPosition(glm::vec3(4.0f, -20.0f, 4.0f));
	testModel2.setInstances(modelInstances.getBuffer());
	testModel2.setPrimitiveType(se::graphics::PrimitiveType::Points);
	scene.add(testModel2);

	se::graphics::Model animModel;
	animModel.loadModelData(animModelData);
	animModel.setMaterial(testMaterial);
	animModel.setPosition(glm::vec3(0.0f, -20.0f, 0.0f));
	animModel.getAnimator().start("Walk");
	scene.add(animModel);

	se::graphics::Model jumpModel;
	jumpModel.loadModelData(jumpModelData);
	jumpModel.setMaterial(phongMaterial);
	jumpModel.setColor(se::Color(se::HexColor::Coral));
	jumpModel.setPosition(glm::vec3(15.0f, -25.0f, 15.0f));
	jumpModel.getAnimator().start("Jump");
	jumpModel.setInstances(modelInstances.getBuffer());
	scene.add(jumpModel);

	se::graphics::Model ballTestModel;
	ballTestModel.loadModelData(rotorTestModelData);
	ballTestModel.setMaterial(phongMaterial);
	ballTestModel.setPosition(glm::vec3(-15.0f, -25.0f, -15.0f));
	ballTestModel.getAnimator().start("rotor_move.L");
	scene.add(ballTestModel);

	se::graphics::Model demonModel;
	demonModel.loadModelData(demonModelData);
	//demonModel.setMaterial(demonMaterial);
	demonModel.setMaterial(phongMaterial);
	demonModel.setPosition(glm::vec3(-15.0f, -25.0f, 15.0f));
	demonModel.setInstances(modelInstances.getBuffer());
	scene.add(demonModel);

	se::graphics::Model simpleModel;
	simpleModel.loadModelData(simpleModelData);
	simpleModel.setMaterial(phongMaterial);
	simpleModel.setPosition(glm::vec3(-15.0f, -25.0f, 0.0f));
	simpleModel.getAnimator().start("Wobble");
	scene.add(simpleModel);

	se::graphics::InstanceBuffer<se::graphics::TransformInstanceData> shapeInstances;
	{
		constexpr int size = 30;
		shapeInstances.resize(size * size);
		size_t index = 0;
		for (int x = -size / 2; x < size / 2; x++)
		{
			for (int z = -size / 2; z < size / 2; z++)
			{
				se::graphics::TransformInstanceData data;
				data.setPosition(glm::vec3((float)x * 3.0f, -100.0f, (float)z * 3.0f));
				shapeInstances.set(index++, data);
			}
		}
	}

	se::graphics::Shape instanceShape;
	instanceShape.generate(se::graphics::ShapeType::Cube, defaultShapeParams, &shapeGenerator);
	instanceShape.setMaterial(phongMaterial);
	scene.add(instanceShape);
	instanceShape.setInstances(shapeInstances.getBuffer());

	ParticleSystem particleSystem(scene, shaderManager, textureManager, shapeGenerator);

	se::imgui::BackendWrapper imguiBackend(eventSignaler, 0, renderer);

	const auto initTime = se::time::now() - initStart;
	se::log::info("Init time: " + std::to_string(initTime.asSeconds()) + " seconds", se::log::GREEN);

	//while (!shaderManager.allShadersReady()) shaderManager.update();

	int frameN = 0;
	se::time::Time lastObjectSpawned = se::time::Time::zero;
	se::time::Time lastObjectDeleted = se::time::Time::zero;
	se::time::Time lastWindow2Hidden = se::time::Time::zero;
	se::time::Time frameTimer = se::time::Time::zero;
	while (true)
	{
		if (objects.size() < 30 &&
			se::time::now() - lastObjectSpawned > se::time::fromSeconds(0.1f))
		{
			objects.push_back(std::make_unique<ShapeObject>(scene, flatMaterial, colorMaterial, shapeGenerator));
			lastObjectSpawned = se::time::now();
		}
		else if (objects.size() > 10 && se::time::now() - lastObjectDeleted > se::time::fromSeconds(5.0f))
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
		}

		{
			const double timeNowSeconds = se::time::now().asSeconds<double>();
			//pointLight.setPosition(glm::vec3(0.0f, -6.0f + (float)abs(sin(timeNowSeconds * 0.5)) * 6.0f, 0.0f));
			//pointLight.setRadius(pointLight.getInnerRadius(), 15.0f + 10.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
			//sunLight.setDirection(glm::vec3((float)sin(timeNowSeconds * 0.2) * 0.5f, -1.0f, (float)cos(timeNowSeconds * 0.2) * 0.5f));

			const glm::vec3 direction = glm::vec3((float)sin(timeNowSeconds * 0.4) * 0.5f, (float)cos(timeNowSeconds * 0.2) * 1.0f, (float)cos(timeNowSeconds * 0.3) * 0.5f);
			spotLight.setDirection(direction);
			const float cone = glm::radians(10.0f + 50.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
			spotLight.setCone(cone - glm::radians(1.0f), cone);

			planeShape.setPosition({ (float)sin(timeNowSeconds * 0.3) * 10.0f, -10.0f, (float)cos(timeNowSeconds * 0.3) * 10.0f });

			testModel.setPosition({ (float)cos(timeNowSeconds * 0.2), (float)sin(timeNowSeconds * 0.2), (float)cos(timeNowSeconds * 0.2) });
			testModel.setScale(glm::vec3(0.5f + fabsf((float)sin(timeNowSeconds * 0.3))));
			testModel.setRotation(glm::quatLookAt(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)));

			skybox.setRotation(glm::rotate(skybox.getRotation(), 0.01f * deltaTimeSystem.deltaSeconds, glm::vec3(1.0f, 0.2f, 0.0f)));

			for (size_t i = 0; i < shapeInstances.size(); i++)
			{
				if (se::rng::weightedCoin(0.01))
				{
					se::graphics::TransformInstanceData data = shapeInstances.get(i);
					data.setRotation(se::rng::rotation());
					shapeInstances.set(i, data);
				}
			}
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

		deltaTimeSystem.update();
		//inifile.update();

		shaderManager.update();
		textureManager.update();
		fontManager.update();
		modelDataManager.update();

		eventCatcher.pollEvents();
		eventSignaler.signalEvents(eventCatcher);
		inputManager.update(eventCatcher);

		ImGui::SetNextWindowPos({ 50.0f, 100.0f }, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize({ 300.0f, 80.0f }, ImGuiCond_FirstUseEver);
		{
			ImGui::ScopedFont monoFont(se::imgui::ImGuiFont::Mono);
			ImGui::Begin("TEST Testing 123");
		}
		ImGui::Text("Lorem ipsum 456\nNewline");
		ImGui::End();

		ImGui::SetNextWindowPos({ 50.0f, 500.0f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 300.0f, 300.0f }, ImGuiCond_Once);
		if (ImGui::Begin("More testing"))
		{
			ImGui::Image(*stoneColor, { 0.2f, 0.2f });
			if (ImGui::ImageButton(*genTexture, { 20.0f, 20.0f }))
			{
				se::log::info("button test");
			}
		}
		ImGui::End();

		cameraController.update(deltaTimeSystem.deltaTime);

		if (inputManager.isKeyPressed((unsigned)se::input::Key::X))
		{
			cameraLight.setIntensity(cameraLight.getIntensity() == 0.0f ? 1.0f : 0.0f);
		}
		cameraLight.setPosition(camera.getPosition());
		cameraLight.setDirection(camera.getDirection());

		particleSystem.update(deltaTimeSystem.deltaTime);
		for (auto&& object : objects)
			object->update(deltaTimeSystem.deltaTime);

		se::graphics::Renderer::debugTextPrintf(1, 1, "loading: %u", resourceLoader->numTasksLeft());
		se::graphics::Renderer::debugTextPrintf(1, 2, "frame: %i", frameN++);
		{
			const se::time::Time frameTime = se::time::now() - frameTimer;
			se::graphics::Renderer::debugTextPrintf(1, 3, "frame time: %i", (int)frameTime.asMilliseconds());
			frameTimer = se::time::now();
		}

		constexpr se::time::Time runFade = se::time::fromMilliseconds(400);
		constexpr se::time::Time idleFade = se::time::fromMilliseconds(200);
		if (inputManager.isKeyDown((unsigned)se::input::Key::SPACE))
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

		if (inputManager.isKeyPressed((unsigned)se::input::Key::F8))
		{
			window1.requestScreenShot("screenshot_" + std::to_string(frameN));
		}

		guitest.update(deltaTimeSystem.deltaTime);

		imguiBackend.render();
		renderer.render();

		if (inputManager.isKeyPressed((unsigned)se::input::Key::F5))
		{
			se::log::info("Reloading shaders...", se::log::TextColor::BLUE);
			//shaderManager.purgeUnusedShaders();
			shaderManager.reload();
		}

		if (inputManager.isKeyPressed((unsigned)se::input::Key::F4))
		{
			se::log::info("Reloading textures and models...", se::log::TextColor::BLUE);
			textureManager.reload();
			modelDataManager.reload();
		}

		if (inputManager.isQuitRequested())
			break;
	}

	return 0;
}