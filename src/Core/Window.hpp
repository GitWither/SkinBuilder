#pragma once

#include "Core.hpp"

#include <string>

namespace SkinBuilder
{
	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void Initialize() = 0;
		virtual void PollEvents() = 0;
		virtual bool ShouldClose() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void* GetNative() const = 0;

		static Unique<Window> Create(const uint32_t width, const uint32_t height, const std::string& title);
	};
}