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

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_FrameInFlightFences;

		uint32_t m_CurrentImageIndex = 0;

		uint32_t m_MaxFramesInFlight;


	public:
		VulkanSwapchain(VkInstance instance, const Shared<VulkanDevice>& device, VkSurfaceKHR surface);
		~VulkanSwapchain();

		uint32_t GetMaxFramesInFlight() const { return m_MaxFramesInFlight; }
		uint32_t GetCurrentFrame() const { return m_CurrentImageIndex; }

		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphores[m_CurrentImageIndex]; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentImageIndex]; }
		VkFence GetFrameInFlightFence() const { return m_FrameInFlightFences[m_CurrentImageIndex]; }

		VkImageView GetCurrentImageView() const { return m_ImageViews[m_CurrentImageIndex]; }
		VkImage GetCurrentImage() const { return m_Images[m_CurrentImageIndex]; }

		VkFormat GetImageFormat() const { return m_SurfaceFormat.format; }

		VkExtent2D GetExtent() const { return m_SwapExtent; }

		uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }
		void AcquireNextFrame();
		void SwapBuffers();
	};
}