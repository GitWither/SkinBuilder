#pragma once

#include "VulkanContext.hpp"
#include "VulkanAllocator.hpp"

#include "vk_mem_alloc.h"

namespace SkinBuilder
{
	class VulkanVertexBuffer
	{
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation;
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(uint32_t count, void* data, const Shared<VulkanDevice> device);
		~VulkanVertexBuffer();

		void Bind(VkCommandBuffer commandBuffer) const;
	};
}