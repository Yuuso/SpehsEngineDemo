#pragma once

#include "boost/signals2.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Graphics/Camera.h"
#include "SpehsEngine/Graphics/Window.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Core/SE_Time.h"


class CameraController
{
public:
	CameraController(const se::graphics::Window& _window, se::graphics::Camera& _camera, se::input::EventSignaler& _eventSignaler);

	void update(const se::time::Time _deltaTime);

private:
	void preUpdateCallback();
	void postUpdateCallback();
	bool mouseHoverCallback(const se::input::MouseHoverEvent& _event);
	bool keyboardCallback(const se::input::KeyboardEvent& _event);
	bool mouseButtonCallback(const se::input::MouseButtonEvent& _event);
	bool mouseMotionCallback(const se::input::MouseMotionEvent& _event);

	se::graphics::Camera& camera;
	const se::graphics::Camera initialState;
	se::input::EventSignaler& eventSignaler;
	const se::graphics::Window& window;

	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec2 rotation = glm::vec2(0.0f);
	float tilt = 0.0f;
	bool mouseMovementActive = false;
	bool receivingHover = false;
	bool boosting = false;
	glm::ivec2 savedMousePos;

	boost::signals2::scoped_connection preUpdateConnection;
	boost::signals2::scoped_connection postUpdateConnection;
	boost::signals2::scoped_connection mouseHoverConnection;
	boost::signals2::scoped_connection keyboardConnection;
	boost::signals2::scoped_connection mouseButtonConnection;
	boost::signals2::scoped_connection mouseMotionConnection;
};
