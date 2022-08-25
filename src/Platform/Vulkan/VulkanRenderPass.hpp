#pragma once

#include "Core/Core.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"


#include "vulkan/vulkan.h"

namespace SkinBuilder
{
	class VulkanRenderPass
	{
	private:
		Shared<VulkanSwapchain> m_Swapchain;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	public:
		VulkanRenderPass(Shared<VulkanSwapchain> swapchain);
		~VulkanRenderPass();
	};

}