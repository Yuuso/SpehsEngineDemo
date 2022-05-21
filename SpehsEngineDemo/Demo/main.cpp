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
			while (true)
			{
				const bool engineExitRequested = !demoContext.update();
				const bool demoExitRequested = !demoApp->update();
				demoContext.render();
				if (engineExitRequested || demoExitRequested)
				{
					break;
				}
			}
		}
	}

	return 0;
}