#pragma once


#include <cstdint>
#include <initializer_list>
#include <array>

namespace SkinBuilder
{
	enum class DataType
	{
		Float,

		Vector2,
		Vector3,
		Vector4,

		Matrix3,
		Matrix4
	};

	constexpr uint32_t GetDataTypeSize(DataType type)
	{
		switch (type)
		{
			case DataType::Float: return sizeof(float);

			case DataType::Vector2: return sizeof(float) * 2;
			case DataType::Vector3: return sizeof(float) * 3;
			case DataType::Vector4: return sizeof(float) * 4;

			case DataType::Matrix3: return sizeof(float) * 3 * 3;
			case DataType::Matrix4: return sizeof(float) * 4 * 4;
		}

		return 0;
	}


	struct VertexAttribute
	{
		uint32_t Location;
		DataType Type;
		uint32_t Offset;
	};


	struct VertexBufferLayout
	{
	private:
		uint32_t m_Stride = 0;
		std::vector<VertexAttribute> m_Attributes;
	public:
		VertexBufferLayout() = default;

		constexpr VertexBufferLayout(const std::initializer_list<VertexAttribute>& list) : m_Attributes(list)
		{
			for (const auto& attr : list)
			{
				m_Stride += GetDataTypeSize(attr.Type);
			}
		}

		constexpr const std::vector<VertexAttribute>& GetAttributes() const
		{
			return m_Attributes;
		}

		constexpr const uint32_t GetStride() const
		{
			return m_Stride;
		}
	};
}