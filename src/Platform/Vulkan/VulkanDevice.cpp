#include <iostream>
#include <Platform/Vulkan/VulkanDevice.hpp>

#include <set>
#include <string>

namespace SkinBuilder
{
#ifdef SKINBUILDER_DEBUG
	constexpr bool s_EnableValidationLayers = true;
	constexpr std::array<const char*, 1> s_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#else
	constexpr bool s_EnableValidationLayers = false;
#endif

	constexpr std::array<const char*, 1> s_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			//TODO: Assert here
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				continue;
			}

			QueueFamilyIndices queueIndices = FindQueueFamilies(device, surface);

			if (!queueIndices.Validate())
			{
				continue;
			}

			if (!VerifyExtensionSupport(device))
			{
				continue;
			}

			m_QueueFamilyIndices = queueIndices;
			m_PhysicalDevice = device;
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			//TODO: Assert here
		}

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily;
		queueCreateInfo.queueCount = 1;

		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//VkPhysicalDeviceFeatures deviceFeatures{};
		VkPhysicalDeviceVulkan13Features deviceFeatures{};
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		deviceFeatures.dynamicRendering = VK_TRUE;
		deviceFeatures.synchronization2 = VK_TRUE;

		VkPhysicalDeviceFeatures2 deviceFeatures2{};
		deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures2.pNext = &deviceFeatures;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();
		deviceCreateInfo.pNext = &deviceFeatures2;

		if constexpr (s_EnableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();
		}
		else
		{
			deviceCreateInfo.enabledLayerCount = 0;
		}

		SB_ASSERT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) == VK_SUCCESS, "Failed to create logical device")

		vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.GraphicsFamily, 0, &m_Queue);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_Device, nullptr);
	}

	QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);


		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		std::vector<VkBool32> presentingSupportedQueues(queueFamilyCount);
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentingSupportedQueues[i]);
		}

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			{
				continue;
			}

			if (presentingSupportedQueues[i] == VK_TRUE)
			{
				indices.PresentFamily = i;
				indices.GraphicsFamily = i;
			}

			i++;
		}

		return indices;
	}

	bool VulkanDevice::VerifyExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> neededExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());


		for (const auto& extension : availableExtensions)
		{
			neededExtensions.erase(extension.extensionName);
		}

		return neededExtensions.empty();
	}
}
