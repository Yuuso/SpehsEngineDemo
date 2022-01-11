#include "stdafx.h"
#include "GUITest.h"

#include "SpehsEngine/Core/RNG.h"
#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/GUI/GUIStack.h"
#include "SpehsEngine/GUI/GUIText.h"

using namespace se::gui;
using namespace se::gui::unit_literals;


GUITest::GUITest(se::graphics::Window& _window,
				 se::graphics::ShaderManager& _shaderManager,
				 se::graphics::TextureManager& _textureManager,
				 se::graphics::FontManager& _fontManager,
				 se::input::EventSignaler& _eventSignaler)
	: view(_shaderManager, _textureManager, _fontManager, _eventSignaler, 465)
{
	_window.add(view.getView());

	root.setSize({ 1.0_view, 1.0_view });
	root.setColor(se::Color(se::HexColor::Black).withAlpha(0.8f));
	view.add(root);

	std::shared_ptr<GUIShape> tempShape;
	std::shared_ptr<GUIText> tempText;

	tempShape = std::make_shared<GUIShape>();
	tempShape->setPosition({ 0.5_view, 0.5_view });
	tempShape->setSize({ 400.0_px, 200.0_px });
	tempShape->setColor(se::Color(se::HexColor::Bisque).withAlpha(0.9f));
	tempShape->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
	tempShape->onClick([]{ se::log::info("click!"); });
	{
		GUIElementProperties hoverProps;
		hoverProps.color = se::Color(se::HexColor::Pink);
		tempShape->setHoverProperties(hoverProps);
		GUIElementProperties pressedProps;
		pressedProps.scale = glm::vec2(0.98f);
		tempShape->setPressedProperties(pressedProps);
	}
	root.addChild(tempShape);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setTexture("test.png");
	tempShape->setSize({ 0.5_ph, 0.5_ph });
	tempShape->setVerticalAnchor(VerticalAlignment::Center);
	tempShape->setVerticalAlignment(VerticalAlignment::Center);
	//tempShape->setClipping(true);
	{
		GUIElementProperties hoverProps;
		hoverProps.color = se::Color(se::HexColor::Yellow);
		tempShape->setHoverProperties(hoverProps);
		GUIElementProperties pressedProps;
		pressedProps.scale = glm::vec2(0.98f);
		tempShape->setPressedProperties(pressedProps);
	}
	root.getChild(0)->addChild(tempShape);

	tempText = std::make_shared<GUIText>();
	tempText->insert("TEST");
	tempText->setWidth(1_pw);
	tempText->setHeight(1_ph);
	tempText->setColor(se::Color(se::HexColor::Green));
	tempText->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
	tempText->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
	tempShape->addChild(tempText);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setSize({ 0.5_ph, 0.5_ph });
	tempShape->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Right);
	tempShape->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Right);
	tempShape->setColor(se::Color(se::HexColor::DarkSeaGreen));
	auto parentIndex = root.getChild(0)->addChild(tempShape);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setTexture("test.png");
	tempShape->setSize({ 0.9_parent, 0.9_parent });
	tempShape->setPosition({ 0.05_parent, 0.05_parent });
	root.getChild(0)->getChild(parentIndex)->addChild(tempShape);

	auto dup = root.getChild(0)->clone();
	dup->setPosition({ 0_px, 0_px });
	dup->setAnchor(VerticalAlignment::Top, HorizontalAlignment::Left);
	root.addChild(dup);


	auto vstack = std::make_shared<GUIStack>();
	vstack->setWidth(0.2_vw);
	vstack->setPadding(2_px);
	vstack->setAnchor(VerticalAnchor::Center, HorizontalAnchor::Right);
	vstack->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Right);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setSize(1_parent);
	tempShape->setPosition(0_px);
	tempShape->setColor(se::Color(se::HexColor::DimGray).withAlpha(0.8f));
	tempShape->setZIndex(-1);
	vstack->addChild(tempShape);

	auto hstack = std::make_shared<GUIStack>();
	hstack->setOrientation(StackOrientation::Horizontal);
	hstack->setHorizontalAnchor(HorizontalAnchor::Center);
	hstack->setHorizontalAlignment(HorizontalAlignment::Center);
	hstack->setPadding(2_px);
	hstack->setHeight(30_px);
	for (size_t i = 0; i < 10; i++)
	{
		auto stackChild = std::make_shared<GUIShape>();
		stackChild->setSize({ GUIUnit(se::rng::random(10.0f, 20.0f), GUIUnitType::Pixels), GUIUnit(se::rng::random(15.0f, 30.0f), GUIUnitType::Pixels) });
		hstack->addChild(stackChild);
	}
	vstack->addChild(hstack);

	for (size_t i = 0; i < 10; i++)
	{
		auto stackChild = std::make_shared<GUIShape>();
		stackChild->setHorizontalAnchor(HorizontalAnchor::Center);
		stackChild->setHorizontalAlignment(HorizontalAlignment::Center);
		stackChild->setSize({ GUIUnit(se::rng::random(0.5f, 1.0f), GUIUnitType::ParentWidth), GUIUnit(se::rng::random(15.0f, 30.0f), GUIUnitType::Pixels) });
		vstack->addChild(stackChild);
	}
	root.addChild(vstack);
}
void GUITest::update([[maybe_unused]] se::time::Time _deltaTime)
{
	root.getChild(1)->setPosition(root.getChild(1)->getPosition() + GUIVec2(0.5_px) * _deltaTime.asSeconds());
	root.getChild(1)->setRotation(root.getChild(1)->getRotation() + 0.5f * _deltaTime.asSeconds());
	root.getChild(1)->setScale(glm::vec2(1.0f + 0.4f * fabsf(sinf(se::time::now().asSeconds()))));
}
