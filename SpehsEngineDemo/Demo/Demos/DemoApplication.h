#pragma once

#include "Demo/DemoContext.h"


class DemoApplication
{
public:
	DemoApplication(DemoContext& _context)
		: demoContext(_context) {}
	virtual ~DemoApplication() = default;

	virtual void init() = 0;
	virtual bool update() = 0;

protected:
	DemoContext& demoContext;
};
