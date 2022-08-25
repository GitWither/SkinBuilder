#include <Platform/Vulkan/VulkanFramebuffer.hpp>

namespace SkinBuilder
{
	VulkanFramebuffer::VulkanFramebuffer(VulkanFramebufferInfo info, Shared<VulkanDevice> device) : m_Info(info), m_Device(device)
	{

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_Info.RenderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &m_Info.ImageView;
		framebufferCreateInfo.width = m_Info.Width;
		framebufferCreateInfo.height = m_Info.Height;
		framebufferCreateInfo.layers = 1;

		vkCreateFramebuffer(device->GetLogicalDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffer);

	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		//vkDestroyRenderPass(m_Device->GetLogicalDevice(), m_Info.RenderPass, nullptr);
	}

}
