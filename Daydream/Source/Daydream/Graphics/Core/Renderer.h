#pragma once

#include "RenderCommandList.h"
#include "RenderCommandQueue.h"
#include "RenderThread.h"

#include "Daydream/Graphics/Core/RenderDevice.h"
#include "Daydream/Graphics/Core/RenderContext.h"
#include "Daydream/Graphics/Camera/Camera.h"
#include "Daydream/Graphics/Resources/Skybox.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"
#include "Daydream/Graphics/Pools/TexturePool/Texture2DPool.h"
#include "Daydream/Graphics/Pools/BufferPool/UploadBufferPool.h"
#include "Daydream/Graphics/Pools/BufferPool/ConstantBufferPool.h"

namespace Daydream
{
	class Scene;
	class Renderer
	{
	public:
		static constexpr UInt32 MaxFramesInFlight = 3;
		static constexpr UInt32 MaxCommandListsInFlight = 3;


		template<typename RenderFunction>
		static void EnqueueCommand(RenderFunction&& _command)
		{
			if (useRenderThread)
			{
				// 멀티스레드
				commandQueues[recordingQueueIndex]->AddRenderCommand(std::forward<RenderFunction>(_command));
			}
			else
			{
				// 싱글스레드(디버깅)용 모드
				_command();
			}
		}

		// This Commands are executed before starting layerloop
		template<typename RenderFunction>
		static void EnqueuePreFrameCommand(RenderFunction&& _command)
		{
			singleTimeCommandQueue.push(_command);
		}

		static void Init(RendererAPIType _API);
		static void PostInit();
		static void Shutdown();

		static bool CreateSwapchainForWindow(DaydreamWindow& _window);
		static void OnSwapchainResize(Swapchain* _swapchain, UInt32 _width, UInt32 _height);

		static void SetRenderThreadEnabled(bool _enabled);
		static bool IsRenderThreadEnabled() { return useRenderThread; }

		static void BeginFrame(Swapchain* _swapchain);
		static void EndFrame(Swapchain* _swapchain);

		static void BeginRendering(const RenderingInfo& _renderingInfo);
		static void EndRendering(const RenderingInfo& _renderingInfo);
		static void BeginRendering(Swapchain* _swapchain, Color _clearColor);

		static void BindPipelineState(const GraphicsPipelineState* _pipelineState);

		//static void SetTexture2D(const String& _name, Shared<Texture2D> _texture);
		//static void SetTextureCube(const String& _name, Shared<TextureCube> _textureCube);
		static void BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _samplerState);
		static void BindConstantBuffer(const String& _name, const ConstantBuffer* _buffer);
		static void BindConstantBuffer(const String& _name, const Shared<ConstantBuffer>& _buffer);

		static void UpdateConstantBuffer(ConstantBuffer* _buffer, const void* _data, UInt32 _size)
		{
			std::vector<UInt8> dataCopy((const UInt8*)_data, (const UInt8*)_data + _size);

			EnqueueCommand([_buffer, data = std::move(dataCopy)]()
				{
					// 렌더 스레드에서 안전하게 복사된 데이터를 사용
					_buffer->UpdateData(data.data(), data.size());
				});
		}

		static void UpdateConstantBuffer(const Shared<ConstantBuffer>& _buffer, const void* _data, UInt32 _size)
		{
			UpdateConstantBuffer(_buffer.get(), _data, _size);
		}

		template <typename DataType>
		static void UpdateConstantBuffer(ConstantBuffer* _buffer, const DataType& _data)
		{
			EnqueueCommand([_buffer, _data]()
				{
					_buffer->UpdateData(&_data, sizeof(DataType));
				});
		}
		template <typename DataType>
		inline static void UpdateConstantBuffer(const Shared<ConstantBuffer>& _buffer, const DataType& _data)
		{
			UpdateConstantBuffer(_buffer.get(), _data);
		}

		static void BindMesh(const Mesh* _mesh);
		static void BindMaterial(const Material* _material);

		static void DrawIndexed(UInt32 _indexCount);

		static void CopyBuffer(const Buffer* _src, const Buffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset);
		static void CopyBufferToTexture2D(const Buffer* _src, const Texture2D* _dst);

		static void CopyTexture2D(const Texture2D* _src, const Texture2D* _dst);
		static void CopyTexture2DToTextureCube(const Texture2D* _srcTexture2D, const TextureCube* _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel = 0);
		static void CopyTextureCubeToTexture2D(const TextureCube* _srcCubemap, const Texture2D* _dstTexture2D, UInt32 _faceIndex, UInt32 _mipLevel = 0);

		static void TransitionTextureState(Texture* _texture,
			ResourceState _afterState,
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1,
			UInt32 _baseLayer = 0,
			UInt32 _layerCount = -1);

		static void TransitionTextureState(const Shared<Texture>& _texture,
			ResourceState _afterState,
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1,
			UInt32 _baseLayer = 0,
			UInt32 _layerCount = -1);

		static void TransitionBufferState(Buffer * _buffer, ResourceState _afterState);
		static void TransitionBufferState(const Shared<Buffer>& _buffer, ResourceState _afterState);

		static void GenerateMips(Texture* _texture);
		inline static void GenerateMips(const Shared<Texture>& _texture) { GenerateMips(_texture.get()); };

		static void ExecutePreFrameCommands();

		static void Submit();


		inline static UInt32 GetCurrentLoop() { return currentLoop; }

		inline static ImGuiRenderer* GetImGuiRenderer() { return imguiRenderer.get(); }

		//static Renderer& Get() { return *instance; }
		inline static RendererAPIType GetAPI() { return renderDevice->GetAPI(); }
		inline static RenderDevice* GetRenderDevice() { return renderDevice.get(); }
		inline static Skybox* GetSkybox() { return skybox.get(); }
		inline static RenderCommandList* GetActiveCommandList() { return renderContext->GetActiveCommandList(); }
		inline static Texture2DPool* GetTexturePool() { return texturePool.get(); }
		inline static UploadBufferPool* GetUploadBufferPool() { return uploadBufferPool.get(); }
		inline static ConstantBufferPool* GetConstantBufferPool() { return constantBufferPool.get(); }
	private:
		Renderer() = default;
		static void InitRenderDevice(RendererAPIType _API);

		inline static RenderContext* GetRenderContext() { return renderContext.get(); }

		inline static Unique<RenderDevice> renderDevice = nullptr;
		inline static Unique<RenderContext> renderContext = nullptr;
		inline static Unique<ImGuiRenderer> imguiRenderer = nullptr;
		inline static Unique<Skybox> skybox = nullptr;

		inline static Unique<Texture2DPool> texturePool;
		inline static Unique<UploadBufferPool> uploadBufferPool;
		inline static Unique<ConstantBufferPool> constantBufferPool;

		inline static Array<IResourcePool*> pools;
		////////////////////////////////////////////////////////////////// 
		// RenderThread
		////////////////////////////////////////////////////////////////// 
		inline static bool useRenderThread = 1;

		inline static Queue<RenderCommand> singleTimeCommandQueue;
		inline static Array<Unique<RenderCommandQueue>> commandQueues;
		//이 큐가 현재 렌더 스레드에서 사용중인지
		inline static std::array<std::atomic<bool>, MaxCommandListsInFlight> commandQueueBusyFlags;
		// 렌더스레드와 충돌 방지
		inline static std::mutex commandQueueStateMutex;
		inline static std::condition_variable commandQueueStateCV;

		inline static Unique<RenderThread> renderThread = nullptr;
		inline static UInt32 recordingQueueIndex = 0;
		//////////////////////////////////////////////////////////////////////////////////

		inline static UInt32 currentLoop = 0;


	};
}


