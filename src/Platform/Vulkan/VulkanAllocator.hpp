#pragma once

#include "vk_mem_alloc.h"
#include "VulkanDevice.hpp"

namespace SkinBuilder
{
	namespace VulkanAllocator
	{
		VmaAllocation AllocateBuffer(VkBufferCreateInfo* bufferInfo, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags memoryFlags, VkBuffer* resultBuffer);
		void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
		void FreeAllocation(VmaAllocation allocation);
		void MapMemory(VmaAllocation allocation, void** resultMemory);
		void UnmapMemory(VmaAllocation allocation);
		void Initialize(VkInstance instance, const Shared<VulkanDevice>& device);
		void Shutdown();
	};
}
