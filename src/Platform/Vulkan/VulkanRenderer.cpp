#include <Platform/Vulkan/VulkanRenderer.hpp>

#include "Graphics/Vertex.hpp"

namespace SkinBuilder
{
	static Vertex s_TriangleVertices[] = {
	{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Top-Right
	{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Bottom-Right
	{ { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Bottom-Left
	{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Top-Left
	};

	static uint32_t s_TriangleIndices[] = { 0, 1, 2, 2, 3, 0 };

	VulkanRenderer::VulkanRenderer(const Shared<VulkanContext>& context) : m_Context(context), m_IndexBuffer(6, s_TriangleIndices, context->GetDevice()), m_VertexBuffer(4 * sizeof(Vertex), s_TriangleVertices, context->GetDevice())
	{
		const VulkanSwapchain& swapchain = *context->GetSwapchain();

		const uint32_t maxFramesInFlight = swapchain.GetMaxFramesInFlight();

		m_CommandBuffers.resize(maxFramesInFlight);
		m_CommandPools.resize(maxFramesInFlight);

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

		//m_VertexBuffer = VulkanVertexBuffer(4 * sizeof(Vertex), s_TriangleVertices, m_Context->GetDevice());
		//m_IndexBuffer = VulkanIndexBuffer(6, s_TriangleIndices, m_Context->GetDevice());
	}

	VulkanRenderer::~VulkanRenderer()
	{
		vkDeviceWaitIdle(m_Context->GetDevice()->GetLogicalDevice());

		for (uint32_t i = 0; i < m_Context->GetSwapchain()->GetMaxFramesInFlight(); i++)
		{
			vkDestroyCommandPool(m_Context->GetDevice()->GetLogicalDevice(), m_CommandPools[i], nullptr);
		}

		m_Context = nullptr;
	}

	void VulkanRenderer::Begin()
	{

		auto swapchain = m_Context->GetSwapchain();

		swapchain->AcquireNextFrame();

		uint32_t currentFrame = swapchain->GetCurrentFrameIndex();

		vkResetCommandPool(m_Context->GetDevice()->GetLogicalDevice(), m_CommandPools[currentFrame], 0);
		//vkResetCommandBuffer(m_CommandBuffers[currentFrame], 0);

		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.flags = 0;
		cmdBufferBeginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(m_CommandBuffers[currentFrame], &cmdBufferBeginInfo);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->GetRenderPass();
		renderPassInfo.framebuffer = swapchain->GetFramebuffer(swapchain->GetCurrentFrameIndex());
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->GetExtent();

		VkClearValue clearVal = { {{1.0f, 1.0f, 1.0f, 1.0f }} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearVal;

		vkCmdBeginRenderPass(m_CommandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//wtf
		vkCmdBindPipeline(m_CommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->GetPipeline()->GetPipeline());

		m_IndexBuffer.Bind(m_CommandBuffers[currentFrame]);
		m_VertexBuffer.Bind(m_CommandBuffers[currentFrame]);
	}

	void VulkanRenderer::End()
	{
		const VkCommandBuffer currentCommandBuffer = m_CommandBuffers[m_Context->GetSwapchain()->GetCurrentFrameIndex()];

		vkCmdEndRenderPass(currentCommandBuffer);
		SB_ASSERT(vkEndCommandBuffer(currentCommandBuffer) == VK_SUCCESS, "Failed to unbind command buffer");

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

		const VkCommandBuffer currentCommandBuffer = m_CommandBuffers[m_Context->GetSwapchain()->GetCurrentFrameIndex()];

		vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);
		//vkCmdDraw(currentCommandBuffer, 3, 1, 0, 0);
		vkCmdDrawIndexed(currentCommandBuffer, m_IndexBuffer.GetCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::Submit()
	{
		const VulkanSwapchain& swapchain = *m_Context->GetSwapchain();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_Context->GetSwapchain()->GetImageAvailableSemaphore() };
		VkPipelineStageFlags pipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[swapchain.GetCurrentFrameIndex()];

		VkSemaphore signalSemaphores[] = { m_Context->GetSwapchain()->GetRenderFinishedSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		SB_ASSERT(vkQueueSubmit(m_Context->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_Context->GetSwapchain()->GetFrameInFlightFence()) == VK_SUCCESS, "Failed to submit queue")

		m_Context->GetSwapchain()->SwapBuffers();
	}
}
