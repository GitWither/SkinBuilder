#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include "Core/SkinBuilder.hpp"
#include "VulkanDevice.hpp"
#include "Core/Core.hpp"
#include <GLFW/glfw3.h>

namespace SkinBuilder
{
	class VulkanSwapchain
	{
	private:
		VkExtent2D m_SwapExtent;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

		VkInstance m_Instance = VK_NULL_HANDLE;
		Shared<VulkanDevice> m_Device;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkSwapchainKHR m_Swapchain;
	public:
		VulkanSwapchain(VkInstance instance, Shared<VulkanDevice> device, VkSurfaceKHR surface);
		~VulkanSwapchain();
	};
}