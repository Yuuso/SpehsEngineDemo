#include "stdafx.h"
#include "Demo/Demos/GeneratedPlanets.h"

#include "SpehsEngine/Core/ScopeTimer.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"
#include "SpehsEngine/input/MouseUtilityFunctions.h"
#include "SpehsEngine/Physics/3D/Collision3D.h"
#include "SpehsEngine/Physics/3D/Ray3D.h"
#include "SpehsEngine/Physics/3D/SphereCollider3D.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"

using namespace se::graphics;


GeneratedPlanets::GeneratedPlanets(DemoContext& _demoContext)
	: DemoApplication(_demoContext)
	, cameraController(_demoContext.mainWindow, _demoContext.camera, _demoContext.eventSignaler)
{}
GeneratedPlanets::~GeneratedPlanets()
{
	if (loadingMaterial)
	{
		while (!materialReady)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(32));
		}
		loadingThread.join();
	}
}
void GeneratedPlanets::init()
{
	generatePlanet();

	////////
	// Planet model

	auto planetModel = demoContext.modelDataManager.create("planet", "icosphere_5.gltf");
	planet.loadModelData(planetModel);
	planet.disableRenderFlags(RenderFlag::CullBackFace); // WTF is going on here?!

	demoContext.scene.add(planet);

	////////
	// Lights

	ambientLight.setColor(se::Color(se::White));
	ambientLight.setIntensity(0.05f);
	demoContext.scene.add(ambientLight);

	sunLight.setColor(se::Color(se::White));
	sunLight.setIntensity(1.1f);
	sunLight.setDirection(glm::vec3(-0.9f, -0.25f, 0.5f));
	demoContext.scene.add(sunLight);

	////////
	// Window

	demoContext.showWindowDefault();
}
float GeneratedPlanets::getNoise(const glm::dvec3& _coord, double _w, size_t _octaves)
{
	constexpr double noiseOctaveMultiplier = 1.75f;
	constexpr float noiseOctaveAmplitudeMultiplier = 0.55f;
	float max = 0.0f;
	float result = 0.0f;
	for (size_t i = 0; i < _octaves; i++)
	{
		const double multiplier = 1.0 + std::pow(i, noiseOctaveMultiplier);
		float sample = noise.generate(_coord.x * multiplier, _coord.y * multiplier, _coord.z * multiplier, _w, planetParams.noiseType);
		sample += 1.0f;
		sample *= 0.5f;
		const float amplitude = std::powf(noiseOctaveAmplitudeMultiplier, static_cast<float>(i));
		max += amplitude;
		result += sample * amplitude;
	}
	result /= max;
	se_assert(result >= 0.0f && result <= 1.0f);
	return result;
}
float GeneratedPlanets::getElevation(const glm::dvec3& coord)
{
	float result = getNoise(coord, 0.0, 8);
	if (planetParams.elevationFactor != 1.0f)
	{
		result -= planetParams.seaLevel;
		result *= planetParams.elevationFactor;
		result += planetParams.seaLevel;
	}
	return result;
}
float GeneratedPlanets::getTemperature(const glm::dvec3& coord)
{
	if (planetParams.temperatureRange == 1.0f)
	{
		return 1.0f;
	}
	else if (planetParams.temperatureRange == -1.0f)
	{
		return 0.0f;
	}

	float result = getNoise(coord, 1.0, 5);
	const float poleFactor = static_cast<float>(abs(glm::dot(coord / planetParams.planetSize, glm::dvec3(0.0, 1.0, 0.0))));
	result -= std::powf(poleFactor, 6.0f); // Cold on the poles
	result += std::powf(1.0f - poleFactor, 6.0f); // Hot on the equator
	if (planetParams.temperatureRange > 0.0f)
	{
		result = glm::mix(result, 1.0f, planetParams.temperatureRange);
	}
	else if (planetParams.temperatureRange < 0.0f)
	{
		result = glm::mix(result, 0.0f, fabsf(planetParams.temperatureRange));
	}
	return glm::clamp(result, 0.0f, 1.0f);
}
void GeneratedPlanets::generatePlanet()
{
	se_assert(!loadingMaterial);
	se_assert(loadingProgress == 1.0f);

	planet.setScale(glm::vec3(10.0f) * static_cast<float>(planetParams.planetSize));
	planet.setMaterial(nullptr);

	material = nullptr;
	loadingMaterial = true;
	materialReady = false;
	loadingProgress = 0.0f;
	loadingThread = std::thread([this] { generateMaterial(); });
}
void GeneratedPlanets::generateMaterial()
{
	se::time::ScopeTimer timer;

	const uint16_t colorCubeFaceSize = planetParams.cubeFaceSize;
	const uint16_t normalCubeFaceSize = 2;
	const uint16_t roughnessCubeFaceSize = colorCubeFaceSize;

	int numElevationDataGens = 0;
	const int estimateElevationDataGens = colorCubeFaceSize * colorCubeFaceSize * 6 + roughnessCubeFaceSize * roughnessCubeFaceSize * 6;

	noise.setSeed(planetParams.seed);

	TextureInput noiseColorInput;
	noiseColorInput.width = noiseColorInput.height = colorCubeFaceSize;
	noiseColorInput.isCubemap = true;
	noiseColorInput.data.resize(4 * colorCubeFaceSize * colorCubeFaceSize * 6);

	TextureInput noiseNormalInput;
	noiseNormalInput.width = noiseNormalInput.height = normalCubeFaceSize;
	noiseNormalInput.isCubemap = true;
	noiseNormalInput.data.resize(4 * normalCubeFaceSize * normalCubeFaceSize * 6);

	TextureInput noiseRoughnessInput;
	noiseRoughnessInput.format = TextureInput::Format::R8;
	noiseRoughnessInput.width = noiseRoughnessInput.height = roughnessCubeFaceSize;
	noiseRoughnessInput.isCubemap = true;
	noiseRoughnessInput.data.resize(1 * roughnessCubeFaceSize * roughnessCubeFaceSize * 6);

	/// How the cube map data is structured (bgfx docs have faces 2 & 3 swapped?):
	///
	///                  +----------+
	///                  |-z       3|
	///                  | ^  +y    |
	///                  | |        |    Unfolded cube:
	///                  | +---->+x |
	///       +----------+----------+----------+----------+
	///       |+y       1|+y       4|+y       0|+y       5|
	///       | ^  -x    | ^  +z    | ^  +x    | ^  -z    |
	///       | |        | |        | |        | |        |
	///       | +---->+z | +---->+x | +---->-z | +---->-x |
	///       +----------+----------+----------+----------+
	///                  |+z       2|
	///                  | ^  -y    |
	///                  | |        |
	///                  | +---->+x |
	///                  +----------+
	///

	auto getElevation2 =
		[&](const glm::dvec3& coord)
		{
			loadingProgress = static_cast<float>(++numElevationDataGens) / estimateElevationDataGens;
			return getElevation(coord);
		};

	auto getColor =
		[&](int _x, int _y, int _z)
		{
			const glm::dvec3 coord = glm::normalize(glm::dvec3(_x, _y, _z) - glm::dvec3(colorCubeFaceSize / 2.0)) * planetParams.planetSize;

			#define HEX(__hex) se::Color(0x##__hex##ff)

			const float elevation = getElevation2(coord);

			if (elevation < planetParams.seaLevel)
			{
				// Water
				constexpr se::Color deepSeaColor = HEX(081b3b);
				constexpr se::Color shoreColor = HEX(1f3e4f);
				const float waterLevel = std::powf(elevation / planetParams.seaLevel, 9.0f);
				return se::mixColor(deepSeaColor, shoreColor, waterLevel);
			}

			const float temperature = getTemperature(coord);
			if (false)
			{
				// Debug
				return se::mixColor(se::Color(0.0f, 0.0f, 1.0f), se::Color(1.0f, 0.0f, 0.0f), temperature);
			}

			const float landLevel = (elevation - planetParams.seaLevel) / (1.0f - planetParams.seaLevel);

			constexpr se::Color landSnowColor = HEX(e9eff0);
			constexpr se::Color mountainSnowColor = HEX(3d4242);
			const se::Color snowColor = se::mixColor(landSnowColor, mountainSnowColor, landLevel);
			if (temperature < freezingPoint)
			{
				// Snow
				return snowColor;
			}

			constexpr se::Color beachColor = HEX(bdb18a);
			constexpr se::Color highDesertColor = HEX(1f170b);
			const se::Color desertColor = se::mixColor(beachColor, highDesertColor, std::pow(landLevel, 2.0f));
			if (temperature > desertFullPoint)
			{
				// Desert
				return desertColor;
			}


			float fadeLevel = 0.0f;
			se::Color fadeColor;
			if (temperature < snowPoint)
			{
				// Fade snow
				fadeColor = snowColor;
				fadeLevel = (snowPoint - temperature) / (snowPoint - freezingPoint);
			}
			else if (temperature > desertStartPoint)
			{
				// Fade desert
				fadeColor = desertColor;
				fadeLevel = 1.0f - ((desertFullPoint - temperature) / (desertFullPoint - desertStartPoint));
			}

			#define ReturnFade(__color) if (fadeLevel > 0.0f) { return se::mixColor(__color, fadeColor, fadeLevel); } return __color;

			if (landLevel < (landBeachLevel * temperature))
			{
				// Beach
				ReturnFade(desertColor);
			}
			constexpr se::Color mountainBaseColor = HEX(182907);
			if (landLevel < landMountainLevel)
			{
				// Forest
				constexpr se::Color deepForestColor = mountainBaseColor;
				constexpr se::Color mainForestColor = HEX(304f1d);
				const float forestLevel = (landLevel - landBeachLevel) / (landMountainLevel - landBeachLevel);
				ReturnFade(se::mixColor(mainForestColor, deepForestColor, forestLevel));
			}
			{
				// Mountains
				constexpr se::Color mountainTopColor = HEX(0a0501);
				const float mountainLevel = (landLevel - landMountainLevel) / (1.0f - landMountainLevel);
				ReturnFade(se::mixColor(mountainBaseColor, mountainTopColor, std::pow(mountainLevel, 2.0f)));
			}
		};
	auto getNormal =
		[&](int, int, int)
		{
			return se::Color(0.5f, 0.5f, 1.0f, 1.0f);
		};
	auto getRoughness =
		[&](int _x, int _y, int _z)
		{
			const glm::dvec3 coord = glm::normalize(glm::dvec3(_x, _y, _z) - glm::dvec3(roughnessCubeFaceSize / 2.0)) * planetParams.planetSize;
			const float elevation = getElevation2(coord);
			if (elevation < planetParams.seaLevel)
			{
				// Shiny water
				return 0.0f;
			}
			// Rough land
			return 1.0f;
		};

	size_t colorDataIndex = 0;
	auto pushColorData =
		[&](se::Color _color)
		{
			noiseColorInput.data[colorDataIndex++] = static_cast<uint8_t>(_color.r * UINT8_MAX);
			noiseColorInput.data[colorDataIndex++] = static_cast<uint8_t>(_color.g * UINT8_MAX);
			noiseColorInput.data[colorDataIndex++] = static_cast<uint8_t>(_color.b * UINT8_MAX);
			noiseColorInput.data[colorDataIndex++] = UINT8_MAX;
		};
	size_t normalDataIndex = 0;
	auto pushNormalData =
		[&](se::Color _color)
		{
			noiseNormalInput.data[normalDataIndex++] = static_cast<uint8_t>(_color.r * UINT8_MAX);
			noiseNormalInput.data[normalDataIndex++] = static_cast<uint8_t>(_color.g * UINT8_MAX);
			noiseNormalInput.data[normalDataIndex++] = static_cast<uint8_t>(_color.b * UINT8_MAX);
			noiseNormalInput.data[normalDataIndex++] = UINT8_MAX;
		};
	size_t roughnessDataIndex = 0;
	auto pushRoughnessData =
		[&](float roughness)
		{
			noiseRoughnessInput.data[roughnessDataIndex++] = static_cast<uint8_t>(roughness * UINT8_MAX);
		};

	auto cubemapGenerator =
		[](auto func, int faceSize)
		{
			// X+
			for (int y = 0; y < faceSize; y++)
			{
				for (int z = 0; z < faceSize; z++)
				{
					func(faceSize, y, faceSize - z);
				}
			}
			// X-
			for (int y = 0; y < faceSize; y++)
			{
				for (int z = 0; z < faceSize; z++)
				{
					func(0, y, z);
				}
			}
			// Y-
			for (int z = 0; z < faceSize; z++)
			{
				for (int x = 0; x < faceSize; x++)
				{
					func(x, 0, z);
				}
			}
			// Y+
			for (int z = 0; z < faceSize; z++)
			{
				for (int x = 0; x < faceSize; x++)
				{
					func(x, faceSize, faceSize - z);
				}
			}
			// Z+
			for (int y = 0; y < faceSize; y++)
			{
				for (int x = 0; x < faceSize; x++)
				{
					func(x, y, faceSize);
				}
			}
			// Z-
			for (int y = 0; y < faceSize; y++)
			{
				for (int x = 0; x < faceSize; x++)
				{
					func(faceSize - x, y, 0);
				}
			}
		};

	// NOTE: DemoContext technically not thread safe, but we're not doing anything else so it's fine...

	cubemapGenerator([&](int _x, int _y, int _z){ pushColorData(getColor(_x, _y, _z)); }, colorCubeFaceSize);
	se_assert(colorDataIndex == noiseColorInput.data.size());
	auto planetColorTexture = demoContext.textureManager.create("PlanetColorTexture", noiseColorInput);
	demoContext.textureManager.remove("PlanetColorTexture"); // No need for manager

	cubemapGenerator([&](int _x, int _y, int _z){ pushNormalData(getNormal(_x, _y, _z)); }, normalCubeFaceSize);
	se_assert(normalDataIndex == noiseNormalInput.data.size());
	auto planetNormalTexture = demoContext.textureManager.create("PlanetNormalTexture", noiseNormalInput);
	demoContext.textureManager.remove("PlanetNormalTexture"); // No need for manager

	cubemapGenerator([&](int _x, int _y, int _z){ pushRoughnessData(getRoughness(_x, _y, _z)); }, roughnessCubeFaceSize);
	se_assert(roughnessDataIndex == noiseRoughnessInput.data.size());
	auto planetRoughnessTexture = demoContext.textureManager.create("PlanetRoughnessTexture", noiseRoughnessInput);
	demoContext.textureManager.remove("PlanetRoughnessTexture"); // No need for manager

	se_assert(numElevationDataGens == estimateElevationDataGens);
	loadingProgress = 1.0f;

	planetColorTexture->waitUntilReady();
	planetNormalTexture->waitUntilReady();
	planetRoughnessTexture->waitUntilReady();

	material = createMaterial(DefaultMaterialType::PhongCubemap, demoContext.shaderManager);
	material->setTexture(planetColorTexture, PhongTextureType::Color);
	material->setTexture(planetNormalTexture, PhongTextureType::Normal);
	material->setTexture(planetRoughnessTexture, PhongTextureType::Roughness);
	auto phongAttributes = std::make_shared<PhongAttributes>();
	phongAttributes->specularStrength() = 0.8f;
	phongAttributes->shininess() = 8.0f;
	material->setUniformContainer(phongAttributes, "PhongAttributes");

	materialReady = true;

	se::log::info("Planet material generation time: " + se::toString(timer.get().asSeconds<float>(), 2) + "s");
}
void GeneratedPlanets::updatePlanetGeneration()
{
	if (!loadingMaterial || !materialReady)
	{
		return;
	}
	se_assert(loadingProgress == 1.0f);
	se_assert(material);

	loadingThread.join();
	loadingMaterial = false;

	planet.setMaterial(material);
}
bool GeneratedPlanets::update()
{
	if (loadingMaterial)
	{
		updatePlanetGeneration();

		ImGui::SetNextWindowPos({ demoContext.mainWindow.getWidth() / 2.0f, demoContext.mainWindow.getHeight() / 2.0f }, 0, { 0.5f, 0.5f });
		ImGui::SetNextWindowSize({ 400.0f, 0.0f });
		if (ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
		{
			const char* text = "Generating planet...";
			auto textWidth = ImGui::CalcTextSize(text).x;
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textWidth) * 0.5f);
			ImGui::Text(text);

			float barWidth = 350.0f;
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - barWidth) * 0.5f);
			ImGui::ProgressBar(loadingProgress, { barWidth, 40.0f });
		}
		ImGui::End();

		return true;
	}

	const se::physics::Ray3D mouseRay(cameraController.getFrustumPoint(glm::vec3(demoContext.inputManager.getMouseCoords(), 0.0f)),
									  cameraController.getFrustumPoint(glm::vec3(demoContext.inputManager.getMouseCoords(), 1.0f)));
	const se::physics::SphereCollider3D planetCollider(planet.getPosition(), planet.getScale().x * 0.5f);
	se::physics::Collision3D mouseHit(mouseRay, planetCollider);
	if (mouseHit.hit() && se::input::getShowCursor())
	{
		/*const */glm::dvec3 coord = glm::normalize(mouseHit.point() - planet.getPosition());
		coord.y = -coord.y; // For some reason y coordinate is inverted...
		const float elevation = getElevation(coord * planetParams.planetSize);
		if (elevation < planetParams.seaLevel)
		{
			ImGui::BeginTooltip();

			ImGui::Text("Water");

			ImGui::EndTooltip();
		}
		else
		{
			ImGui::BeginTooltip();

			//ImGui::Text("Coordinates: " + se::toString(glm::vec3(coord)));

			const float elevationKm = 15'000.0f * (elevation - planetParams.seaLevel); // idk some random values...
			ImGui::Text("Elevation: " + std::to_string(static_cast<int>(elevationKm)) + "m above sea level");

			const float temperature = getTemperature(coord * planetParams.planetSize);
			const float temperatureC = (temperature - snowPoint) * 50.0f;
			ImGui::Text("Temperature: " + std::to_string(static_cast<int>(temperatureC)) + " degrees celcius");

			ImGui::EndTooltip();
		}
	}

	ImGui::SetNextWindowPos({ 20.0f, demoContext.mainWindow.getHeight() - 20.0f }, 0, { 0.0f, 1.0f });
	ImGui::SetNextWindowSize({ 0.0f, 250.0f });
	if (ImGui::Begin("Planet Parameters", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		auto GetNoiseTypeString =
			[](se::NoiseType _type) -> std::string
			{
				switch (_type)
				{
					case se::NoiseType::OpenSimplex_4D_ImproveXYZ_ImproveXY:	return "ImproveXYZ_ImproveXY";
					case se::NoiseType::OpenSimplex_4D_ImproveXYZ_ImproveXZ:	return "ImproveXYZ_ImproveXZ";
					case se::NoiseType::OpenSimplex_4D_ImproveXYZ:				return "ImproveXYZ";
					case se::NoiseType::OpenSimplex_4D_Fallback:				return "Fallback";
					case se::NoiseType::OpenSimplex_4D_UnskewedBase:			return "UnskewedBase";
					default: return "Unsupported";
				}
			};

		ImGui::InputT("Seed", planetParams.seed);
		if (ImGui::Button("Noise type: " + GetNoiseTypeString(planetParams.noiseType)))
		{
			planetParams.noiseType = static_cast<se::NoiseType>(static_cast<int>(planetParams.noiseType) + 1);
			if (static_cast<int>(planetParams.noiseType) > static_cast<int>(se::NoiseType::OpenSimplex_4D_UnskewedBase))
			{
				planetParams.noiseType = se::NoiseType::OpenSimplex_4D_ImproveXYZ_ImproveXY;
			}
		}
		if (ImGui::InputT("Size", planetParams.planetSize))
		{
			planetParams.planetSize = std::clamp(planetParams.planetSize, 0.1, 10.0);
		}
		int texSize = static_cast<int>(planetParams.cubeFaceSize); // TODO: Fix InputT for uint16_t ?
		if (ImGui::InputT("Texture Size", texSize))
		{
			texSize = std::clamp(texSize, 16, 8192);
			planetParams.cubeFaceSize = static_cast<uint16_t>(texSize);
		}
		ImGui::Separator();
		if (ImGui::InputT("Sea Level", planetParams.seaLevel))
		{
			planetParams.seaLevel = std::clamp(planetParams.seaLevel, 0.0f, 1.0f);
		}
		if (ImGui::InputT("Elevation Variance", planetParams.elevationFactor))
		{
			planetParams.elevationFactor = std::clamp(planetParams.elevationFactor, 0.0f, 1.0f);
		}
		if (ImGui::InputT("Temperature Weight", planetParams.temperatureRange))
		{
			planetParams.temperatureRange = std::clamp(planetParams.temperatureRange, -1.0f, 1.0f);
		}
		ImGui::Separator();
		if (ImGui::Button("Generate"))
		{
			generatePlanet();
		}
	}
	ImGui::End();

	cameraController.update(demoContext.deltaTimeSystem.deltaTime);
	return true;
}
