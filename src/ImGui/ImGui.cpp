#include "ImGui.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"
#include "Platform/Vulkan/VulkanRenderer.hpp"

namespace SkinBuilder
{
	void ImGuiHelper::Initialize(const Shared<VulkanContext>& context, const VulkanRenderer& renderer, const Window* window)
	{
		ImGui::CreateContext();

		VkDescriptorPool descriptorPool;
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		vkCreateDescriptorPool(context->GetDevice()->GetLogicalDevice(), &pool_info, nullptr, &descriptorPool);

		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window->GetNative()), true);
		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = context->GetInstance();
		initInfo.PhysicalDevice = context->GetDevice()->GetPhysicalDevice();
		initInfo.Device = context->GetDevice()->GetLogicalDevice();
		initInfo.QueueFamily = context->GetDevice()->GetQueueFamilyIndices().GraphicsFamily;
		initInfo.Queue = context->GetDevice()->GetGraphicsQueue();
		initInfo.PipelineCache = nullptr;
		initInfo.DescriptorPool = descriptorPool;
		initInfo.Subpass = 0;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = context->GetSwapchain()->GetImageCount();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&initInfo, context->GetSwapchain()->GetRenderPass());

		{
			// Use any command queue
			//VkCommandPool command_pool = renderer.GetCommandPool();
			VkCommandBuffer command_buffer = renderer.GetCommandBuffer();

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(command_buffer, &begin_info);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			vkEndCommandBuffer(command_buffer);
			vkQueueSubmit(context->GetDevice()->GetGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE);

			vkDeviceWaitIdle(context->GetDevice()->GetLogicalDevice());
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	void ImGuiHelper::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


	void ImGuiHelper::End(const VkCommandBuffer buffer)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
	}

	void ImGuiHelper::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
	}


}
