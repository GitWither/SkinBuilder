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

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		std::vector<VkImage> m_Images;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_FrameInFlightFence;

		uint32_t m_CurrentImageIndex = 0;
		uint32_t m_ImageCount = 0;


	public:
		VulkanSwapchain(VkInstance instance, Shared<VulkanDevice> device, VkSurfaceKHR surface);
		~VulkanSwapchain();

		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }
		VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphore; }
		VkFence GetFrameInFlightFence() const { return m_FrameInFlightFence; }

		VkFormat GetImageFormat() const { return m_SurfaceFormat.format; }
		VkRenderPass GetRenderPass() const { return m_RenderPass; }
		VkFramebuffer GetFramebuffer(uint32_t imageIndex) const
		{
			return m_Framebuffers[imageIndex];
		}

		VkExtent2D GetExtent() const { return m_SwapExtent; }

		uint32_t GetCurrentFrameIndex() const { return m_CurrentImageIndex; }
		uint32_t GetImageCount() const { return m_ImageCount; }
		void AcquireNextFrame();
		void SwapBuffers();
	};
}