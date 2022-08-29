#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace SkinBuilder
{
	class Camera
	{
	private:
		float_t m_Fov = 45.0f;
		float_t m_NearClip = 0.1f;
		float_t m_FarClip = 100.0f;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
		glm::vec2 m_Rotation = { 0.0f, 0.0f };
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };


		float_t m_Distance = 10.0f;

		constexpr static float_t s_RotationSpeed = 0.8f;
		constexpr static float_t s_ZoomSpeed = 0.5f;

		void UpdateProjectionMatrix();
		void UpdateViewMatrix();

		void Pan(const glm::vec2& delta);
		void Rotate(const glm::vec2& delta);
		void Zoom(float_t delta);

		glm::vec3 CalculatePosition() const;
	public:
		Camera() = default;
		Camera(float_t fov, float_t aspectRatio, float_t nearClip, float_t farClip);

		void OnUpdate(const glm::vec2& mousePos, bool shouldRotate, bool shouldPan);

		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		float_t GetDistance() const { return m_Distance; }
		void setDistance(float_t distance) { m_Distance = distance; }

		glm::vec3 GetUp() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetForward() const;

		const glm::vec3& GetPosition() const { return m_Position; }

		glm::quat GetOrientation() const;

		float_t GetPitch() const { return m_Rotation.x; }
		float_t GetYaw() const { return m_Rotation.y; }

	};
}