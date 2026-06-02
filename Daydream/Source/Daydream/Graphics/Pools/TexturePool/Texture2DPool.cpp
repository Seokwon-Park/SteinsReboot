#include "DaydreamPCH.h"
#include "Texture2DPool.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	Texture2DPool::Texture2DPool()
	{
	}

	Texture2DPool::~Texture2DPool()
	{
	}
	Texture2DAllocation Texture2DPool::AllocateTexture2DHandle(UInt32 _width, UInt32 _height, RenderFormat _format)
	{
		Texture2DPoolKey key;
		key.width = _width;
		key.height = _height;
		key.format = _format;

		Texture2DAllocation resource;

		//렌더타겟 풀에서 key에 해당하는 렌더타겟 큐에 할당할 렌더타겟이 있으면 할당, 없으면 생성해서 리턴
		if (!freeQueue[key].empty())
		{
			resource = std::move(freeQueue[key].front().payload); // PoolItem.Payload
			freeQueue[key].pop();
		}
		else
		{
			Texture2DDesc textureDesc{};
			textureDesc.width = _width;
			textureDesc.height = _height;
			textureDesc.mipLevels = 1;
			textureDesc.format = _format;

			if (!GraphicsUtility::IsDepthFormat(_format))
			{
				textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
				resource.texture = Texture2D::Create(textureDesc);

				TextureViewDesc rtvDesc{};
				rtvDesc.type = TextureViewType::RenderTarget;
				rtvDesc.baseMip = 0;
				rtvDesc.mipLevels = 1;
				rtvDesc.baseLayer = 0;
				rtvDesc.layerCount = 1;
				resource.renderTargetView = TextureView::Create(resource.texture, rtvDesc);

				TextureViewDesc srvDesc{};
				srvDesc.type = TextureViewType::ShaderResource;
				srvDesc.baseMip = 0;
				srvDesc.mipLevels = 1;
				srvDesc.baseLayer = 0;
				srvDesc.layerCount = 1;
				resource.shaderResourceView = TextureView::Create(resource.texture, srvDesc);
			}
			else // depth 포멧인 경우
			{
				textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::DepthStencil;
				resource.texture = Texture2D::Create(textureDesc);

				TextureViewDesc dsvDesc{};
				dsvDesc.type = TextureViewType::DepthStencil;
				dsvDesc.format = RenderFormat::D24_UNORM_S8_UINT;
				dsvDesc.baseMip = 0;
				dsvDesc.mipLevels = 1;
				dsvDesc.baseLayer = 0;
				dsvDesc.layerCount = 1;
				resource.depthStencilView = TextureView::Create(resource.texture, dsvDesc);

				TextureViewDesc srvDesc{};
				srvDesc.type = TextureViewType::ShaderResource;
				srvDesc.format = RenderFormat::R24_UNORM_X8_TYPELESS;
				srvDesc.baseMip = 0;
				srvDesc.mipLevels = 1;
				srvDesc.baseLayer = 0;
				srvDesc.layerCount = 1;
				resource.shaderResourceView = TextureView::Create(resource.texture, srvDesc);
			}
		}
		return resource;
	}
	Texture2DAllocation Texture2DPool::AllocateTexture2DHandle(const Texture2DPoolKey& _key)
	{
		return AllocateTexture2DHandle(_key.width, _key.height, _key.format);
	}
}

