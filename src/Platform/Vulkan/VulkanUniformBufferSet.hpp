#pragma once

#include "Platform/Vulkan/VulkanUniformBuffer.hpp"

#include <unordered_map>

namespace SkinBuilder
{
	class VulkanUniformBufferSet
	{
	private:
		uint32_t m_MaxFramesInFlight = 0;
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, Shared<VulkanUniformBuffer>>> m_UniformBuffers;
	public:
		VulkanUniformBufferSet(uint32_t framesInFlight) : m_MaxFramesInFlight(framesInFlight) {}
		~VulkanUniformBufferSet() {
			m_UniformBuffers.clear();
		}

		void Create(uint32_t size, uint32_t binding)
		{
			for (uint32_t frame = 0; frame < m_MaxFramesInFlight; frame++)
			{
				Shared<VulkanUniformBuffer> uniformBuffer = MakeShared<VulkanUniformBuffer>(size, binding);
				Set(uniformBuffer, frame);
			}
		}

		Shared<VulkanUniformBuffer> Get(uint32_t binding, uint32_t frame) const
		{
			return m_UniformBuffers.at(frame).at(binding);
		}

		void Set(const Shared<VulkanUniformBuffer>& uniformBuffer, uint32_t frame)
		{
			m_UniformBuffers[frame][uniformBuffer->GetBinding()] = uniformBuffer;
		}
	};
}
