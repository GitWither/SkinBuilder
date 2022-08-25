#include <Core/Logger.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace SkinBuilder
{
	Shared<spdlog::logger> Logger::s_Logger;

	void Logger::Initialize()
	{
		std::array sinks = {
			MakeShared<spdlog::sinks::stdout_color_sink_mt>()
		}; 

		sinks[0]->set_pattern("%^[%T] %n: %v%$");

		s_Logger = MakeShared<spdlog::logger>("SKINBUILDER", sinks.begin(), sinks.end());
		spdlog::register_logger(s_Logger);
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
	}

}