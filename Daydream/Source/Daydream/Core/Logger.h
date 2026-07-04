#pragma once

#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#define FMT_UNICODE 0

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Daydream
{
	class Logger
	{
	public:
		static void Init();
		static void Shutdown();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;
	};
	
}

// Core log macros
#define DAYDREAM_CORE_TRACE(...)		::Daydream::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define DAYDREAM_CORE_INFO(...)		::Daydream::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define DAYDREAM_CORE_WARN(...)		::Daydream::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define DAYDREAM_CORE_ERROR(...)		::Daydream::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define DAYDREAM_CORE_FATAL(...)		::Daydream::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define DAYDREAM_TRACE(...)			::Daydream::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define DAYDREAM_INFO(...)			::Daydream::Logger::GetClientLogger()->info(__VA_ARGS__)
#define DAYDREAM_WARN(...)			::Daydream::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define DAYDREAM_ERROR(...)			::Daydream::Logger::GetClientLogger()->error(__VA_ARGS__)
#define DAYDREAM_FATAL(...)			::Daydream::Logger::GetClientLogger()->critical(__VA_ARGS__)

#define DAYDREAM_RENDERER_TRACE(...)	DAYDREAM_CORE_TRACE("[Renderer] " ##__VA_ARGS__)		
#define DAYDREAM_RENDERER_INFO(...)		DAYDREAM_CORE_INFO("[Renderer] " ##__VA_ARGS__)		
#define DAYDREAM_RENDERER_WARN(...)		DAYDREAM_CORE_WARN("[Renderer] " ##__VA_ARGS__)		
#define DAYDREAM_RENDERER_ERROR(...)	DAYDREAM_CORE_ERROR("[Renderer] " ##__VA_ARGS__)		
#define DAYDREAM_RENDERER_FATAL(...)	DAYDREAM_CORE_FATAL("[Renderer] " ##__VA_ARGS__)		

#define DAYDREAM_ASSET_TRACE(...)		DAYDREAM_CORE_TRACE("[AssetManager] " ##__VA_ARGS__)
#define DAYDREAM_ASSET_INFO(...)		DAYDREAM_CORE_INFO("[AssetManager] " ##__VA_ARGS__)
#define DAYDREAM_ASSET_WARN(...)		DAYDREAM_CORE_WARN("[AssetManager] " ##__VA_ARGS__)
#define DAYDREAM_ASSET_ERROR(...)		DAYDREAM_CORE_ERROR("[AssetManager] " ##__VA_ARGS__)
#define DAYDREAM_ASSET_FATAL(...)		DAYDREAM_CORE_FATAL("[AssetManager] " ##__VA_ARGS__)

#define DAYDREAM_PHYSICS_INFO(...)		DAYDREAM_CORE_INFO("[Physics] " ##__VA_ARGS__)

