#pragma once

#include "Core/Memory.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace SkinBuilder
{
	class Logger
	{
	private:
		static Shared<spdlog::logger> s_Logger;
	public:
		static void Initialize();
		static Shared<spdlog::logger> GetLogger() { return s_Logger; }
	};

}


#define SB_INFO(...) ::SkinBuilder::Logger::GetLogger()->info(__VA_ARGS__)
#define SB_WARN(...) ::SkinBuilder::Logger::GetLogger()->warn(__VA_ARGS__)
#define SB_ERROR(...) ::SkinBuilder::Logger::GetLogger()->error(__VA_ARGS__)
#define SB_DEBUG(...) ::SkinBuilder::Logger::GetLogger()->debug(__VA_ARGS__)