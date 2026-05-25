#include "DaydreamPCH.h"
#include "Texture2D.h"

#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"


namespace Daydream
{
	Texture2D::Texture2D(Shared<GPUTexture> _texture)
		:Texture(_texture)
	{

	}

	Shared<Texture2D> Texture2D::Create(const Texture2DDesc& _desc, const void* _initialData)
	{
		TextureDesc desc{};
		desc.width = _desc.width;
		desc.height = _desc.height;
		desc.layerCount = 1;
		desc.mipLevels = _desc.mipLevels;
		desc.sampleCount = _desc.sampleCount;
		desc.format = _desc.format;
		desc.textureUsage = _desc.textureUsage;
		desc.type = TextureType::Texture2D;

		Shared<GPUTexture> gpuTexture = Renderer::GetRenderDevice()->CreateGPUTexture(desc);
		Shared<Texture2D> texture2D = MakeShared<Texture2D>(gpuTexture);
		
		if (_initialData)
		{
			UInt32 imageSize = desc.width * desc.height * GraphicsUtility::GetRenderFormatSize(desc.format);
			Shared<Array<Byte>> dataArray = MakeShared<Array<Byte>>(imageSize);

			memcpy(dataArray->data(), _initialData, imageSize);

			Renderer::EnqueuePreFrameCommand([=]()
				{
					Renderer::TransitionTextureState(texture2D, ResourceState::Undefined, ResourceState::CopyDest, 0, 1);
					Renderer::CopyDataToTexture2D(texture2D, dataArray);
					Renderer::TransitionTextureState(texture2D, ResourceState::CopyDest, ResourceState::ShaderResource, 0, 1);
				});
		}

		return texture2D;
	}
}

