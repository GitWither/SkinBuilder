#pragma once

#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanAllocator.hpp"

namespace SkinBuilder
{
	class VulkanIndexBuffer
	{
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation;

		uint32_t m_IndexCount = 0;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(uint32_t count, uint32_t* data, const Shared<VulkanDevice>& device);
		~VulkanIndexBuffer();

		void Bind(VkCommandBuffer buffer) const;

		uint32_t GetCount() const { return m_IndexCount; }
	};
}