#include "SkinBuilder.hpp"

namespace SkinBuilder
{
	SkinBuilder::SkinBuilder(const uint32_t width, const uint32_t height, const std::string& title)
	{
		m_Window = Window::Create(width, height, title);
		m_Window->Initialize();
	}

	void SkinBuilder::Run()
	{
		m_Running = true;

		while (m_Running)
		{
			m_Window->PollEvents();


		}
	}
}
