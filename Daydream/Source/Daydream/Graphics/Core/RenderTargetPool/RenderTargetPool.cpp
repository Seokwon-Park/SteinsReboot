#include "DaydreamPCH.h"
#include "RenderTargetPool.h"

namespace Daydream
{
	RenderTargetPool::RenderTargetPool()
	{
	}

	RenderTargetPool::~RenderTargetPool()
	{
	}
	RenderTargetPoolHandle RenderTargetPool::RequestRenderTargetView(UInt32 _width, UInt32 _height, RenderFormat _format)
	{
		RenderTargetPoolKey key;
		key.width = _width;
		key.height = _height;
		key.format = _format;

		RenderTargetPoolHandle handle{};
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
			textureDesc.textureUsage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
			textureDesc.format = _format;
			handle.texture = Texture2D::Create(textureDesc);

			TextureViewDesc rtvDesc{};
			rtvDesc.type = TextureViewType::RenderTarget;
			rtvDesc.baseMip = 0;
			rtvDesc.mipLevels = 1;
			rtvDesc.baseLayer = 0;
			rtvDesc.layerCount = 1;
			handle.renderTargetView = TextureView::Create(handle.texture, rtvDesc);

			TextureViewDesc srvDesc{};
			rtvDesc.type = TextureViewType::ShaderResource;
			rtvDesc.baseMip = 0;
			rtvDesc.mipLevels = 1;
			rtvDesc.baseLayer = 0;
			rtvDesc.layerCount = 1;
			handle.shaderResourceView = TextureView::Create(handle.texture, srvDesc);
		}
		return handle;
	}
	void RenderTargetPool::ReleaseRenderTarget(RenderTargetPoolHandle&& _renderTargetHandle, UInt32 _lastUsedLoop)
	{
		_renderTargetHandle.lastUsedLoop = _lastUsedLoop;
		RenderTargetPoolKey key;
		key.width = _renderTargetHandle.texture->GetWidth();
		key.height = _renderTargetHandle.texture->GetHeight();
		key.format = _renderTargetHandle.texture->GetFormat();

		pool[key].push(std::move(_renderTargetHandle));
	}

	void RenderTargetPool::UpdatePool(UInt64 _thresholdLoop)
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

