#pragma once

#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "Core/Core.hpp"
#include "Core/Window.hpp"

namespace SkinBuilder
{
	class VulkanContext
	{
	public:
		VulkanContext(const Window* window);
		~VulkanContext();
	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		Shared<VulkanDevice> m_Device;
		Shared<VulkanSwapchain> m_Swapchain;

		bool FetchValidationLayers();
	};
}