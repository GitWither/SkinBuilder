#include "Graphics/Mesh.hpp"

#include "Vertex.hpp"

namespace SkinBuilder
{
	Shared<Mesh> Mesh::LoadWavefront(const std::filesystem::path& path, const Shared<VulkanDevice>& device)
	{
		const aiScene* scene = aiImportFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_OptimizeMeshes);

		SB_ASSERT(scene, "Model scene not loaded");
		SB_ASSERT(scene->HasMeshes(), "Loaded scene doesn't have meshes");

		//We don't care about submeshes/etc as minecraft player models are extremely trivial
		const aiMesh* mesh = scene->mMeshes[0];

		std::vector<uint32_t> triangleIndices;
		std::vector<Vertex> vertices;

		triangleIndices.reserve(mesh->mNumFaces * 3);
		vertices.reserve(mesh->mNumVertices);

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			const aiVector3D vertex = mesh->mVertices[i];

			const float u = mesh->mTextureCoords[0][i].x;
			const float v = mesh->mTextureCoords[0][i].y;

			vertices.emplace_back(glm::vec3(vertex.x, vertex.y, vertex.z), glm::vec2(u, v));
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			triangleIndices.emplace_back(mesh->mFaces[i].mIndices[0]);
			triangleIndices.emplace_back(mesh->mFaces[i].mIndices[1]);
			triangleIndices.emplace_back(mesh->mFaces[i].mIndices[2]);
		}

		aiReleaseImport(scene);

		return MakeShared<Mesh>(
			MakeShared<VulkanVertexBuffer>(vertices.size() * sizeof(Vertex), vertices.data(), device), 
			MakeShared<VulkanIndexBuffer>(triangleIndices.size(), triangleIndices.data(), device)
			);
	}

	Mesh::Mesh(const Shared<VulkanVertexBuffer>& vertexBuffer, const Shared<VulkanIndexBuffer>& indexBuffer) :
		m_IndexBuffer(indexBuffer),
		m_VertexBuffer(vertexBuffer)
	{
		
	}

	Mesh::~Mesh()
	{
		m_IndexBuffer = nullptr;
		m_VertexBuffer = nullptr;
	}

}
