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
			UInt32 unalignedRowPitch = _desc.width * GraphicsUtility::GetRenderFormatSize(_desc.format);
			UInt32 alignedRowPitch = Renderer::GetRenderDevice()->GetAlignedRowPitch(_desc.width, _desc.format);
			UInt32 totalBytes = alignedRowPitch * _desc.height;

			Byte* alignedImageData = new Byte[totalBytes];
			const Byte* srcByte = (Byte*)_initialData;

			for (UInt32 row = 0; row < _desc.height; row++)
			{
				memcpy(alignedImageData + (row * alignedRowPitch), srcByte + (row * unalignedRowPitch), unalignedRowPitch);
			}

			Shared<UploadBuffer> uploadBuffer = Renderer::GetUploadBufferPool()->RequestBuffer(totalBytes);
			uploadBuffer->UpdateData(alignedImageData, totalBytes);

			delete[] alignedImageData;

			Renderer::EnqueuePreFrameCommand([=]()
				{
					Renderer::TransitionTextureState(texture2D, ResourceState::CopyDest, 0, 1, 0, -1);
					Renderer::CopyBufferToTexture2D(uploadBuffer.get(), texture2D.get());
					Renderer::TransitionTextureState(texture2D, ResourceState::ShaderResource, 0, 1, 0, -1);
				});

			Renderer::GetUploadBufferPool()->ReturnResource(totalBytes, std::move(uploadBuffer));
		}

		return texture2D;
	}
}

