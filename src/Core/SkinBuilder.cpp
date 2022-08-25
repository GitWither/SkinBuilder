#include "SkinBuilder.hpp"


#include <Platform/Vulkan/VulkanContext.hpp>

#include "Platform/Vulkan/VulkanRenderer.hpp"

#include "ImGui/ImGui.hpp"
#include "spdlog/common.h"

namespace SkinBuilder
{
	static SkinBuilder* s_SkinBuilder = nullptr;

	SkinBuilder::SkinBuilder(const uint32_t width, const uint32_t height, const std::string& title)
	{
		Logger::Initialize();

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

		ImGuiHelper::Initialize(context, renderer, m_Window.get());

		while (m_Running)
		{
			m_Window->PollEvents();

			ImGuiHelper::Begin();

			ImGui::ShowDemoWindow();

			ImGui::Render();

			renderer.Begin();
			renderer.Draw(0, 0);
			ImGuiHelper::End(renderer.GetCommandBuffer());
			renderer.End();

			renderer.Submit();

			m_Running = !m_Window->ShouldClose();
		}

		ImGuiHelper::Shutdown();
	}
}
