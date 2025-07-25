#pragma once

#pragma warning(disable: 4819)
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <vector>
#include <string_view>
#include <set>
#include <unordered_map>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Steins/Core/Logger.h"
#include "Steins/Core/Assert.h"
#include "Steins/Core/Core.h"
#include "Steins/Enum/RendererEnums.h"
#include "Base/Base.h"




#if defined(STEINS_PLATFORM_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif // !NOMINMAX
	#include <Windows.h>
	#include <windowsx.h>

	#include <wrl.h> 
	#include <d3d11.h>
	#include <d3d12.h>
	#include <dxgi1_6.h>
	#include <dxgidebug.h>
	#include <d3dcompiler.h>
	#include <DirectXCollision.h>

	#pragma comment(lib, "d3d11")
	#pragma comment(lib, "d3d12")
	#pragma comment(lib, "d3dcompiler") 
	#pragma comment(lib, "DXGI") 
	#pragma comment(lib, "dxguid") 

	using namespace Microsoft::WRL;
#endif