#include <Platform/Vulkan/VulkanShader.hpp>

namespace SkinBuilder
{
	VulkanShader::VulkanShader(const std::filesystem::path& path, Shared<VulkanDevice> device) : m_Device(device)
	{
		{
			std::ifstream shader(path.string() + ".frag.spv", std::ios::ate | std::ios::binary);

			SB_ASSERT(shader.is_open(), "Could not open fragment shader")

			const size_t shaderSize = shader.tellg();
			std::vector<char> shaderBuffer(shaderSize);

			shader.seekg(0);
			shader.read(shaderBuffer.data(), shaderSize);

			shader.close();

			VkShaderModuleCreateInfo fragmentCreateInfo{};
			fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			fragmentCreateInfo.codeSize = shaderBuffer.size();
			fragmentCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBuffer.data());

			SB_ASSERT(vkCreateShaderModule(m_Device->GetLogicalDevice(), &fragmentCreateInfo, nullptr, &m_FragmentShader) == VK_SUCCESS, "Could not create Fragment shader module")
		}

		{
			std::ifstream shader(path.string() + ".vert.spv", std::ios::ate | std::ios::binary);

			SB_ASSERT(shader.is_open(), "Could not open fragment shader")

				const size_t shaderSize = shader.tellg();
			std::vector<char> shaderBuffer(shaderSize);

			shader.seekg(0);
			shader.read(shaderBuffer.data(), shaderSize);

			shader.close();

			VkShaderModuleCreateInfo fragmentCreateInfo{};
			fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			fragmentCreateInfo.codeSize = shaderBuffer.size();
			fragmentCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBuffer.data());

			SB_ASSERT(vkCreateShaderModule(m_Device->GetLogicalDevice(), &fragmentCreateInfo, nullptr, &m_VertexShader) == VK_SUCCESS, "Could not create Vertex shader module")
		}

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_FragmentShader;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_VertexShader;
		vertShaderStageInfo.pName = "main";

		m_PipelineShaderStages = { vertShaderStageInfo, fragShaderStageInfo };
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_VertexShader, nullptr);
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_FragmentShader, nullptr);
	}
}
