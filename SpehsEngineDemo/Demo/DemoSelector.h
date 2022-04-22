#pragma once

#include "Demo/DemoContext.h"
#include "Demo/Demos/DemoApplication.h"
#include <memory>


std::unique_ptr<DemoApplication> demoApplicationSelector(DemoContext& _demoContext);
