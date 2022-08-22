#include <Platform/Vulkan/VulkanCommandBuffer.hpp>

namespace SkinBuilder
{
	VulkanCommandBuffer::VulkanCommandBuffer(Shared<VulkanDevice> device) : m_Device(device)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = device->GetQueueFamilyIndices().GraphicsFamily;

		SB_ASSERT(vkCreateCommandPool(m_Device->GetLogicalDevice(), &poolInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool")

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		SB_ASSERT(vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), nullptr, &m_CommandBuffer) == VK_SUCCESS, "Failed to allocate command buffers");
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_CommandPool, nullptr);
	}


}