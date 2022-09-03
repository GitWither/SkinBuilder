#include <Platform/Vulkan/VulkanRenderer.hpp>

#include "Graphics/Vertex.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace SkinBuilder
{
	VulkanRenderer::VulkanRenderer(const Shared<VulkanContext>& context)
	:
	m_Context(context),
	m_UniformBufferSet(m_Context->GetSwapchain()->GetMaxFramesInFlight())
	{
		const VulkanSwapchain& swapchain = *context->GetSwapchain();

		m_Texture = MakeShared<VulkanTexture>("steve.png", context->GetDevice());

		const uint32_t maxFramesInFlight = swapchain.GetMaxFramesInFlight();

		m_CommandBuffers.resize(maxFramesInFlight);
		m_CommandPools.resize(maxFramesInFlight);

		m_UniformBufferSet.Create(sizeof(glm::mat4), 0);

		VulkanPipelineInfo pipelineInfo;
		pipelineInfo.RenderPass = m_Context->GetSwapchain()->GetRenderPass();
		pipelineInfo.Shader = MakeShared<VulkanShader>("main", m_Context->GetDevice());
		pipelineInfo.UniformBuffers = 1;
		pipelineInfo.Layout = {
			{0, DataType::Vector3, offsetof(Vertex, Position)},
			{1, DataType::Vector2, offsetof(Vertex, TexCoords)}
		};

		m_GeoPipeline = MakeShared<VulkanPipeline>(pipelineInfo, m_Context->GetDevice());


		const std::array<VkDescriptorPoolSize, 2> poolSizes = {
			{
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxFramesInFlight},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxFramesInFlight}
			}
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = maxFramesInFlight;

		VK_ASSERT(vkCreateDescriptorPool(m_Context->GetDevice()->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool));

		m_DescriptorSetLayouts.resize(maxFramesInFlight, m_GeoPipeline->GetDescriptorSetLayout());

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = maxFramesInFlight;
		descriptorSetAllocateInfo.pSetLayouts = m_DescriptorSetLayouts.data();

		m_DescriptorSets.resize(maxFramesInFlight);

		vkAllocateDescriptorSets(m_Context->GetDevice()->GetLogicalDevice(), &descriptorSetAllocateInfo, m_DescriptorSets.data());

		for (size_t i = 0; i < maxFramesInFlight; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBufferSet.Get(0, i)->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(glm::mat4);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_Texture->GetImageView();
			imageInfo.sampler = m_Texture->GetSampler();

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(m_Context->GetDevice()->GetLogicalDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}



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

		vkDestroyDescriptorPool(m_Context->GetDevice()->GetLogicalDevice(), m_DescriptorPool, nullptr);


		for (uint32_t i = 0; i < m_Context->GetSwapchain()->GetMaxFramesInFlight(); i++)
		{
			vkDestroyCommandPool(m_Context->GetDevice()->GetLogicalDevice(), m_CommandPools[i], nullptr);
		}

		m_Context = nullptr;
	}

	void VulkanRenderer::Begin(const Camera& camera)
	{

		auto swapchain = m_Context->GetSwapchain();

		swapchain->AcquireNextFrame();

		uint32_t currentFrame = swapchain->GetCurrentFrameIndex();


		glm::mat4 viewProjection = camera.GetViewProjection();

		Shared<VulkanUniformBuffer> uniformBuffer = m_UniformBufferSet.Get(0, currentFrame);
		uniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));

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
		vkCmdBindPipeline(m_CommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GeoPipeline->GetPipeline());
	}

	void VulkanRenderer::End()
	{
		const VkCommandBuffer currentCommandBuffer = m_CommandBuffers[m_Context->GetSwapchain()->GetCurrentFrameIndex()];

		vkCmdEndRenderPass(currentCommandBuffer);
		SB_ASSERT(vkEndCommandBuffer(currentCommandBuffer) == VK_SUCCESS, "Failed to unbind command buffer");

	}


	void VulkanRenderer::Draw(float width, float height)
	{
		const VkExtent2D swapchainExtent = m_Context->GetSwapchain()->GetExtent();
		
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

		const uint32_t currentFrame = m_Context->GetSwapchain()->GetCurrentFrameIndex();
		const VkCommandBuffer& currentCommandBuffer = m_CommandBuffers[currentFrame];

		vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);
		//vkCmdDraw(currentCommandBuffer, 3, 1, 0, 0);
		vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GeoPipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[currentFrame], 0, nullptr);
		//vkCmdDrawIndexed(currentCommandBuffer, m_IndexBuffer.GetCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::DrawMesh(const Shared<Mesh>& mesh)
	{
		const uint32_t currentFrame = m_Context->GetSwapchain()->GetCurrentFrameIndex();
		const VkCommandBuffer& currentCommandBuffer = m_CommandBuffers[currentFrame];

		mesh->GetIndexBuffer()->Bind(currentCommandBuffer);
		mesh->GetVertexBuffer()->Bind(currentCommandBuffer);

		vkCmdDrawIndexed(currentCommandBuffer, mesh->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::Submit()
	{
		const VulkanSwapchain& swapchain = *m_Context->GetSwapchain();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		constexpr VkPipelineStageFlags pipelineStageFlags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const VkSemaphore waitSemaphores[] = { m_Context->GetSwapchain()->GetImageAvailableSemaphore() };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = pipelineStageFlags;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[swapchain.GetCurrentFrameIndex()];

		const VkSemaphore signalSemaphores[] = { m_Context->GetSwapchain()->GetRenderFinishedSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		SB_ASSERT(vkQueueSubmit(m_Context->GetDevice()->GetGraphicsQueue(), 1, &submitInfo, m_Context->GetSwapchain()->GetFrameInFlightFence()) == VK_SUCCESS, "Failed to submit queue")

		m_Context->GetSwapchain()->SwapBuffers();
	}
}
