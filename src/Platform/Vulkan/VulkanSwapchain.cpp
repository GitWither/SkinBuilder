#include <vector>
#include <Platform/Vulkan/VulkanSwapchain.hpp>

namespace SkinBuilder
{
	VulkanSwapchain::VulkanSwapchain(VkInstance instance, Shared<VulkanDevice> device, VkSurfaceKHR surface) : m_Instance(instance), m_Device(std::move(device)), m_Surface(surface)
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
			glfwGetFramebufferSize(static_cast<GLFWwindow*>(SkinBuilder::GetInstance()->GetWindow()->GetNative()), &width, &height);
			m_SwapExtent.width = std::clamp(static_cast<uint32_t>(width), m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
			m_SwapExtent.height = std::clamp(static_cast<uint32_t>(width), m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
		}

		uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;
		if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount)
		{
			imageCount = m_SurfaceCapabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		createInfo.imageFormat = m_SurfaceFormat.format;
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

		if (vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
		{
			//TODO: Assert
		}
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_Swapchain, nullptr);
	}


}