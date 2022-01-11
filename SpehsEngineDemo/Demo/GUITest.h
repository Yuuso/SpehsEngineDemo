#pragma once

#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Graphics/FontManager.h"
#include "SpehsEngine/Graphics/ShaderManager.h"
#include "SpehsEngine/Graphics/TextureManager.h"
#include "SpehsEngine/GUI/GUIView.h"
#include "SpehsEngine/GUI/GUIElement.h"
#include "SpehsEngine/GUI/GUIShape.h"
#include "SpehsEngine/Input/InputManager.h"
#include <vector>
#include <memory>


class GUITest
{
public:
	GUITest(se::graphics::Window& _window, se::graphics::ShaderManager& _shaderManager, se::graphics::TextureManager& _textureManager,
			se::graphics::FontManager& _fontManager, se::input::EventSignaler& _eventSignaler);
	void update(se::time::Time _deltaTime);

private:
	se::gui::GUIView view;
	se::gui::GUIShape root;
};
