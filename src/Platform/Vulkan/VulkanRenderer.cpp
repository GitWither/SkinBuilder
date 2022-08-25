#include <Platform/Vulkan/VulkanRenderer.hpp>

namespace SkinBuilder
{
	VulkanRenderer::VulkanRenderer(const Shared<VulkanContext>& context) : m_Context(context)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_Context->GetDevice()->GetQueueFamilyIndices().GraphicsFamily;

		SB_ASSERT(vkCreateCommandPool(m_Context->GetDevice()->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool")

			VkCommandBufferAllocateInfo commandBufferAllocInfo {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = 1;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		SB_ASSERT(vkAllocateCommandBuffers(m_Context->GetDevice()->GetLogicalDevice(), &commandBufferAllocInfo, &m_CommandBuffer) == VK_SUCCESS, "Failed to allocate command buffer")
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkDeviceWaitIdle(m_Context->GetDevice()->GetLogicalDevice());

		vkDestroyCommandPool(m_Context->GetDevice()->GetLogicalDevice(), m_CommandPool, nullptr);

		m_Context = nullptr;
	}

	void VulkanRenderer::Begin()
	{

		auto swapchain = m_Context->GetSwapchain();

		swapchain->AcquireNextFrame();

		vkResetCommandBuffer(m_CommandBuffer, 0);

		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.flags = 0;
		cmdBufferBeginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(m_CommandBuffer, &cmdBufferBeginInfo);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->GetRenderPass();
		renderPassInfo.framebuffer = swapchain->GetFramebuffer(swapchain->GetCurrentFrameIndex());
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->GetExtent();

		VkClearValue clearVal = { {{1.0f, 1.0f, 1.0f, 1.0f }} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearVal;

		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//wtf
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->GetPipeline()->GetPipeline());
	}

	void VulkanRenderer::End()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
		SB_ASSERT(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS, "Failed to unbind command buffer");

	}


	void VulkanRenderer::Draw(float width, float height)
	{
		VkExtent2D swapchainExtent = m_Context->GetSwapchain()->GetExtent();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = swapchainExtent.width;
		viewport.height = swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;

		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
		vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);
	}

	void VulkanRenderer::Submit()
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_Context->GetSwapchain()->GetImageAvailableSemaphore() };
		VkPipelineStageFlags pipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		VkSemaphore signalSemaphores[] = { m_Context->GetSwapchain()->GetRenderFinishedSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		SB_ASSERT(vkQueueSubmit(m_Context->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_Context->GetSwapchain()->GetFrameInFlightFence()) == VK_SUCCESS, "Failed to submit queue")

		m_Context->GetSwapchain()->SwapBuffers();
	}
}
