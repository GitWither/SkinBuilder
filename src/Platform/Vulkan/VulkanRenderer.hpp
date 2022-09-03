#pragma once

#include <vector>

#include "Core/Core.hpp"
#include "Core/SkinBuilder.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanPipeline.hpp"
#include "Platform/Vulkan/VulkanUniformBufferSet.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanContext.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanTexture.hpp"
#include "VulkanVertexBuffer.hpp"
#include "Graphics/Mesh.hpp"

namespace SkinBuilder
{
	class VulkanRenderer
	{
	private:
		std::vector<VkCommandPool> m_CommandPools;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		Shared<VulkanContext> m_Context;
		VulkanUniformBufferSet m_UniformBufferSet;

		Shared<VulkanPipeline> m_GeoPipeline;
		Shared<VulkanTexture> m_Texture;

		VkDescriptorPool m_DescriptorPool;
	public:
		VulkanRenderer(const Shared<VulkanContext>& context);
		~VulkanRenderer();

		VkCommandPool GetCommandPool() const { return m_CommandPools[m_Context->GetSwapchain()->GetCurrentFrameIndex()]; }
		VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffers[m_Context->GetSwapchain()->GetCurrentFrameIndex()]; }

		void Begin(const Camera& camera);
		void End();

		void Draw(float width, float height);
		void DrawMesh(const Shared<Mesh>& mesh);
		void Submit();
	};
}
