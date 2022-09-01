#include "Core/Core.hpp"

#include "Platform/Vulkan/VulkanAllocator.hpp"

#include "VulkanTexture.hpp"

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace SkinBuilder
{
	VulkanTexture::VulkanTexture(const std::filesystem::path& path, const Shared<VulkanDevice>& device) : m_Device(device)
	{
		int width, height, channels;

		stbi_uc* imageData = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		SB_ASSERT(imageData, "Failed to load texture")

		const VkDeviceSize imageSize = height * width * 4;

		VkBuffer stagingImageBuffer;

		VkBufferCreateInfo stagingBufferCreateInfo{};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferCreateInfo.size = imageSize;
		stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const VmaAllocation bufferAllocation = VulkanAllocator::AllocateBuffer(&stagingBufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingImageBuffer);

		void* mappedMemory;
		VulkanAllocator::MapMemory(bufferAllocation, &mappedMemory);
		std::memcpy(mappedMemory, imageData, imageSize);
		VulkanAllocator::UnmapMemory(bufferAllocation);

		stbi_image_free(imageData);

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = static_cast<uint32_t>(width);
		imageCreateInfo.extent.height = static_cast<uint32_t>(height);
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;

		VK_ASSERT(vkCreateImage(device->GetLogicalDevice(), &imageCreateInfo, nullptr, &m_TextureImage));

		m_TextureAllocation = VulkanAllocator::AllocateImage(imageCreateInfo, VMA_MEMORY_USAGE_GPU_ONLY, m_TextureImage);


		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = 0;
		commandPoolInfo.queueFamilyIndex = device->GetQueueFamilyIndices().GraphicsFamily;

		VkCommandPool commandPool;
		VK_ASSERT(vkCreateCommandPool(device->GetLogicalDevice(), &commandPoolInfo, nullptr, &commandPool));

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandPool = commandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VK_ASSERT(vkAllocateCommandBuffers(device->GetLogicalDevice(), &commandBufferInfo, &commandBuffer));

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));


		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_TextureImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			static_cast<unsigned>(width),
			static_cast<unsigned>(height),
			1
		};
		vkCmdCopyBufferToImage(commandBuffer, stagingImageBuffer, m_TextureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VK_ASSERT(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		VK_ASSERT(vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
		VK_ASSERT(vkQueueWaitIdle(device->GetGraphicsQueue()));

		vkFreeCommandBuffers(device->GetLogicalDevice(), commandPool, 1, &commandBuffer);
		vkDestroyCommandPool(device->GetLogicalDevice(), commandPool, nullptr);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_TextureImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VK_ASSERT(vkCreateImageView(m_Device->GetLogicalDevice(), &viewInfo, nullptr, &m_TextureImageView));

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		vkCreateSampler(m_Device->GetLogicalDevice(), &samplerInfo, nullptr, &m_Sampler);

		VulkanAllocator::DestroyBuffer(stagingImageBuffer, bufferAllocation);
	}

	VulkanTexture::~VulkanTexture()
	{
		vkDestroySampler(m_Device->GetLogicalDevice(), m_Sampler, nullptr);
		vkDestroyImageView(m_Device->GetLogicalDevice(), m_TextureImageView, nullptr);
		vkDestroyImage(m_Device->GetLogicalDevice(), m_TextureImage, nullptr);
		VulkanAllocator::FreeAllocation(m_TextureAllocation);
	}


}