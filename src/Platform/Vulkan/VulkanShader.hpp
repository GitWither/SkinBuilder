#pragma once

#include "Core/Core.hpp"



#include "vulkan/vulkan.h"

#include <filesystem>
#include <iostream>
#include <array>
#include <fstream>

#include "VulkanDevice.hpp"

namespace SkinBuilder
{
	class VulkanShader
	{
	private:
		Shared<VulkanDevice> m_Device;
		VkShaderModule m_FragmentShader = VK_NULL_HANDLE;
		VkShaderModule m_VertexShader = VK_NULL_HANDLE;

		std::array<VkPipelineShaderStageCreateInfo, 2> m_PipelineShaderStages;
	public:
		VulkanShader(const std::filesystem::path& path, Shared<VulkanDevice> device);
		~VulkanShader();

		std::array<VkPipelineShaderStageCreateInfo, 2> GetPipelineShaderStages() const { return m_PipelineShaderStages; }
	};
}
