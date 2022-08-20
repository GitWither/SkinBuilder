#include "Platform/Vulkan/VulkanContext.hpp"
#include "Core/Window.hpp"

#include <GLFW/glfw3.h>

#include <vector>
#include <array>
#include <iostream>

namespace SkinBuilder
{

#ifdef SKINBUILDER_DEBUG
	constexpr bool s_EnableValidationLayers = true;
	constexpr std::array s_ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#else
	constexpr bool s_EnableValidationLayers = false;
#endif

	VulkanContext::VulkanContext(const Window* window)
	{
		if (s_EnableValidationLayers && !FetchValidationLayers())
		{
			//TODO: Assert
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Skin Builder";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Skin Builder";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		if constexpr (s_EnableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
		//TODO: Assert here

		uint32_t extensionCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		for (const auto& ext : extensions)
		{
			std::cout << ext.extensionName << std::endl;
		}

		glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(window->GetNative()), nullptr, &m_Surface);

		m_Device = MakeShared<VulkanDevice>(m_Instance, m_Surface);
		m_Swapchain = MakeShared<VulkanSwapchain>(m_Instance, m_Device, m_Surface);
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(m_Instance, nullptr);
	}

	bool VulkanContext::FetchValidationLayers()
	{
		uint32_t layers;
		vkEnumerateInstanceLayerProperties(&layers, nullptr);

		std::vector<VkLayerProperties> availableLayers(layers);
		vkEnumerateInstanceLayerProperties(&layers, availableLayers.data());

		for (const char* layer : s_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProps : availableLayers)
			{
				if (strcmp(layer, layerProps.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}
}
