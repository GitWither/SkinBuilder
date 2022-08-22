#pragma once

#include <vector>

#include "Core/Core.hpp"
#include "Core/SkinBuilder.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanPipeline.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanContext.hpp"

namespace SkinBuilder
{
	class VulkanRenderer
	{
	private:
		std::vector<VkCommandPool> m_CommandPools;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		Shared<VulkanRenderPass> m_SwapchainRenderPass;
		Shared<VulkanPipeline> m_BasePipeline;

		Shared<VulkanContext> m_Context;
	public:
		VulkanRenderer(const Shared<VulkanContext>& context);
		~VulkanRenderer();

		void Begin();
		void End();

		void Draw(float width, float height);
		void Submit();
	};
}
