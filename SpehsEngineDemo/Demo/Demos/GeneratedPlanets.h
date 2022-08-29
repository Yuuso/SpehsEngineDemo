#pragma once

#include "Demo/DemoContext.h"
#include "Demo/Demos/DemoApplication.h"
#include "Demo/Utility/CameraController.h"
#include "SpehsEngine/Core/Noise.h"
#include "SpehsEngine/Graphics/Model.h"
#include "SpehsEngine/Graphics/Lights.h"
#include "SpehsEngine/Graphics/Material.h"
#include <atomic>
#include <memory>
#include <thread>


class GeneratedPlanets final : public DemoApplication
{
public:
	GeneratedPlanets(DemoContext& _demoContext);
	~GeneratedPlanets();

	void init() override;
	bool update() override;

private:

	struct PlanetParams
	{
		int64_t seed = 2;
		se::NoiseType noiseType = se::NoiseType::OpenSimplex_4D_ImproveXYZ;

		double planetSize = 2.0;
		uint16_t cubeFaceSize = 256;

		float seaLevel = 0.5f; // 0: no water, 1: all water
		float elevationFactor = 1.0f; // 0: flat land, 1: potentially mountains (does not affect water depth)
		float temperatureRange = 0.2f; // -1: cold, 0: neutral, 1: hot
	};

	inline static constexpr float landBeachLevel = 0.008f;
	inline static constexpr float landMountainLevel = 0.35f;
	inline static constexpr float desertStartPoint = 0.7f;
	inline static constexpr float desertFullPoint = 0.85f;
	inline static constexpr float freezingPoint = 0.05f;
	inline static constexpr float snowPoint = freezingPoint * 2.0f;

	float getNoise(const glm::dvec3& _coord, double _w, size_t _octaves);
	float getElevation(const glm::dvec3& coord);
	float getTemperature(const glm::dvec3& coord);
	void generatePlanet();
	void generateMaterial();
	void updatePlanetGeneration();

	se::NoiseGenerator noise;
	PlanetParams planetParams;

	std::atomic_bool loadingMaterial = false;
	std::atomic_bool materialReady = false;
	std::atomic<float> loadingProgress = 1.0f;
	std::thread loadingThread;

	CameraController cameraController;
	se::graphics::DirectionalLight sunLight;
	se::graphics::AmbientLight ambientLight;
	se::graphics::Model planet;
	std::shared_ptr<se::graphics::Material> material;
};
