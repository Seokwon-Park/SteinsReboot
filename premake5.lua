workspace "Daydream"
	architecture "x64"
	startproject "Sandbox"
	
	configurations
	{
		"Debug",
		"Release",
	}

	IncludeDir = {}
	IncludeDir["assimp"] = "Daydream/Vendor/assimp/include"
	IncludeDir["GLFW"] = "Daydream/Vendor/glfw/include"
	IncludeDir["glad"] = "Daydream/Vendor/glad/include"
	IncludeDir["glm"] = "Daydream/Vendor/glm"
	IncludeDir["DaydreamMath"] = "Daydream/Vendor/DaydreamMath"
	IncludeDir["ImGui"] = "Daydream/Vendor/imgui"
	IncludeDir["stb"] = "Daydream/Vendor/stb"
	IncludeDir["dxcompiler"] = "Daydream/Vendor/dxcompiler/include"
	IncludeDir["VulkanMemoryAllocator"] = "Daydream/Vendor/VulkanMemoryAllocator"
	IncludeDir["D3D12MemoryAllocator"] = "Daydream/Vendor/D3D12MemoryAllocator"
	IncludeDir["ImGuizmo"] = "Daydream/Vendor/ImGuizmo"
	IncludeDir["stduuid"] = "Daydream/Vendor/stduuid/include"
	IncludeDir["yaml"] = "Daydream/Vendor/yaml-cpp/include"
	IncludeDir["nfd"] = "Daydream/Vendor/nfd-extended/src/include"

	include "Daydream/Vendor/glfw"
	include "Daydream/Vendor/glad"
	include "Daydream/Vendor/imgui"
	include "Daydream/Vendor/yaml-cpp"
	include "Daydream/Vendor/nfd-extended"
		
project "Daydream"
	location "Daydream"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("Build/Bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("Build/Intermediate/%{cfg.buildcfg}/%{prj.name}")

	pchheader "DaydreamPCH.h"
	pchsource "%{prj.name}/Source/DaydreamPCH.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Vendor/stb/**.h",
		"%{prj.name}/Vendor/stb/**.cpp",
		"%{prj.name}/Vendor/glm/glm/**.hpp",
 		"%{prj.name}/Vendor/glm/glm/**.inl",
 		"%{prj.name}/Vendor/DaydreamMath/DaydreamMath/**.h",
 		"%{prj.name}/Vendor/VulkanMemoryAllocator/**.cpp",
 		"%{prj.name}/Vendor/D3D12MemoryAllocator/**.cpp",
 		"%{prj.name}/Vendor/ImGuizmo/ImGuizmo.h",
 		"%{prj.name}/Vendor/ImGuizmo/ImGuizmo.cpp",
	}
	includedirs
	{
		"%{prj.name}/Source",
		"%{prj.name}/Vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.DaydreamMath}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.dxcompiler}",
		"%{IncludeDir.VulkanMemoryAllocator}",
		"%{IncludeDir.D3D12MemoryAllocator}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stduuid}",
		"%{IncludeDir.yaml}",
		"%{IncludeDir.nfd}",
		"$(VULKAN_SDK)/Include",
	}
	libdirs 
	{
        "$(VULKAN_SDK)/Lib",
		"Daydream/Vendor/assimp/bin/%{cfg.buildcfg}",
		"Daydream/Vendor/dxcompiler",
    }
	links
	{
		"GLFW",
		"glad",
		"ImGui",
		"yaml-cpp",
		"nfd-extended",
		"vulkan-1.lib",
		"dxcompiler.lib",
		"dxil.lib",
		"opengl32.lib",
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE",
		"GLM_FORCE_LEFT_HANDED",
		"GLM_FORCE_DEPTH_ZERO_TO_ONE"
	}

	filter "files:Daydream/Vendor/ImGuizmo/**.cpp"
		flags {"NoPCH"}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"
		defines
		{
			"DAYDREAM_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}
		flags { "MultiProcessorCompile" }


	filter "configurations:Debug"
		defines "DAYDREAM_DEBUG"
		symbols "On"
		runtime "Debug"
		links 
		{
			"spirv-cross-cd.lib",
			"spirv-cross-cored.lib",
			"spirv-cross-cppd.lib",
			"spirv-cross-glsld.lib",
			"spirv-cross-hlsld.lib",
			"spirv-cross-msld.lib",
			"spirv-cross-reflectd.lib",
			"spirv-cross-utild.lib",
            "assimp-vc143-mtd.lib" -- Assimp Debug 버전 라이브러리 이름 (필요 시 주석 해제)
        }
	filter "configurations:Release"
		defines "DAYDREAM_RELEASE"
		symbols "On"
		runtime "Release"
		links 
		{
			"spirv-cross-c.lib",
			"spirv-cross-core.lib",
			"spirv-cross-cpp.lib",
			"spirv-cross-glsl.lib",
			"spirv-cross-hlsl.lib",
			"spirv-cross-msl.lib",
			"spirv-cross-reflect.lib",
			"spirv-cross-util.lib",
            "assimp-vc143-mt.lib" -- Assimp Release 버전 라이브러리 이름 (필요 시 주석 해제)
        }
	
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("Build/Bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("Build/Intermediate/%{cfg.buildcfg}/%{prj.name}")
	postbuildcommands { "{COPYFILE} %[Daydream/Vendor/assimp/bin/%{cfg.buildcfg}] %[Build/Bin/%{cfg.buildcfg}/%{prj.name}]" }
	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}
	includedirs
	{
		"Daydream/Vendor/spdlog/include",
		"Daydream/Source",
		"Daydream/Vendor",
		"%{IncludeDir.DaydreamMath}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stduuid}",
	}
	links
	{
		"Daydream",
	}
	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"
		defines
		{
			"DAYDREAM_PLATFORM_WINDOWS",
		}
		flags { "MultiProcessorCompile" }
	filter "configurations:Debug"
		defines "DAYDREAM_DEBUG"
		symbols "On"
		runtime "Debug"
	filter "configurations:Release"
		defines "DAYDREAM_RELEASE"
		symbols "On"
		runtime "Release"

project "DaydreamEditor"
	location "DaydreamEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("Build/Bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("Build/Intermediate/%{cfg.buildcfg}/%{prj.name}")
	postbuildcommands { "{COPYFILE} %[Daydream/Vendor/assimp/bin/%{cfg.buildcfg}] %[Build/Bin/%{cfg.buildcfg}/%{prj.name}]" }

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}
	includedirs
	{
		"Daydream/Vendor/spdlog/include",
		"Daydream/Source",
		"Daydream/Vendor",
		"%{IncludeDir.DaydreamMath}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.stduuid}",
	}
	links
	{
		"Daydream",
	}
	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"
		defines
		{
			"DAYDREAM_PLATFORM_WINDOWS",
		}
		flags { "MultiProcessorCompile" }
	filter "configurations:Debug"
		defines "DAYDREAM_DEBUG"
		symbols "On"
		runtime "Debug"
	filter "configurations:Release"
		defines "DAYDREAM_RELEASE"
		symbols "On"
		runtime "Release"
