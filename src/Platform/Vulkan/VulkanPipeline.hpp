#pragma once

#include <array>

#include "Core/Core.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShader.hpp"

#include "vulkan/vulkan.h"

namespace SkinBuilder
{
	struct VulkanPipelineInfo
	{
		VkRenderPass RenderPass;
		Shared<VulkanShader> Shader;
	};

	class VulkanPipeline
	{
	private:
		VulkanPipelineInfo m_Info;
		Shared<VulkanDevice> m_Device;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	public:
		VkPipeline GetPipeline() const { return m_Pipeline; }

		VulkanPipeline(VulkanPipelineInfo info, Shared<VulkanDevice> device);
		~VulkanPipeline();
	};
}