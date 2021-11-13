#include "stdafx.h"
#include "GUITest.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/GUI/GUIText.h"

using namespace se::gui;


GUITest::GUITest(se::graphics::Window& _window, se::graphics::ShaderManager& _shaderManager, se::graphics::TextureManager& _textureManager, se::graphics::FontManager& _fontManager)
	: view(_shaderManager, _textureManager, _fontManager)
{
	_window.add(view.getView());

	using namespace se::gui::unit_literals;

	auto root = std::make_shared<GUIShape>();
	root->setSize({ 1.0_view, 1.0_view });
	root->setColor(se::Color(se::HexColor::Black).withAlpha(0.8f));
	view.add(*root);
	elements.push_back(root);

	testElement = std::make_shared<GUIShape>();
	testElement->setPosition({ 0.5_view, 0.5_view });
	testElement->setSize({ 400.0f, 200.0f });
	testElement->setColor(se::Color(se::HexColor::Bisque).withAlpha(0.9f));
	testElement->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
	root->addChild(*testElement);

	std::shared_ptr<GUIShape> tempShape;
	std::shared_ptr<GUIText> tempText;

	tempShape = std::make_shared<GUIShape>();
	tempShape->setTexture("test.png");
	tempShape->setSize({ 0.5_ph, 0.5_ph });
	tempShape->setVerticalAnchor(VerticalAlignment::Center);
	tempShape->setVerticalAlignment(VerticalAlignment::Center);
	testElement->addChild(*tempShape);
	elements.push_back(tempShape);

	tempText = std::make_shared<GUIText>();
	tempText->insert("TEST");
	tempText->setColor(se::Color(se::HexColor::Green));
	tempText->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Center);
	tempText->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Center);
	tempShape->addChild(*tempText);
	elements.push_back(tempText);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setSize({ 0.5_ph, 0.5_ph });
	tempShape->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Right);
	tempShape->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Right);
	tempShape->setColor(se::Color(se::HexColor::DarkSeaGreen));
	testElement->addChild(*tempShape);
	elements.push_back(tempShape);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setTexture("test.png");
	tempShape->setSize({ 0.9_parent, 0.9_parent });
	tempShape->setMargin(Margin(0.05_parent));
	elements.back()->addChild(*tempShape);
	elements.push_back(tempShape);
}
void GUITest::update([[maybe_unused]] se::time::Time _deltaTime)
{
	//testElement->setPosition(testElement->getPosition().value() + glm::vec2(5.0f) * _deltaTime.asSeconds());
	//testElement->setRotation(testElement->getRotation() + 0.5f * _deltaTime.asSeconds());
	//testElement->setScale(glm::vec2(1.0f + 0.4f * fabsf(sinf(se::time::now().asSeconds()))));
}
