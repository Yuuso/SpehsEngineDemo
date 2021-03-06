#include "stdafx.h"
#include "CameraController.h"

#include "boost/bind.hpp"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/common.hpp"
#include "glm/gtx/rotate_vector.hpp"


CameraController::CameraController(const se::graphics::Window& _window, se::graphics::Camera& _camera, se::input::EventSignaler& _eventSignaler)
	: camera(_camera)
	, initialState(_camera)
	, eventSignaler(_eventSignaler)
	, window(_window)
{
	eventSignaler.connectToPreUpdateSignal(			preUpdateConnection,			boost::bind(&CameraController::preUpdateCallback,			this));
	eventSignaler.connectToPostUpdateSignal(		postUpdateConnection,			boost::bind(&CameraController::postUpdateCallback,			this));
	eventSignaler.connectToKeyboardPressSignal(		keyboardPressConnection,		boost::bind(&CameraController::keyboardPressCallback,		this, boost::placeholders::_1), 0);
	eventSignaler.connectToKeyboardDownSignal(		keyboardDownConnection,			boost::bind(&CameraController::keyboardDownCallback,		this, boost::placeholders::_1), 0);
	eventSignaler.connectToMouseButtonPressSignal(	mouseButtonPressConnection,		boost::bind(&CameraController::mouseButtonPressCallback,	this, boost::placeholders::_1), 0);
	eventSignaler.connectToMouseMotionSignal(		mouseMotionConnection,			boost::bind(&CameraController::mouseMotionCallback,			this, boost::placeholders::_1), 0);
	eventSignaler.connectToMouseHoverSignal(		mouseHoverConnection,			boost::bind(&CameraController::mouseHoverCallback,			this, boost::placeholders::_1), 0);
}

void CameraController::update(const se::time::Time _deltaTime)
{
	static constexpr float MOVEMENT_SPEED = 12.0f;
	static constexpr float ROTATION_SPEED = 0.08f;
	static constexpr float TILT_SPEED = 0.4f;

	float moveSpeed = MOVEMENT_SPEED;
	if (boosting)
		moveSpeed *= 3;

	const glm::vec3 position = camera.getPosition() + movement * moveSpeed * _deltaTime.asSeconds();
	const glm::vec3 direction = glm::rotate(glm::rotate(camera.getDirection	(), rotation.y * ROTATION_SPEED * _deltaTime.asSeconds(), -camera.getLeft()),
																			   rotation.x * ROTATION_SPEED * _deltaTime.asSeconds(), -camera.getUp());
	const glm::vec3 up = glm::rotate(glm::normalize(glm::cross(direction, camera.getLeft())), tilt * TILT_SPEED * _deltaTime.asSeconds(), direction);

	camera.setDirection(direction);
	camera.setPosition(position);
	camera.setUp(up);

	if (mouseButtonReleaseConnection.connected())
	{
		se::input::setShowCursor(false);
		se::input::setMousePosition({ window.getWidth() / 2, window.getHeight() / 2 });
	}
	else
	{
		se::input::setShowCursor(true);
	}
}

void CameraController::preUpdateCallback()
{
	receivingHover = false;
	movement = glm::mix(movement, glm::vec3(0.0f), 0.2f);
	rotation = glm::mix(rotation, glm::vec2(0.0f), 0.2f);
	tilt = glm::mix(tilt, 0.0f, 0.2f);
	boosting = false;
}
void CameraController::postUpdateCallback()
{
	if (glm::length(movement) > 1.0f)
		movement = glm::normalize(movement);
}
bool CameraController::keyboardPressCallback(const se::input::KeyboardPressEvent& _event)
{
	if (!window.getKeyboardFocus())
		return false;
	if (_event.key == se::input::Key::BACKSPACE)
	{
		camera = initialState;
		return true;
	}
	return false;
}
bool CameraController::keyboardDownCallback(const se::input::KeyboardDownEvent& _event)
{
	if (!window.getKeyboardFocus())
		return false;
	switch (_event.key)
	{
		case se::input::Key::W:
			movement += camera.getDirection();
			return true;
		case se::input::Key::S:
			movement += -camera.getDirection();
			return true;
		case se::input::Key::A:
			movement += camera.getLeft();
			return true;
		case se::input::Key::D:
			movement += -camera.getLeft();
			return true;
		case se::input::Key::R:
			movement += camera.getUp();
			return true;
		case se::input::Key::F:
			movement += -camera.getUp();
			return true;
		case se::input::Key::Q:
			tilt -= 1.0f;
			return true;
		case se::input::Key::E:
			tilt += 1.0f;
			return true;
		case se::input::Key::LSHIFT:
			boosting = true;
			return true;
	}
	return false;
}
bool CameraController::mouseButtonPressCallback(const se::input::MouseButtonPressEvent& _event)
{
	if (!window.getMouseFocus())
		return false;
	switch (_event.button)
	{
		case se::input::MouseButton::right:
			savedMousePos = se::input::getMousePosition();
			eventSignaler.connectToMouseButtonReleaseSignal(mouseButtonReleaseConnection, boost::bind(&CameraController::mouseButtonReleaseCallback, this, boost::placeholders::_1), 0);
			return true;
	}
	return false;
}
bool CameraController::mouseButtonReleaseCallback(const se::input::MouseButtonReleaseEvent& _event)
{
	if (!window.getMouseFocus())
		return false;
	switch (_event.button)
	{
		case se::input::MouseButton::right:
			mouseButtonReleaseConnection.disconnect();
			se::input::setMousePosition(savedMousePos);
			return true;
	}
	return false;
}
bool CameraController::mouseMotionCallback(const se::input::MouseMotionEvent& _event)
{
	if (!window.getMouseFocus())
		return false;
	if (mouseButtonReleaseConnection.connected()
		&& receivingHover)
	{
		rotation += _event.position - glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f);
		return true;
	}
	return false;
}
bool CameraController::mouseHoverCallback(const se::input::MouseHoverEvent&)
{
	if (!window.getMouseFocus())
		return false;
	if (mouseButtonReleaseConnection.connected())
	{
		receivingHover = true;
		return true;
	}
	return false;
}