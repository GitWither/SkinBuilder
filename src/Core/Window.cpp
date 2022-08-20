#include <Core/Window.hpp>

#include <Platform/Windows/WindowsWindow.hpp>

namespace SkinBuilder
{

	Unique<Window> Window::Create(const uint32_t width, const uint32_t height, const std::string& title)
	{
		return MakeUnique<WindowsWindow>(width, height, title);
	}


}