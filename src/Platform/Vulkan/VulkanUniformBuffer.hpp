#pragma once

#include "Platform/Vulkan/VulkanAllocator.hpp"

namespace SkinBuilder
{
	class VulkanUniformBuffer
	{
	private:
		VmaAllocation m_BufferAllocation;
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDescriptorBufferInfo m_DescriptorInfo{};
		uint32_t m_Size = 0;
		uint32_t m_Binding = 0;
		VkShaderStageFlagBits m_ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		~VulkanUniformBuffer();

		void SetData(void* data, uint32_t size, uint32_t offset = 0);
		uint32_t GetBinding() const { return m_Binding; }
		VkBuffer GetBuffer() const { return m_Buffer; }
	};
}