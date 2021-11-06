#include "stdafx.h"
#include "GUITest.h"

#include "SpehsEngine/Graphics/DefaultMaterials.h"
#include "SpehsEngine/GUI/GUIShape.h"

using namespace se::gui;


GUITest::GUITest(se::graphics::Window& _window, se::graphics::ShaderManager& _shaderManager, se::graphics::TextureManager& _textureManager)
{
	_window.add(view.getView());

	auto colorMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::FlatColor, _shaderManager);

	std::shared_ptr<se::graphics::Material> texMaterial = se::graphics::createMaterial(se::graphics::DefaultMaterialType::FlatTexture, _shaderManager);
	texMaterial->setTexture(_textureManager.find("balldemon"));

	auto root = std::make_shared<GUIElement>();
	view.add(*root);
	elements.push_back(root);

	testElement = std::make_shared<GUIShape>();
	testElement->setMaterial(colorMaterial);
	testElement->setPosition({ 5.0f, 5.0f });
	testElement->setSize({ 400.0f, 200.0f });
	testElement->setColor(se::Color(se::HexColor::Bisque).withAlpha(0.5f));
	root->addChild(*testElement);

	std::shared_ptr<GUIShape> tempShape;

	tempShape = std::make_shared<GUIShape>();
	tempShape->setMaterial(texMaterial);
	tempShape->setPosition({ 0.0f, 0.0f });
	tempShape->setSize({ 100.0f, 100.0f });
	tempShape->setVerticalAnchor(VerticalAlignment::Center);
	tempShape->setVerticalAlignment(VerticalAlignment::Center);
	testElement->addChild(*tempShape);
	elements.push_back(tempShape);

	tempShape = std::make_shared<GUIShape>();
	tempShape->setMaterial(texMaterial);
	tempShape->setPosition({ 0.0f, 0.0f });
	tempShape->setSize({ 100.0f, 100.0f });
	tempShape->setAnchor(VerticalAlignment::Center, HorizontalAlignment::Right);
	tempShape->setAlignment(VerticalAlignment::Center, HorizontalAlignment::Right);
	testElement->addChild(*tempShape);
	elements.push_back(tempShape);
}
void GUITest::update([[maybe_unused]] se::time::Time _deltaTime)
{
	testElement->setPosition(testElement->getPosition().value() + glm::vec2(5.0f) * _deltaTime.asSeconds());
	testElement->setRotation(testElement->getRotation() + 0.5f * _deltaTime.asSeconds());
}
