#include "SkinBuilder.hpp"


#include <Platform/Vulkan/VulkanContext.hpp>

#include "Platform/Vulkan/VulkanRenderer.hpp"

namespace SkinBuilder
{
	static SkinBuilder* s_SkinBuilder = nullptr;

	SkinBuilder::SkinBuilder(const uint32_t width, const uint32_t height, const std::string& title)
	{
		m_Window = Window::Create(width, height, title);
		m_Window->Initialize();

		s_SkinBuilder = this;
	}

	Window* SkinBuilder::GetWindow() const
	{
		return m_Window.get();
	}

	SkinBuilder& SkinBuilder::GetInstance()
	{
		return *s_SkinBuilder;
	}

	void SkinBuilder::Run()
	{
		m_Running = true;

		Shared<VulkanContext> context = MakeShared<VulkanContext>(m_Window.get());
		VulkanRenderer renderer(context);

		while (m_Running)
		{
			m_Window->PollEvents();



			renderer.Begin();
			renderer.Draw(0, 0);
			renderer.End();

			renderer.Submit();

			m_Running = !m_Window->ShouldClose();
		}
	}
}
