#pragma once

#include "Demo/DemoContext.h"
#include "Demo/Demos/DemoApplication.h"


class MusicManager final : public DemoApplication
{
public:
	MusicManager(DemoContext& _demoContext);

	void init() override;
	bool update() override;

private:
};
