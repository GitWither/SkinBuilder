#pragma once

#include "Core.hpp"
#include <glm/glm.hpp>

#include <string>

namespace SkinBuilder
{
	enum MouseCode
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void Initialize() = 0;
		virtual void PollEvents() = 0;
		virtual bool ShouldClose() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual glm::vec2 GetMousePos() const = 0;
		virtual bool IsMouseButtonPressed(const MouseCode code) const = 0;

		virtual void* GetNative() const = 0;

		static Unique<Window> Create(const uint32_t width, const uint32_t height, const std::string& title);
	};
}