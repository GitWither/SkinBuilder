#pragma once

#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "Core/Core.hpp"
#include "Core/Window.hpp"

namespace SkinBuilder
{
	class VulkanContext
	{
	public:
		VulkanContext(const Window* window);
		~VulkanContext();

		Shared<VulkanSwapchain> GetSwapchain() const { return m_Swapchain; }
		Shared<VulkanPipeline> GetPipeline() const { return m_Pipeline; }
		Shared<VulkanDevice> GetDevice() const { return m_Device; }
		VkInstance GetInstance() const { return m_Instance; }
		VkSurfaceKHR GetSurface() const { return m_Surface; }
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		Shared<VulkanDevice> m_Device;
		Shared<VulkanSwapchain> m_Swapchain;
		Shared<VulkanPipeline> m_Pipeline;

		bool FetchValidationLayers();
	};
}
