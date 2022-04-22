#include "stdafx.h"
#include "Demo/DemoSelector.h"

#include "Demo/Demos/GraphicsPlayground.h"
#include "Demo/Demos/GUIPlayground.h"
#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/GUI/GUIStack.h"
#include "SpehsEngine/GUI/GUIText.h"

using namespace se::gui;
using namespace se::gui::unit_literals;


static std::unique_ptr<DemoApplication> runApplicationSelector(DemoContext& _demoContext)
{
	std::unique_ptr<DemoApplication> result;
	bool demoSelected = false;
	GUIView guiView(_demoContext.shaderManager, _demoContext.textureManager, _demoContext.fontManager, _demoContext.eventSignaler, 8999);
	_demoContext.mainWindow.add(guiView.getView());

	GUIShape root;
	root.setSize(1_view);
	root.setPadding(30_px);
	root.setColor(se::Color(se::LightGray));
	guiView.add(root);

	auto border = std::make_shared<GUIShape>();
	border->setSize(1_parent);
	border->setPadding(2_px);
	border->setColor(se::Color(se::DimGray));
	border->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
	border->setAnchor(VerticalAnchor::Center, HorizontalAnchor::Center);
	root.addChild(border);

	auto foreground = std::make_shared<GUIShape>();
	foreground->setSize(1_parent);
	foreground->setColor(se::Color(se::LightGray));
	foreground->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
	foreground->setAnchor(VerticalAnchor::Center, HorizontalAnchor::Center);
	border->addChild(foreground);

	auto appList = std::make_shared<GUIStack>();
	appList->setPosition(0.5_parent);
	appList->setWidth(0.7_parent);
	appList->setPadding(4_px);
	appList->setOrientation(StackOrientation::Vertical);
	appList->setAnchor(VerticalAnchor::Center, HorizontalAnchor::Center);
	foreground->addChild(appList);

	{
		GUIElement button;
		button.setHeight(44_px);
		button.setWidth(1_parent);
		{
			GUIElementProperties hoverProps;
			hoverProps.padding = 1_px;
			button.setHoverProperties(hoverProps);
		}

		GUIShape fill;
		fill.setSize(1_parent);
		fill.setColor(se::Color(se::LightSeaGreen));
		fill.setConsumeInput(false);
		fill.setInheritInputStatus(true);
		{
			GUIElementProperties hoverProps;
			hoverProps.color = se::Color(se::MediumSeaGreen);
			fill.setHoverProperties(hoverProps);
		}
		{
			GUIElementProperties pressedProps;
			pressedProps.color = se::Color(se::DarkGray);
			fill.setPressedProperties(pressedProps);
		}
		{
			GUIShape decoration;
			decoration.setSize(8_px);
			decoration.setColor(se::Color(se::SlateGray));
			decoration.setRotation(se::PI<float> / 4.0f);
			decoration.setAnchor(VerticalAnchor::Center, HorizontalAnchor::Center);
			decoration.setY(0.5_parent);

			decoration.setX(0.05_parent);
			fill.addChild(decoration.clone());

			decoration.setX(0.95_parent);
			fill.addChild(decoration.clone());
		}
		button.addChild(fill.clone());

		GUIText text;
		text.setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
		text.setAnchor(VerticalAnchor::Center, HorizontalAnchor::Center);
		text.setHeight(24_px);
		text.setConsumeInput(false);
		text.setInheritInputStatus(true);
		{
			// TODO: Fix
			//GUIElementProperties hoverProps;
			//hoverProps.size = text.getSize();
			//hoverProps.size->y = 26_px;
			//text.setHoverProperties(hoverProps);
		}

		//

	#define SelectDemoType(__type) \
		[&](auto&){ result = std::make_unique<__type>(_demoContext); demoSelected = true; }

		auto graphicsAppButton = button.clone();
		text.clear();
		text.insert("Graphics Playground");
		graphicsAppButton->addChild(text.clone());
		graphicsAppButton->onClick(SelectDemoType(GraphicsPlayground));
		appList->addChild(graphicsAppButton);

		auto GUIAppButton = button.clone();
		text.clear();
		text.insert("GUI Playground");
		GUIAppButton->addChild(text.clone());
		GUIAppButton->onClick(SelectDemoType(GUIPlayground));
		appList->addChild(GUIAppButton);

		GUIElement gap;
		gap.setHeight(10_px);
		appList->addChild(gap.clone());

		auto quitButton = button.clone();
		text.clear();
		text.insert("Quit");
		quitButton->addChild(text.clone());
		quitButton->onClick([&](auto&){ demoSelected = true; });
		appList->addChild(quitButton);
	}

	_demoContext.mainWindow.setBorderless(true);
	_demoContext.mainWindow.setWidth(800);
	_demoContext.mainWindow.setHeight(400);
	_demoContext.mainWindow.setCenteredX();
	_demoContext.mainWindow.setCenteredY();
	_demoContext.mainWindow.show();

	while (_demoContext.update() && !demoSelected) {}
	guiView.remove(root);
	return result;
}

std::unique_ptr<DemoApplication> demoApplicationSelector(DemoContext& _demoContext)
{
	_demoContext.reset();

	auto result = runApplicationSelector(_demoContext);

	_demoContext.reset();
	_demoContext.update();

	if (result)
	{
		result->init();
	}

	return result;
}
