#pragma once

#include "Platform/Vulkan/VulkanIndexBuffer.hpp"
#include "Platform/Vulkan/VulkanVertexBuffer.hpp"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

namespace SkinBuilder
{
	class Mesh
	{
	public:
		Mesh(const Shared<VulkanVertexBuffer>& vertexBuffer, const Shared<VulkanIndexBuffer>& indexBuffer);
		~Mesh();

		static Shared<Mesh> LoadWavefront(const std::filesystem::path& path, const Shared<VulkanDevice>& device);

		const Shared<VulkanIndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
		const Shared<VulkanVertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
	private:
		Shared<VulkanIndexBuffer> m_IndexBuffer;
		Shared<VulkanVertexBuffer> m_VertexBuffer;
	};
}
