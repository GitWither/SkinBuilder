#include "Platform/Windows/WindowsWindow.hpp"

namespace SkinBuilder
{
	WindowsWindow::WindowsWindow(const uint32_t width, const uint32_t height, std::string title) : m_Width(width), m_Height(height), m_Title(std::move(title))
	{
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}


	void WindowsWindow::Initialize()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(static_cast<int>(m_Width), static_cast<int>(m_Height), m_Title.c_str(), nullptr, nullptr);
	}

	void WindowsWindow::PollEvents()
	{
		glfwPollEvents();
	}

	bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}
}
