#include "stdafx.h"

#include "Demo/DemoContext.h"
#include "Demo/DemoSelector.h"
#include "SpehsEngine/Audio/AudioLib.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"
#include "SpehsEngine/GUI/GUILib.h"
#include "SpehsEngine/Input/InputLib.h"


int main()
{
	se::CoreLib core;
	se::InputLib input;
	se::GraphicsLib graphics;
	se::GUILib gui;
	se::AudioLib audio;

	{
		DemoContext demoContext;

		while (auto demoApp = demoApplicationSelector(demoContext))
		{
			while (demoApp->update() && demoContext.update()) {}
		}
	}

	return 0;
}