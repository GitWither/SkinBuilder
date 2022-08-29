#pragma once

#include <array>

#include "Core/Core.hpp"
#include "Graphics/VertexBufferLayout.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShader.hpp"

#include "vulkan/vulkan.h"

namespace SkinBuilder
{
	struct VulkanPipelineInfo
	{
		uint32_t UniformBuffers;
		VkRenderPass RenderPass;
		VertexBufferLayout Layout;
		Shared<VulkanShader> Shader;
	};

	class VulkanPipeline
	{
	private:
		VulkanPipelineInfo m_Info;
		Shared<VulkanDevice> m_Device;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
		VkDescriptorSetLayout m_DescriptorSetLayout;
	public:
		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		VkPipeline GetPipeline() const { return m_Pipeline; }

		VulkanPipeline(const VulkanPipelineInfo& info, const Shared<VulkanDevice>& device);
		~VulkanPipeline();
	};
}