#include "SkinBuilder.hpp"


#include <Platform/Vulkan/VulkanContext.hpp>

#include "Platform/Vulkan/VulkanRenderer.hpp"

#include "ImGui/ImGui.hpp"
#include "spdlog/common.h"

namespace SkinBuilder
{
	static SkinBuilder* s_SkinBuilder = nullptr;

	constexpr static std::array s_SkinCategories = {
		"Skin Texture",
		"Hair",
		"Eyes",
		"Mouths",
		"Facial Hair",
		"Top Clothing",
		"Bottom Clothing",
		"Outerwear",
		"Hats",
		"Face Accessories",
		"Gloves",
		"Shoes"
	};

	SkinBuilder::SkinBuilder(const uint32_t width, const uint32_t height, const std::string& title)
	{
		Logger::Initialize();

		m_Window = Window::Create(width, height, title);
		m_Window->Initialize();

		s_SkinBuilder = this;

		m_Camera = Camera(45.0f, width / static_cast<float>(height), 0.01f, 100.0f);
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

		Shared<Mesh> mesh = Mesh::LoadWavefront("player.obj", context->GetDevice());

		while (m_Running)
		{
			m_Window->PollEvents();


			bool shouldRotateViewport = m_Window->IsMouseButtonPressed(MouseCode::Button0);
			bool shouldPanViewport = m_Window->IsMouseButtonPressed(MouseCode::Button1);
			m_Camera.OnUpdate(m_Window->GetMousePos(), shouldRotateViewport, shouldPanViewport);

			ImGuiHelper::Begin();

			ImGui::Begin("Character Builder");

			constexpr float_t thumbnailSize = 80.0f;
			constexpr float_t padding = 18.0f;

			const ImVec2 maxRegion = ImGui::GetContentRegionAvail();
			const uint32_t maxColumns = static_cast<uint32_t>(maxRegion.x / (thumbnailSize + padding));

			for (const auto& skinCategory : s_SkinCategories)
			{
				if (ImGui::CollapsingHeader(skinCategory))
				{
					ImGui::Columns(maxColumns, skinCategory, false);
					for (int i = 0; i < 5; i++)
					{
						ImGuiHelper::ScopedColor(ImGuiCol_Button, 0x00000000);
						ImGui::Button("Test", ImVec2(thumbnailSize, thumbnailSize));
						ImGui::NextColumn();
					}
					ImGui::Columns(1);
				}
			}

			ImGui::End();
			ImGui::ShowDemoWindow();

			ImGui::Render();

			renderer.Begin(m_Camera);

			renderer.Draw(0, 0);

			renderer.DrawMesh(mesh);

			ImGuiHelper::End(renderer.GetCommandBuffer());

			renderer.End();

			renderer.Submit();

			m_Running = !m_Window->ShouldClose();
		}
	}
}
