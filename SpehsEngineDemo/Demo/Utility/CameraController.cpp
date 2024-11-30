#include "stdafx.h"
#include "CameraController.h"

#include "boost/bind.hpp"
#include "SpehsEngine/Input/MouseUtilityFunctions.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/common.hpp"
#include "glm/gtx/rotate_vector.hpp"


CameraController::CameraController(const se::gfx::Window& _window, se::gfx::Camera& _camera, se::input::EventSignaler& _eventSignaler)
	: camera(_camera)
	, initialState(_camera)
	, eventSignaler(_eventSignaler)
	, window(_window)
{
	constexpr int cameraControllerinputPriority = se::input::EventSignaler::minPriority;
	eventSignaler.connectToPreUpdateSignal(			preUpdateConnection,			[this]{ preUpdateCallback(); });
	eventSignaler.connectToPostUpdateSignal(		postUpdateConnection,			[this]{ postUpdateCallback(); });
	eventSignaler.connectToKeyboardSignal(			keyboardConnection,				[this](auto& event){ return keyboardCallback(event); },		cameraControllerinputPriority);
	eventSignaler.connectToMouseButtonSignal(		mouseButtonConnection,			[this](auto& event){ return mouseButtonCallback(event); },	cameraControllerinputPriority);
	eventSignaler.connectToMouseMotionSignal(		mouseMotionConnection,			[this](auto& event){ return mouseMotionCallback(event); },	cameraControllerinputPriority);
	eventSignaler.connectToMouseHoverSignal(		mouseHoverConnection,			[this](auto& event){ return mouseHoverCallback(event); },	cameraControllerinputPriority);
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
	const glm::vec3 direction = glm::rotate(glm::rotate(camera.getDirection	(), rotation.y * ROTATION_SPEED * _deltaTime.asSeconds(), camera.getLeft()),
																				rotation.x * ROTATION_SPEED * _deltaTime.asSeconds(), -camera.getUp());
	const glm::vec3 up = glm::rotate(glm::normalize(glm::cross(direction, camera.getLeft())), tilt * TILT_SPEED * _deltaTime.asSeconds(), direction);

	camera.setDirection(direction);
	camera.setPosition(position);
	camera.setUp(up);

	if (mouseMovementActive)
	{
		se::input::setMousePosition({ window.getWidth() / 2, window.getHeight() / 2 });
	}
}
glm::vec3 CameraController::getFrustumPoint(const glm::vec3& _screenCoordinates) const
{
#pragma warning(push)
#pragma warning(disable : 4127)
	const glm::vec3 screenCoordinatesFlipped(_screenCoordinates.x, float(window.getHeight()) - _screenCoordinates.y, _screenCoordinates.z);
	return glm::unProject(screenCoordinatesFlipped, camera.getViewMatrix(), camera.getProjectionMatrix(window.getWidth(), window.getHeight()), glm::ivec4(0, 0, window.getWidth(), window.getHeight()));
#pragma warning(pop)
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
bool CameraController::keyboardCallback(const se::input::KeyboardEvent& _event)
{
	if (!window.hasKeyboardFocus())
		return false;
	if (_event.type == se::input::KeyboardEvent::Type::Press)
	{
		if (_event.key == se::input::Key::BACKSPACE)
		{
			camera = initialState;
			return true;
		}
	}
	else if (_event.type == se::input::KeyboardEvent::Type::Hold)
	{
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
	}
	else if (_event.type == se::input::KeyboardEvent::Type::Release)
	{
	}
	return false;
}
bool CameraController::mouseButtonCallback(const se::input::MouseButtonEvent& _event)
{
	if (_event.type == se::input::MouseButtonEvent::Type::Press)
	{
		if (!window.hasMouseFocus())
			return false;
		switch (_event.button)
		{
			case se::input::MouseButton::Right:
				savedMousePos = se::input::getMousePosition().value();
				mouseMovementActive = true;
				se::input::setShowCursor(false);
				return true;
		}
	}
	else if (_event.type == se::input::MouseButtonEvent::Type::Release)
	{
		switch (_event.button)
		{
			case se::input::MouseButton::Right:
				se::input::setMousePosition(savedMousePos);
				mouseMovementActive = false;
				se::input::setShowCursor(true);
				return true;
		}
	}
	return false;
}
bool CameraController::mouseMotionCallback(const se::input::MouseMotionEvent& _event)
{
	if (!window.hasMouseFocus())
		return false;
	if (mouseMovementActive && receivingHover)
	{
		rotation += _event.position - glm::vec2(window.getWidth() / 2.0f, window.getHeight() / 2.0f);
		return true;
	}
	return false;
}
bool CameraController::mouseHoverCallback(const se::input::MouseHoverEvent&)
{
	if (!window.hasMouseFocus())
		return false;
	if (mouseMovementActive)
	{
		receivingHover = true;
		return true;
	}
	return false;
}