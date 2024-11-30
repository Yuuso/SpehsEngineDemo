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
	CameraController(const se::gfx::Window& _window, se::gfx::Camera& _camera, se::input::EventSignaler& _eventSignaler);

	void update(const se::time::Time _deltaTime);
	glm::vec3 getFrustumPoint(const glm::vec3& _screenCoordinates) const;

private:
	void preUpdateCallback();
	void postUpdateCallback();
	bool mouseHoverCallback(const se::input::MouseHoverEvent& _event);
	bool keyboardCallback(const se::input::KeyboardEvent& _event);
	bool mouseButtonCallback(const se::input::MouseButtonEvent& _event);
	bool mouseMotionCallback(const se::input::MouseMotionEvent& _event);

	se::gfx::Camera& camera;
	const se::gfx::Camera initialState;
	se::input::EventSignaler& eventSignaler;
	const se::gfx::Window& window;

	glm::vec3 movement = glm::vec3(0.0f);
	glm::vec2 rotation = glm::vec2(0.0f);
	float tilt = 0.0f;
	bool mouseMovementActive = false;
	bool receivingHover = false;
	bool boosting = false;
	glm::ivec2 savedMousePos;

	se::ScopedConnection preUpdateConnection;
	se::ScopedConnection postUpdateConnection;
	se::ScopedConnection mouseHoverConnection;
	se::ScopedConnection keyboardConnection;
	se::ScopedConnection mouseButtonConnection;
	se::ScopedConnection mouseMotionConnection;
};
