#include <Platform/Vulkan/VulkanRenderPass.hpp>

namespace SkinBuilder
{

	VulkanRenderPass::VulkanRenderPass(const Shared<VulkanSwapchain>& swapchain, const VulkanRenderPassInfo& info) : m_Info(info), m_Swapchain(swapchain)
	{
		m_Info.TargetImageSize = m_Info.SwapchainTarget ? m_Swapchain->GetExtent() : m_Info.TargetImageSize;
	}

	void VulkanRenderPass::Begin(VkCommandBuffer commandBuffer)
	{
		VkImageView imageView = m_Info.SwapchainTarget ? m_Swapchain->GetCurrentImageView() : m_Info.TargetImageView;

		VkRenderingAttachmentInfo colorAttachmentInfo{};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.clearValue = { {{1.0f, 1.0f, 1.0f, 1.0f}} };
		colorAttachmentInfo.imageView = imageView;
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.flags = 0;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = m_Info.TargetImageSize;
		renderingInfo.layerCount = 1;
		renderingInfo.viewMask = 0;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachmentInfo;
		renderingInfo.pDepthAttachment = nullptr;
		renderingInfo.pStencilAttachment = nullptr;

		vkCmdBeginRendering(commandBuffer, &renderingInfo);
	}

	void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRendering(commandBuffer);
	}

	VkImageView VulkanRenderPass::GetImageView() const
	{
		return m_Info.SwapchainTarget ? m_Swapchain->GetCurrentImageView() : m_Info.TargetImageView;
	}


	VulkanRenderPass::~VulkanRenderPass()
	{
		m_Swapchain = nullptr;
	}


}