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
		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

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
