#pragma once

#include "VulkanContext.hpp"
#include "Core/Core.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"


#include "vulkan/vulkan.h"

namespace SkinBuilder
{
	struct VulkanRenderPassInfo
	{
		bool SwapchainTarget = false;
		VkImageView TargetImageView = VK_NULL_HANDLE;
		VkExtent2D TargetImageSize = { 0, 0 };
	};
	class VulkanRenderPass
	{
	private:
		VulkanRenderPassInfo m_Info;
		Shared<VulkanSwapchain> m_Swapchain;
	public:
		VulkanRenderPass(const Shared<VulkanSwapchain>& swapchain, const VulkanRenderPassInfo& info);
		~VulkanRenderPass();

		void Begin(VkCommandBuffer commandBuffer);
		void End(VkCommandBuffer commandBuffer);

		VkImageView GetImageView() const;
	};

}