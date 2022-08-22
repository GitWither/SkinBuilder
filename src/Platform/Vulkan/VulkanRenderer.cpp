#include <Platform/Vulkan/VulkanRenderer.hpp>

namespace SkinBuilder
{
	VulkanRenderer::VulkanRenderer(const Shared<VulkanContext>& context) : m_Context(context)
	{
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();
		Shared<VulkanDevice> device = m_Context->GetDevice();

		VulkanRenderPassInfo renderPassInfo{};
		renderPassInfo.SwapchainTarget = true;
		m_SwapchainRenderPass = MakeShared<VulkanRenderPass>(swapchain, renderPassInfo);

		VulkanPipelineInfo pipelineInfo{};
		pipelineInfo.Shader = MakeShared<VulkanShader>("main", device);
		pipelineInfo.ColorAttachmentFormat = swapchain->GetImageFormat();
		m_BasePipeline = MakeShared<VulkanPipeline>(pipelineInfo, device);

		uint32_t maxFramesInFlight = swapchain->GetMaxFramesInFlight();
		m_CommandPools.resize(maxFramesInFlight);
		m_CommandBuffers.resize(maxFramesInFlight);

		for (uint32_t i = 0; i < maxFramesInFlight; i++)
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo{};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_Context->GetDevice()->GetQueueFamilyIndices().GraphicsFamily;

			SB_ASSERT(vkCreateCommandPool(m_Context->GetDevice()->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPools[i]) == VK_SUCCESS, "Failed to create command pool")

				VkCommandBufferAllocateInfo commandBufferAllocInfo {};
			commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocInfo.commandBufferCount = 1;
			commandBufferAllocInfo.commandPool = m_CommandPools[i];
			commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			SB_ASSERT(vkAllocateCommandBuffers(m_Context->GetDevice()->GetLogicalDevice(), &commandBufferAllocInfo, &m_CommandBuffers[i]) == VK_SUCCESS, "Failed to allocate command buffer")
		}
	}

	VulkanRenderer::~VulkanRenderer()
	{
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();
		Shared<VulkanDevice> device = m_Context->GetDevice();

		vkDeviceWaitIdle(device->GetLogicalDevice());

		for (uint32_t i = 0; i < swapchain->GetMaxFramesInFlight(); i++)
		{
			vkDestroyCommandPool(device->GetLogicalDevice(), m_CommandPools[i], nullptr);
		}

		m_Context = nullptr;
	}

	void VulkanRenderer::Begin()
	{
		Shared<VulkanDevice> device = m_Context->GetDevice();
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();

		swapchain->AcquireNextFrame();


		uint32_t currentFrame = swapchain->GetCurrentFrame();

		vkResetCommandPool(device->GetLogicalDevice(), m_CommandPools[currentFrame], 0);
		VkCommandBuffer currentCommandBuffer = m_CommandBuffers[currentFrame];


		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.flags = 0;
		cmdBufferBeginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(currentCommandBuffer, &cmdBufferBeginInfo);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		VkImageMemoryBarrier2 imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
		imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.image = swapchain->GetCurrentImage();
		imageMemoryBarrier.subresourceRange = subresourceRange;

		VkDependencyInfo dependencyInfo{};
		dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.dependencyFlags = 0;
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;
		vkCmdPipelineBarrier2(currentCommandBuffer, &dependencyInfo);

		m_SwapchainRenderPass->Begin(currentCommandBuffer);
		vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_BasePipeline->GetPipeline());
	}

	void VulkanRenderer::End()
	{
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();

		VkCommandBuffer currentCommandBuffer = m_CommandBuffers[swapchain->GetCurrentFrame()];

		m_SwapchainRenderPass->End(currentCommandBuffer);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

		VkImageMemoryBarrier2 imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
		imageMemoryBarrier.dstAccessMask = VK_PIPELINE_STAGE_2_NONE;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageMemoryBarrier.image = swapchain->GetCurrentImage();
		imageMemoryBarrier.subresourceRange = subresourceRange;

		VkDependencyInfo dependencyInfo{};
		dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependencyInfo.dependencyFlags = 0;
		dependencyInfo.imageMemoryBarrierCount = 1;
		dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;
		vkCmdPipelineBarrier2(currentCommandBuffer, &dependencyInfo);


		SB_ASSERT(vkEndCommandBuffer(currentCommandBuffer) == VK_SUCCESS, "Failed to unbind command buffer");

	}


	void VulkanRenderer::Draw(float width, float height)
	{
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();
		VkExtent2D swapchainExtent = swapchain->GetExtent();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<uint32_t>(swapchainExtent.width);
		viewport.height = static_cast<uint32_t>(swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;

		VkCommandBuffer currentCommandBuffer = m_CommandBuffers[swapchain->GetCurrentFrame()];

		vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);
		vkCmdDraw(currentCommandBuffer, 3, 1, 0, 0);
	}

	void VulkanRenderer::Submit()
	{
		Shared<VulkanSwapchain> swapchain = m_Context->GetSwapchain();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { swapchain->GetImageAvailableSemaphore() };
		VkPipelineStageFlags pipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[swapchain->GetCurrentFrame()];

		VkSemaphore signalSemaphores[] = { m_Context->GetSwapchain()->GetRenderFinishedSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		SB_ASSERT(vkQueueSubmit(m_Context->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_Context->GetSwapchain()->GetFrameInFlightFence()) == VK_SUCCESS, "Failed to submit queue")

		m_Context->GetSwapchain()->SwapBuffers();
	}
}
