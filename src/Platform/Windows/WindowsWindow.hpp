#pragma once

#include "Core/Window.hpp"

#include <GLFW/glfw3.h>

namespace SkinBuilder
{
	class WindowsWindow : public Window
	{

	public:
		WindowsWindow(const uint32_t width, const uint32_t height, const std::string title);
		~WindowsWindow() override;

		void Initialize() override;
		void PollEvents() override;
		bool ShouldClose() const override;
		uint32_t GetHeight() const override { return m_Width; }
		uint32_t GetWidth() const override { return m_Height; }
		void* GetNative() const override { return m_Window; }


	private:
		GLFWwindow* m_Window = nullptr;
		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Title;
	};
}