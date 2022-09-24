#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanRenderer.hpp"


namespace SkinBuilder::ImGuiHelper
{
	struct ScopedStyle
	{
		ScopedStyle(ImGuiStyleVar style, const ImVec2& value)
		{
			ImGui::PushStyleVar(style, value);
		}

		ScopedStyle(ImGuiStyleVar style, float_t value)
		{
			ImGui::PushStyleVar(style, value);
		}

		~ScopedStyle()
		{
			ImGui::PopStyleVar();
		}

		ScopedStyle(const ScopedStyle& other) = delete;
		ScopedStyle(const ScopedStyle&& other) = delete;
	};

	struct ScopedColor
	{
		ScopedColor(ImGuiCol style, const uint32_t value)
		{
			ImGui::PushStyleColor(style, value);
		}


		~ScopedColor()
		{
			ImGui::PopStyleColor();
		}

		ScopedColor(const ScopedColor& other) = delete;
		ScopedColor(const ScopedColor&& other) = delete;
	};


	void Initialize(const Shared<VulkanContext>& context, const VulkanRenderer& renderer, const Window* window);
	void Begin();
	void End(const VkCommandBuffer buffer);
	void Shutdown();
}
