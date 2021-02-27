#include "stdafx.h"

#include "CameraController.h"
#include "DefaultResourcePathFinders.h"
#include "Materials.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Core/Console.h"
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
#include "SpehsEngine/Graphics/DefaultShaderManager.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Line.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "SpehsEngine/Graphics/Scene.h"
#include "SpehsEngine/Graphics/Shape.h"
#include "SpehsEngine/Graphics/Text.h"
#include "SpehsEngine/Graphics/TextureManager.h"
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
										   //, se::graphics::RendererBackend::OpenGL
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

	std::shared_ptr<se::graphics::ResourceLoader> resourceLoader = se::graphics::makeResourceLoader();

	se::graphics::DefaultShaderManager shaderManager;
	se::graphics::TextureManager textureManager;
	se::graphics::FontManager fontManager;

	std::shared_ptr<ShaderPathFinder> shaderPathFinder = std::make_shared<ShaderPathFinder>();
	shaderManager.setResourcePathFinder(shaderPathFinder);
	shaderManager.setResourceLoader(resourceLoader);

	std::shared_ptr<TexturePathFinder> texturePathFinder = std::make_shared<TexturePathFinder>();
	textureManager.setResourcePathFinder(texturePathFinder);
	textureManager.setResourceLoader(resourceLoader);

	std::shared_ptr<FontPathFinder> fontPathFinder = std::make_shared<FontPathFinder>();
	fontManager.setResourcePathFinder(fontPathFinder);
	fontManager.setResourceLoader(resourceLoader);

	auto testShader = shaderManager.create("test", "vs_test.bin", "fs_test.bin");

	auto testColor = textureManager.create("testColor", "test_color.png");
	auto testNormal = textureManager.create("testNormal", "test_normal.png");

	auto testFont = fontManager.create("test", "open-sans.regular.ttf", se::graphics::FontSize(32, se::graphics::FontSizeType::Pixel), se::graphics::defaultCharacterSet);
	//while (!testFont->ready())
	//{
	//	fontManager.update();
	//	std::this_thread::sleep_for(std::chrono::seconds(1));
	//}
	//auto testFontTexture = testFont->getDebugTexture();

	se::graphics::TextureInput textureInput;
	textureInput.width = 2;
	textureInput.height = 2;
	textureInput.data = {	255,	0,		0,		255,
							0,		255,	0,		255,
							0,		0,		255,	255,
							255,	0,		255,	255, };
	se::graphics::TextureModes genModes;
	genModes.sampleMin = se::graphics::TextureSamplingMode::Point;
	genModes.sampleMag = se::graphics::TextureSamplingMode::Point;
	genModes.sampleMip = se::graphics::TextureSamplingMode::Point;
	auto genTexture = textureManager.create("genTest", textureInput, genModes);

	std::shared_ptr<se::graphics::FlatColorMaterial> colorMaterial = std::make_unique<se::graphics::FlatColorMaterial>(shaderManager);

	std::shared_ptr<se::graphics::FlatTextureMaterial> flatMaterial = std::make_unique<se::graphics::FlatTextureMaterial>(shaderManager);
	flatMaterial->setTexture(genTexture);

	std::shared_ptr<se::graphics::PhongMaterial> phongMaterial = std::make_unique<se::graphics::PhongMaterial>(shaderManager);
	phongMaterial->setTexture(se::graphics::MaterialTextureType::Color, testColor);
	phongMaterial->setTexture(se::graphics::MaterialTextureType::Normal, testNormal);

	std::shared_ptr<TestMaterial> testMaterial = std::make_unique<TestMaterial>(shaderManager);
	testMaterial->setTexture(se::graphics::MaterialTextureType::Color, testColor);
	testMaterial->setTexture(se::graphics::MaterialTextureType::Normal, testNormal);

	std::shared_ptr<se::graphics::TextMaterial> textMaterial = std::make_unique<se::graphics::TextMaterial>(shaderManager);
	textMaterial->setFont(testFont);

	se::graphics::Text testText;
	se::graphics::TextStyle style;
	testText.setMaterial(textMaterial);
	//testText.insert("\n");
	//testText.insert("|\n");
	testText.insert("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
	testText.insert("abcdrfghijklmnopqrstuvwxyz\n");
	style.color = se::hexColor(se::Red);
	testText.setStyle(style);
	testText.insert("1234567890\n");
	testText.insert(u8"�\n");
	testText.setPosition({ -window1.getWidth() * 0.5f + 200.0f, 0.0f, -window1.getHeight() * 0.5f + 200.0f });
	hudScene.add(testText);

	se::graphics::AmbientLight ambientLight(se::hexColor(se::White), 0.2f);
	scene.add(ambientLight);

	se::graphics::PointLight pointLight;
	pointLight.setRadius(5.0f, 10.0f);
	pointLight.setIntensity(0.5f);
	scene.add(pointLight);
	pointLight.setIntensity(0.0f);

	std::shared_ptr<se::graphics::Primitive> pointLightDebugPrimitive = pointLight.getDebugPrimitive();
	pointLightDebugPrimitive->setMaterial(colorMaterial);
	scene.add(*pointLightDebugPrimitive.get());

	se::graphics::DirectionalLight sunLight;
	sunLight.setIntensity(0.2f);
	scene.add(sunLight);

	se::graphics::SpotLight spotLight;
	spotLight.setCone(glm::radians(9.0f), glm::radians(45.0f));
	spotLight.setRadius(52.0f, 52.0f);
	spotLight.setPosition({ 0.0f, 0.0f, 0.0f });
	scene.add(spotLight);

	std::shared_ptr<se::graphics::Primitive> spotLightDebugPrimitive = spotLight.getDebugPrimitive();
	spotLightDebugPrimitive->setMaterial(colorMaterial);
	scene.add(*spotLightDebugPrimitive.get());

	se::graphics::PointLight testLight;
	testLight.setRadius(12.0f, 12.0f);
	testLight.setIntensity(0.5f);
	scene.add(testLight);

	std::shared_ptr<se::graphics::Primitive> testLightDebugPrimitive = testLight.getDebugPrimitive();
	testLightDebugPrimitive->setMaterial(colorMaterial);
	scene.add(*testLightDebugPrimitive.get());

	class ShapeObject
	{
	public:

		ShapeObject(se::graphics::Scene& _scene, std::shared_ptr<se::graphics::Material> _material, std::shared_ptr<se::graphics::Material> _trailMaterial)
			: shape((se::graphics::ShapeType)se::rng::random(3, 12))
		{
			shape.setMaterial(_material);
			trail.setMaterial(_trailMaterial);
			trail.setRenderState(false);
			_scene.add(shape);
			_scene.add(trail);

			objectColor = se::randomBrightColor();
			shape.setColor(objectColor);
			shape.setScale(glm::vec3(se::rng::random(0.5f, 2.0f)));
			//shape.setRenderMode(se::graphics::RenderMode::Static);
			//shape.setPrimitiveType(se::graphics::PrimitiveType::Triangles);
			init();
		}
		void update(const se::time::Time _deltaTime)
		{
			velocity = glm::max(0.0f, velocity - 1.0f * _deltaTime.asSeconds());
			angularVelocity = glm::max(0.0f, angularVelocity - 3.0f * _deltaTime.asSeconds());

			shape.setPosition(shape.getPosition() + direction * velocity * _deltaTime.asSeconds());
			shape.setRotation(glm::rotate(shape.getRotation(), angularVelocity * _deltaTime.asSeconds(), axis));

			trailCounter -= _deltaTime;
			if (velocity > 0.1f && trailCounter <= se::time::Time::zero)
			{
				trailCounter = se::time::fromSeconds(0.3f);
				trail.addPoint(shape.getPosition(), objectColor);
			}

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

			shape.setRenderMode((se::graphics::RenderMode)se::rng::random(0, 2));
			shape.setPrimitiveType((se::graphics::PrimitiveType)se::rng::random(0, 2));
		}

		se::graphics::Shape shape;
		se::graphics::Line trail;
		se::time::Time trailCounter;
		se::Color objectColor;
		glm::vec3 direction;
		glm::vec3 axis;
		float velocity;
		float angularVelocity;
	};
	std::vector<std::unique_ptr<ShapeObject>> objects;

	se::graphics::Shape hudShape(se::graphics::ShapeType::Square);
	hudShape.setMaterial(flatMaterial);
	hudShape.setRenderMode(se::graphics::RenderMode::Dynamic);
	hudShape.setPrimitiveType(se::graphics::PrimitiveType::Triangles);
	hudShape.setColor(se::hexColor(se::Orange));
	hudShape.setScale(glm::vec3(20.0f));
	hudShape.setPosition({ -window1.getWidth() * 0.5f + 20.0f, 0.0f, -window1.getHeight() * 0.5f + 100.0f });
	hudScene.add(hudShape);

	se::graphics::Shape planeShape(se::graphics::ShapeType::Cube);
	planeShape.disableRenderFlag(se::graphics::RenderFlag::CullBackFace);
	planeShape.enableRenderFlag(se::graphics::RenderFlag::CullFrontFace);
	planeShape.setMaterial(testMaterial);
	planeShape.setScale(glm::vec3(50.0f));
	planeShape.setPosition({ 0.0f, 0.0f, 0.0f });
	planeShape.setColor(se::Color());
	//planeShape.setRenderMode(se::graphics::RenderMode::Static);
	//planeShape.setPrimitiveType(se::graphics::PrimitiveType::Lines);
	scene.add(planeShape);

	se::graphics::Shape testShape(se::graphics::ShapeType::Sphere/*, 8*/);
	testShape.setMaterial(phongMaterial);
	testShape.setScale(glm::vec3(3.0f));
	testShape.setPosition({ 10.0f, -7.0f, 0.0f });
	//testShape.setColor(se::Color(1.5f, 1.5f, 1.5f));
	//testShape.setRenderMode(se::graphics::RenderMode::Static);
	//testShape.setPrimitiveType(se::graphics::PrimitiveType::Lines);
	scene.add(testShape);

	se::graphics::Line originX;
	originX.addPoint(glm::vec3(0.0f), se::hexColor(se::HexColor::Red));
	originX.addPoint(glm::vec3(1.0f, 0.0f, 0.0f), se::hexColor(se::HexColor::Red));
	originX.setMaterial(colorMaterial);
	scene.add(originX);
	se::graphics::Line originZ;
	originZ.addPoint(glm::vec3(0.0f), se::hexColor(se::HexColor::Green));
	originZ.addPoint(glm::vec3(0.0f, 0.0f, 1.0f), se::hexColor(se::HexColor::Green));
	originZ.setMaterial(colorMaterial);
	scene.add(originZ);
	se::graphics::Line originY;
	originY.addPoint(glm::vec3(0.0f), se::hexColor(se::HexColor::Blue));
	originY.addPoint(glm::vec3(0.0f, 1.0f, 0.0f), se::hexColor(se::HexColor::Blue));
	originY.setMaterial(colorMaterial);
	scene.add(originY);

	//se::Console console;

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
			objects.push_back(std::make_unique<ShapeObject>(scene, phongMaterial, colorMaterial));
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
			pointLight.setPosition(glm::vec3(0.0f, -6.0f + (float)abs(sin(timeNowSeconds * 0.5)) * 6.0f, 0.0f));
			pointLight.setRadius(pointLight.getInnerRadius(), 15.0f + 10.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
			sunLight.setDirection(glm::vec3((float)sin(timeNowSeconds * 0.2) * 0.5f, -1.0f, (float)cos(timeNowSeconds * 0.2) * 0.5f));

			const glm::vec3 direction = glm::vec3((float)sin(timeNowSeconds * 0.4) * 0.5f, (float)cos(timeNowSeconds * 0.2) * 1.0f, (float)cos(timeNowSeconds * 0.3) * 0.5f);
			spotLight.setDirection(direction);
			const float cone = glm::radians(10.0f + 50.0f * fabsf((float)sin(timeNowSeconds * 0.3)));
			spotLight.setCone(cone, cone);
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

		shaderManager.update();
		textureManager.update();
		fontManager.update();

		eventCatcher.pollEvents();
		eventSignaler.signalEvents(eventCatcher);
		inputManager.update(eventCatcher);

		cameraController.update(deltaTimeSystem.deltaTime);

		for (auto&& object : objects)
			object->update(deltaTimeSystem.deltaTime);

		se::graphics::Renderer::debugTextPrintf(1, 1, "frame: %i", frameN++);
		{
			const se::time::Time frameTime = se::time::now() - frameTimer;
			se::graphics::Renderer::debugTextPrintf(1, 2, "frame time: %i", (int)frameTime.asMilliseconds());
			frameTimer = se::time::now();
		}
		renderer.render();

		if (inputManager.isKeyPressed((unsigned)se::input::Key::F5))
		{
			se::log::info("Reloading shaders...", se::log::TextColor::BLUE);
			//shaderManager.purgeUnusedShaders();
			shaderManager.reload();
		}

		if (inputManager.isKeyPressed((unsigned)se::input::Key::F4))
		{
			se::log::info("Reloading textures...", se::log::TextColor::BLUE);
			textureManager.reload();
		}

		if (inputManager.isQuitRequested())
			break;
	}

	return 0;
}