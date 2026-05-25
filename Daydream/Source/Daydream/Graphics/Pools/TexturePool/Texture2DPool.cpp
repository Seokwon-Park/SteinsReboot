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
	Texture2DPoolHandle Texture2DPool::RequestTexture2DHandle(UInt32 _width, UInt32 _height, RenderFormat _format)
	{
		Texture2DPoolKey key;
		key.width = _width;
		key.height = _height;
		key.format = _format;

		Texture2DPoolHandle handle{};
		//렌더타겟 풀에서 key에 해당하는 렌더타겟 큐에 할당할 렌더타겟이 있으면 할당, 없으면 생성해서 리턴
		if (!pool[key].empty())
		{
			handle = std::move(pool[key].front());
			pool[key].pop();
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
				handle.texture = Texture2D::Create(textureDesc);

				TextureViewDesc rtvDesc{};
				rtvDesc.type = TextureViewType::RenderTarget;
				rtvDesc.baseMip = 0;
				rtvDesc.mipLevels = 1;
				rtvDesc.baseLayer = 0;
				rtvDesc.layerCount = 1;
				handle.renderTargetView = TextureView::Create(handle.texture, rtvDesc);

				TextureViewDesc srvDesc{};
				srvDesc.type = TextureViewType::ShaderResource;
				srvDesc.baseMip = 0;
				srvDesc.mipLevels = 1;
				srvDesc.baseLayer = 0;
				srvDesc.layerCount = 1;
				handle.shaderResourceView = TextureView::Create(handle.texture, srvDesc);
			}
			else // depth 포멧인 경우
			{
				textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::DepthStencil;
				handle.texture = Texture2D::Create(textureDesc);

				TextureViewDesc dsvDesc{};
				dsvDesc.type = TextureViewType::DepthStencil;
				dsvDesc.baseMip = 0;
				dsvDesc.mipLevels = 1;
				dsvDesc.baseLayer = 0;
				dsvDesc.layerCount = 1;
				handle.depthStencilView = TextureView::Create(handle.texture, dsvDesc);

				TextureViewDesc srvDesc{};
				srvDesc.type = TextureViewType::ShaderResource;
				srvDesc.baseMip = 0;
				srvDesc.mipLevels = 1;
				srvDesc.baseLayer = 0;
				srvDesc.layerCount = 1;
				handle.shaderResourceView = TextureView::Create(handle.texture, srvDesc);
			}
		}
		return handle;
	}
	void Texture2DPool::ReturnTexture2DHandle(Texture2DPoolHandle&& _handle, UInt32 _lastUsedLoop)
	{
		_handle.lastUsedLoop = _lastUsedLoop;
		Texture2DPoolKey key;
		key.width = _handle.texture->GetWidth();
		key.height = _handle.texture->GetHeight();
		key.format = _handle.texture->GetFormat();

		pool[key].push(std::move(_handle));
	}

	void Texture2DPool::CleanUp(UInt64 _thresholdLoop)
	{
		for (auto& [key, handles] : pool)
		{
			while(!handles.empty() && handles.front().lastUsedLoop <= _thresholdLoop)
			{
				handles.pop();
			}
		}
	}
}

