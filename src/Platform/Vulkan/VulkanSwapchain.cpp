#include <iostream>
#include <vector>
#include <Platform/Vulkan/VulkanSwapchain.hpp>

namespace SkinBuilder
{
	VulkanSwapchain::VulkanSwapchain(VkInstance instance, const Shared<VulkanDevice>& device, VkSurfaceKHR surface) : m_Instance(instance), m_Device(device), m_Surface(surface)
	{
		//TODO: Assert this
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->GetPhysicalDevice(), m_Surface, &m_SurfaceCapabilities);

		uint32_t availableFormats = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Surface, &availableFormats, nullptr);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(availableFormats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Surface, &availableFormats, surfaceFormats.data());

		bool foundFormat = false;
		for (const auto& availableFormat : surfaceFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_SurfaceFormat = availableFormat;
				foundFormat = true;
				break;
			}
		}

		if (!foundFormat)
		{
			m_SurfaceFormat = surfaceFormats[0];
		}

		uint32_t availablePresentModesCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Surface, &availablePresentModesCount, nullptr);


		std::vector<VkPresentModeKHR> availablePresentModes(availablePresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Surface, &availablePresentModesCount, availablePresentModes.data());


		bool foundMode = false;
		for (const auto& presentMode : availablePresentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_PresentMode = presentMode;
				foundMode = true;
				break;
			}
		}

		if (!foundMode)
		{
			m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}


		if (m_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			m_SwapExtent = m_SurfaceCapabilities.currentExtent;
		}
		else
		{
			int width = 0, height = 0;
			glfwGetFramebufferSize(static_cast<GLFWwindow*>(SkinBuilder::GetInstance().GetWindow()->GetNative()), &width, &height);
			m_SwapExtent.width = std::clamp(static_cast<uint32_t>(width), m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
			m_SwapExtent.height = std::clamp(static_cast<uint32_t>(width), m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
		}

		uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
		{
			imageCount = m_SurfaceCapabilities.maxImageCount;
		}

		m_MaxFramesInFlight = imageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_SurfaceFormat.format;
		createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		createInfo.imageExtent = m_SwapExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const QueueFamilyIndices queueFamilies = m_Device->GetQueueFamilyIndices();
		const std::array queueFamilyIndices = {queueFamilies.GraphicsFamily, queueFamilies.PresentFamily};

		if (queueFamilies.GraphicsFamily != queueFamilies.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		SB_ASSERT(vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &createInfo, nullptr, &m_Swapchain) == VK_SUCCESS, "Failed to create swap chain")

		vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &imageCount, nullptr);

		m_Images.resize(imageCount);
		m_ImageViews.resize(imageCount);

		vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_Swapchain, &imageCount, m_Images.data());


		for (size_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo viewCreateInfo{};
			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCreateInfo.image = m_Images[i];
			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.format = m_SurfaceFormat.format;
			viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCreateInfo.subresourceRange.baseMipLevel = 0;
			viewCreateInfo.subresourceRange.levelCount = 1;
			viewCreateInfo.subresourceRange.baseArrayLayer = 0;
			viewCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device->GetLogicalDevice(), &viewCreateInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
			{
				//TODO: Assert here
			}
		}

		m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
		m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
		m_FrameInFlightFences.resize(m_MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo frameInFlightInfo{};
		frameInFlightInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		frameInFlightInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < m_MaxFramesInFlight; i++)
		{
			vkCreateSemaphore(m_Device->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
			vkCreateSemaphore(m_Device->GetLogicalDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
			vkCreateFence(m_Device->GetLogicalDevice(), &frameInFlightInfo, nullptr, &m_FrameInFlightFences[i]);
		}
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		for (uint32_t i = 0; i < m_MaxFramesInFlight; i++)
		{
			vkDestroyFence(m_Device->GetLogicalDevice(), m_FrameInFlightFences[i], nullptr);
			vkDestroySemaphore(m_Device->GetLogicalDevice(), m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device->GetLogicalDevice(), m_RenderFinishedSemaphores[i], nullptr);
		}
		for (uint32_t i = 0; i < m_ImageViews.size(); i++)
		{
			vkDestroyImageView(m_Device->GetLogicalDevice(), m_ImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_Swapchain, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		m_Device = nullptr;
	}

	void VulkanSwapchain::AcquireNextFrame()
	{
		vkWaitForFences(m_Device->GetLogicalDevice(), 1, &m_FrameInFlightFences[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetLogicalDevice(), 1, &m_FrameInFlightFences[m_CurrentImageIndex]);

		vkAcquireNextImageKHR(m_Device->GetLogicalDevice(), m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentImageIndex], VK_NULL_HANDLE, &m_CurrentImageIndex);
	}

	void VulkanSwapchain::SwapBuffers()
	{

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;



		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentImageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_CurrentImageIndex;
		presentInfo.pResults = nullptr;

		SB_ASSERT(vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &presentInfo) == VK_SUCCESS, "Failed to present swapchain")

		m_CurrentImageIndex = (m_CurrentImageIndex + 1) % m_MaxFramesInFlight;
	}
}
