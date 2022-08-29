#pragma once

#include "Core.hpp"
#include "Window.hpp"
#include "Graphics/Camera.hpp"

namespace SkinBuilder {

	class SkinBuilder
	{
	private:
		Unique<Window> m_Window;
		bool m_Running = false;
		Camera m_Camera;
	public:
		SkinBuilder(const uint32_t width, const uint32_t height, const std::string& title);

		Window* GetWindow() const;

		static SkinBuilder& GetInstance();

		void Run();
	};



}
