#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanRenderer.hpp"


namespace SkinBuilder::ImGuiHelper
{

	void Initialize(const Shared<VulkanContext>& context, const VulkanRenderer& renderer, const Window* window);
	void Begin();
	void End(const VkCommandBuffer buffer);
	void Shutdown();
}
