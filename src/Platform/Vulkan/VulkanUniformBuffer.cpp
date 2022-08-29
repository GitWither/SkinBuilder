#include "Platform/Vulkan/VulkanUniformBuffer.hpp"

namespace SkinBuilder
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding) : m_Size(size), m_Binding(binding)
	{
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = 0;
		allocateInfo.memoryTypeIndex = 0;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.size = size;

		m_BufferAllocation = VulkanAllocator::AllocateBuffer(&bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &m_Buffer);

		m_DescriptorInfo.buffer = m_Buffer;
		m_DescriptorInfo.offset = 0;
		m_DescriptorInfo.range = m_Size;
	}

	void VulkanUniformBuffer::SetData(void* data, uint32_t size, uint32_t offset)
	{
		void* memory;
		VulkanAllocator::MapMemory(m_BufferAllocation, &memory);
		std::memcpy(memory, static_cast<const uint8_t*>(data) + offset, size);
		VulkanAllocator::UnmapMemory(m_BufferAllocation);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VulkanAllocator::DestroyBuffer(m_Buffer, m_BufferAllocation);
	}


}