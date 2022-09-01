#include "Platform/Vulkan/VulkanAllocator.hpp"

namespace SkinBuilder::VulkanAllocator
{
	static VmaAllocator s_Allocator;
	static uint32_t s_TotalBytes = 0;

	VmaAllocation AllocateBuffer(VkBufferCreateInfo* bufferInfo, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlags memoryFlags, VkBuffer* resultBuffer)
	{
		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.usage = memoryUsage;
		allocationCreateInfo.requiredFlags = memoryFlags;

		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		SB_ASSERT(vmaCreateBuffer(s_Allocator, bufferInfo, &allocationCreateInfo, resultBuffer, &allocation, &allocationInfo) == VK_SUCCESS, "Failed to allocate buffer");

		return allocation;
	}

	VmaAllocation AllocateImage(VkImageCreateInfo createInfo, VmaMemoryUsage memoryUsage, VkImage& image)
	{
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = memoryUsage;

		VmaAllocation allocation;

		VK_ASSERT(vmaCreateImage(s_Allocator, &createInfo, &allocationCreateInfo, &image, &allocation, nullptr));

		return allocation;
	}


	void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
	{
		vmaDestroyBuffer(s_Allocator, buffer, allocation);
	}

	void FreeAllocation(VmaAllocation allocation)
	{
		vmaFreeMemory(s_Allocator, allocation);
	}

	void MapMemory(VmaAllocation allocation, void** resultMemory)
	{
		vmaMapMemory(s_Allocator, allocation, resultMemory);
	}

	void UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(s_Allocator, allocation);
	}

	void Initialize(VkInstance instance, const Shared<VulkanDevice>& device)
	{
		VmaAllocatorCreateInfo allocatorCreateInfo{};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_0;
		allocatorCreateInfo.physicalDevice = device->GetPhysicalDevice();
		allocatorCreateInfo.device = device->GetLogicalDevice();
		allocatorCreateInfo.instance = instance;

		SB_ASSERT(vmaCreateAllocator(&allocatorCreateInfo, &s_Allocator) == VK_SUCCESS, "Failed to create allocator");
	}

	void Shutdown()
	{
		vmaDestroyAllocator(s_Allocator);
	}
}
