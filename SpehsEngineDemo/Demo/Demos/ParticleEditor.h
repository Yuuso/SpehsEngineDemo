#pragma once

#include "Demo/Demos/DemoApplication.h"
#include "Demo/Utility/CameraController.h"
#include "Demo/Utility/ParticleSystem.h"


class ParticleEditor final : public DemoApplication
{
public:

	ParticleEditor(DemoContext& _context);

	void init() override;
	bool update() override;

private:

	CameraController cameraController;
	//se::xpr::ParticleSystem particleSystem;
	se::gfx::Shape worldPlane;
	se::time::Time frameTimer = se::time::Time::zero;
};
