#pragma once

#include "Platform/Vulkan/VulkanDevice.hpp"

#include "vulkan/vulkan.h"

namespace SkinBuilder
{
	struct VulkanFramebufferInfo
	{
		uint32_t Width;
		uint32_t Height;

		std::vector<VkImageView> ImageViews;
		VkFormat Format;
		VkRenderPass RenderPass;
	};

	class VulkanFramebuffer
	{
	private:
		VkFramebuffer m_Framebuffer;

		VulkanFramebufferInfo m_Info;

		Shared<VulkanDevice> m_Device;
	public:
		VulkanFramebuffer(VulkanFramebufferInfo info, Shared<VulkanDevice> device);
		~VulkanFramebuffer();
	};

}