#include "Platform/Vulkan/VulkanIndexBuffer.hpp"

namespace SkinBuilder
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, uint32_t* data, const Shared<VulkanDevice>& device) : m_IndexCount(count)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.size = count * sizeof(uint32_t);
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer buffer;
		VmaAllocation bufferAllocation = VulkanAllocator::AllocateBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer);

		void* mappedMemory;
		VulkanAllocator::MapMemory(bufferAllocation, &mappedMemory);
		std::memcpy(mappedMemory, data, count * sizeof(uint32_t));
		VulkanAllocator::UnmapMemory(bufferAllocation);
		
		VkBufferCreateInfo actualBufferInfo{};
		actualBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		actualBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		actualBufferInfo.size = count * sizeof(uint32_t);
		actualBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		m_BufferAllocation = VulkanAllocator::AllocateBuffer(&actualBufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_Buffer);

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = 0;
		commandPoolInfo.queueFamilyIndex = device->GetQueueFamilyIndices().GraphicsFamily;

		VkCommandPool commandPool;
		VK_ASSERT(vkCreateCommandPool(device->GetLogicalDevice(), &commandPoolInfo, nullptr, &commandPool));

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandPool = commandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VK_ASSERT(vkAllocateCommandBuffers(device->GetLogicalDevice(), &commandBufferInfo, &commandBuffer));

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		VkBufferCopy bufferCopy{};
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		bufferCopy.size = count * sizeof(uint32_t);
		vkCmdCopyBuffer(commandBuffer, buffer, m_Buffer, 1, &bufferCopy);

		VK_ASSERT(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		VK_ASSERT(vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
		VK_ASSERT(vkQueueWaitIdle(device->GetGraphicsQueue()));

		vkFreeCommandBuffers(device->GetLogicalDevice(), commandPool, 1, &commandBuffer);
		vkDestroyCommandPool(device->GetLogicalDevice(), commandPool, nullptr);

		//delete staging buffer
		VulkanAllocator::DestroyBuffer(buffer, bufferAllocation);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_Buffer, m_BufferAllocation);
	}

	void VulkanIndexBuffer::Bind(VkCommandBuffer buffer)
	{
		vkCmdBindIndexBuffer(buffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
	}



}