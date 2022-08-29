#include "Graphics/Camera.hpp"

namespace SkinBuilder
{
	Camera::Camera(float_t fov, float_t aspectRatio, float_t nearClip, float_t farClip) : m_Fov(fov), m_NearClip(nearClip), m_FarClip(farClip)
	{
		UpdateProjectionMatrix();
	}



	glm::vec3 Camera::CalculatePosition() const
	{
		return m_FocalPoint - GetForward() * m_Distance;
	}


	void Camera::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_ViewportSize.x / m_ViewportSize.y, m_NearClip, m_FarClip);
		m_ProjectionMatrix[1][1] *= -1;
	}


	void Camera::UpdateViewMatrix()
	{
		m_Position = CalculatePosition();

		const glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void Camera::Pan(const glm::vec2& delta)
	{
		const float_t x = std::min(m_ViewportSize.x / 1000.0f, 2.4f); // max = 2.4f
		const float_t xSpeed = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float_t y = std::min(m_ViewportSize.y / 1000.0f, 2.4f); // max = 2.4f
		const float_t ySpeed = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		m_FocalPoint += -GetRight() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUp() * delta.y * ySpeed * m_Distance;
	}

	void Camera::Rotate(const glm::vec2& delta)
	{
		const float_t yawSign = GetUp().y < 0 ? -1.0f : 1.0f;
		m_Rotation += glm::vec2{ delta.y * s_RotationSpeed, yawSign * delta.x * s_RotationSpeed };
	}

	void Camera::Zoom(float_t delta)
	{
		const float_t distance = std::max(m_Distance * 0.2f, 0.0f);
		const float_t speed = std::min(distance * distance, 100.0f);

		m_Distance -= delta * speed;
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForward();
			m_Distance = 1.0f;
		}
	}

	void Camera::OnUpdate(const glm::vec2& mousePos, bool shouldRotate, bool shouldPan)
	{
		glm::vec2 mousePosDelta = (mousePos - m_LastMousePosition) * 0.003f;
		m_LastMousePosition = mousePos;

		if (shouldRotate)
		{
			Rotate(mousePosDelta);
		}
		if (shouldPan)
		{
			Pan(mousePosDelta);
		}


		UpdateViewMatrix();
	}

	glm::vec3 Camera::GetUp() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRight() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetForward() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Rotation.x, -m_Rotation.y, 0.0f));
	}





}