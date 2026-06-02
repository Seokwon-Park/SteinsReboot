#include "DaydreamPCH.h"
#include "Daydream/Graphics/Core/RenderDevice.h"

#include "Daydream/Core/Window.h"

#include "Daydream/Graphics/Core/Swapchain.h"

#include "Daydream/Graphics/API/OpenGL/OpenGLRenderDevice.h"
#include "Daydream/Graphics/API/DirectX11/D3D11RenderDevice.h"
#include "Daydream/Graphics/API/DirectX12/D3D12RenderDevice.h"
#include "Daydream/Graphics/API/Vulkan/VulkanRenderDevice.h"

#include "Daydream/Graphics/Resources/Buffer.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	UInt32 RenderDevice::GetAlignedRowPitch(UInt32 _width, RenderFormat _format) const
	{
		return _width * GraphicsUtility::GetRenderFormatSize(_format);
	}
	Unique<RenderDevice> RenderDevice::Create(RendererAPIType _API)
	{
		switch (_API)
		{
		case RendererAPIType::None: return nullptr;
		case RendererAPIType::OpenGL: return MakeUnique<OpenGLRenderDevice>();
		case RendererAPIType::DirectX11: return MakeUnique<D3D11RenderDevice>();
		case RendererAPIType::DirectX12: return MakeUnique<D3D12RenderDevice>();
		case RendererAPIType::Vulkan: return MakeUnique<VulkanRenderDevice>();
		default: return nullptr;
		}
	}
}
