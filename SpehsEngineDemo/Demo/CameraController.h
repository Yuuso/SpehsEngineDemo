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
	bool keyboardPressCallback(const se::input::KeyboardPressEvent& _event);
	bool keyboardDownCallback(const se::input::KeyboardDownEvent& _event);
	bool mouseButtonPressCallback(const se::input::MouseButtonPressEvent& _event);
	bool mouseButtonReleaseCallback(const se::input::MouseButtonReleaseEvent& _event);
	bool mouseMotionCallback(const se::input::MouseMotionEvent& _event);

	se::graphics::Camera& camera;
	const se::graphics::Camera initialState;
	se::input::EventSignaler& eventSignaler;
	const se::graphics::Window& window;

	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec2 rotation = glm::vec2(0.0f);
	float tilt = 0.0f;
	bool receivingHover = false;

	boost::signals2::scoped_connection preUpdateConnection;
	boost::signals2::scoped_connection postUpdateConnection;
	boost::signals2::scoped_connection mouseHoverConnection;
	boost::signals2::scoped_connection keyboardPressConnection;
	boost::signals2::scoped_connection keyboardDownConnection;
	boost::signals2::scoped_connection mouseButtonPressConnection;
	boost::signals2::scoped_connection mouseButtonReleaseConnection;
	boost::signals2::scoped_connection mouseMotionConnection;
};
