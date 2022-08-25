#include <Platform/Vulkan/VulkanRenderPass.hpp>

namespace SkinBuilder
{
	VulkanRenderPass::VulkanRenderPass(Shared<VulkanSwapchain> swapchain) : m_Swapchain(swapchain)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_Swapchain->GetImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		
	}


}