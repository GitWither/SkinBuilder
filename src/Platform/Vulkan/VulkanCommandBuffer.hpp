#pragma once

#include "Core/Core.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"

namespace SkinBuilder
{
	class VulkanCommandBuffer {
	private:
		Shared<VulkanDevice> m_Device;

		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;
	public:
		VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

		VulkanCommandBuffer(Shared<VulkanDevice> device);
		~VulkanCommandBuffer();
	};
}