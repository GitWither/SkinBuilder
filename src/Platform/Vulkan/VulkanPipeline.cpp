#include <Platform/Vulkan/VulkanPipeline.hpp>


namespace SkinBuilder
{

	constexpr std::array s_DynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};


	VulkanPipeline::VulkanPipeline(const VulkanPipelineInfo& info, const Shared<VulkanDevice>& device) : m_Info(info), m_Device(device)
	{
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(s_DynamicStates.size());
		dynamicStateInfo.pDynamicStates = s_DynamicStates.data();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterState{};
		rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterState.depthClampEnable = VK_FALSE;
		rasterState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterState.lineWidth = 1.0f;
		rasterState.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterState.depthBiasEnable = VK_FALSE;
		rasterState.depthBiasConstantFactor = 0.0f;
		rasterState.depthBiasClamp = 0.0f;
		rasterState.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo msInfo{};
		msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		msInfo.sampleShadingEnable = VK_FALSE;
		msInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		msInfo.minSampleShading = 1.0f;
		msInfo.pSampleMask = nullptr;
		msInfo.alphaToCoverageEnable = VK_FALSE;
		msInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo blendingInfo{};
		blendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendingInfo.logicOpEnable = VK_FALSE;
		blendingInfo.logicOp = VK_LOGIC_OP_COPY;
		blendingInfo.attachmentCount = 1;
		blendingInfo.pAttachments = &colorBlendAttachment;
		blendingInfo.blendConstants[0] = 0.0f;
		blendingInfo.blendConstants[1] = 0.0f;
		blendingInfo.blendConstants[2] = 0.0f;
		blendingInfo.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayout{};
		pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayout.setLayoutCount = 0;
		pipelineLayout.pSetLayouts = nullptr;
		pipelineLayout.pushConstantRangeCount = 0;
		pipelineLayout.pPushConstantRanges = nullptr;

		SB_ASSERT(vkCreatePipelineLayout(m_Device->GetLogicalDevice(), &pipelineLayout, nullptr, &m_PipelineLayout) == VK_SUCCESS, "Failed to create Vulkan pipeline layout")


		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = m_Info.Shader->GetPipelineShaderStages().data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterState;
		pipelineInfo.pMultisampleState = &msInfo;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &blendingInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = m_Info.RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		SB_ASSERT(vkCreateGraphicsPipelines(m_Device->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) == VK_SUCCESS, "Failed to create Vulkan pipeline")
	}

	VulkanPipeline::~VulkanPipeline()
	{

		vkDestroyPipeline(m_Device->GetLogicalDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetLogicalDevice(), m_PipelineLayout, nullptr);

		m_Device = nullptr;
	}


}
