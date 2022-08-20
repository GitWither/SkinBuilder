#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include "Core/Core.hpp"

namespace SkinBuilder
{
	template <typename T>
	using Limit = std::numeric_limits<T>;

	struct QueueFamilyIndices
	{
		uint32_t GraphicsFamily;
		uint32_t PresentFamily;

		bool Validate() const
		{
			return GraphicsFamily != Limit<uint32_t>::max() && PresentFamily != Limit<uint32_t>::max();
		}
	};


	class VulkanDevice
	{
	public:
		VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
		~VulkanDevice();

		QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetLogicalDevice() const { return m_Device; }
	private:
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		bool VerifyExtensionSupport(VkPhysicalDevice device);


		QueueFamilyIndices m_QueueFamilyIndices;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkQueue m_Queue = VK_NULL_HANDLE;
	};
}