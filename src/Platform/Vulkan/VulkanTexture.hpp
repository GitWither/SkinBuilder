#pragma once

#include <Platform/Vulkan/VulkanDevice.hpp>

#include <filesystem>

namespace SkinBuilder
{
	class VulkanTexture
	{
	private:
		Shared<VulkanDevice> m_Device = nullptr;

		VkSampler m_Sampler = VK_NULL_HANDLE;
		VkImageView m_TextureImageView = VK_NULL_HANDLE;
		VkImage m_TextureImage = VK_NULL_HANDLE;
		VmaAllocation m_TextureAllocation = VK_NULL_HANDLE;
	public:
		VulkanTexture(const std::filesystem::path& path, const Shared<VulkanDevice>& device);
		~VulkanTexture();

		VkSampler GetSampler() const { return m_Sampler; }
		VkImageView GetImageView() const { return m_TextureImageView; }
	};
}