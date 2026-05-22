#pragma once

#include "RenderCommandList.h"
#include "RenderCommandQueue.h"
#include "RenderThread.h"

#include "Daydream/Graphics/Core/RenderDevice.h"
#include "Daydream/Graphics/Core/RenderContext.h"
#include "Daydream/Graphics/Camera/Camera.h"
#include "Daydream/Graphics/Resources/Skybox.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"
#include "Daydream/Graphics/Core/RenderTargetPool/RenderTargetPool.h"

namespace Daydream
{
	struct ResourceCapturedData
	{
		UInt64 capturedLoop;            
		Array<Shared<GPUResource>> resources; 
	};

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

		template<typename RenderFunction>
		static void EnqueuePreFrameCommand(RenderFunction&& _command)
		{
			singleTimeCommandQueue.push(_command);
		}

		static void Init(RendererAPIType _API);
		static void PostInit();
		static void Shutdown();

		static bool CreateSwapchainForWindow(DaydreamWindow& _window);
		static void OnSwapchainResize(const Shared<Swapchain>& _swapchain, UInt32 _width, UInt32 _height);

		static void SetRenderThreadEnabled(bool _enabled);
		static bool IsRenderThreadEnabled() { return useRenderThread; }

		static void BeginFrame(const Shared<Swapchain>& _swapchain);
		static void EndFrame(const Shared<Swapchain>& _swapchain);

		static void BeginRendering(const RenderingInfo& _renderingInfo);
		static void EndRendering(const RenderingInfo& _renderingInfo);
		static void BeginRendering(const Shared<Swapchain>& _swapchain, Color _clearColor);
		//static void EndRendering(Swapchain* _swapchain);
		//static void BeginRenderPass(const Shared<RenderPass>& _renderPass, const Shared<Framebuffer>& _framebuffer);
		//static void EndRenderPass(const Shared<RenderPass>& _renderPass);

		//static void BeginSwapchainRenderPass(Swapchain* _swapchain);
		//static void EndSwapchainRenderPass(Swapchain* _swapchain);

		static void BindPipelineState(const Shared<GraphicsPipelineState>& _pipelineState);

		//static void SetTexture2D(const String& _name, Shared<Texture2D> _texture);
		//static void SetTextureCube(const String& _name, Shared<TextureCube> _textureCube);
		static void BindShaderResourceView(const String& _name, const Shared<TextureView>& _textureView, Shared<Sampler> _samplerState);
		static void BindConstantBuffer(const String& _name, const Shared<ConstantBuffer>& _buffer);


		template <typename DataType>
		static void UpdateConstantBuffer(const Shared<ConstantBuffer>& _buffer, const DataType& _data)
		{
			EnqueueCommand([_buffer, _data]()
				{
					_buffer->UpdateData(&_data, sizeof(DataType));
				});
		}

		static void BindMesh(const Shared<Mesh>& _mesh);
		static void BindMaterial(const Shared<Material>& _material);

		static void DrawIndexed(UInt32 _indexCount);

		//static void RequestResizeFramebuffer(const Shared<Framebuffer>& _framebuffer, UInt32 _width, UInt32 _height);

		static void CopyBuffer(const Shared<GPUBuffer>& _src, const Shared<GPUBuffer>& _dst, UInt32 _copySize);
		static void CopyBufferToTexture(const Shared<GPUBuffer>& _src, const Shared<GPUTexture>& _dst);
		static void CopyDataToTexture2D(const Shared<Texture2D>& _target, const Shared<Array<Byte>>& _data);

		static void CopyTexture2D(const Shared<Texture2D>& _src, const Shared<Texture2D>& _dst);
		static void CopyTexture2DToTextureCube(const Shared<Texture2D>& _srcTexture2D, const Shared<TextureCube>& _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel = 0);
		static void CopyTextureCubeToTexture2D(const Shared<TextureCube>& _srcCubemap, const Shared<Texture2D>& _dstTexture2D, UInt32 _faceIndex, UInt32 _mipLevel = 0);


		static void TransitionTextureState(const Shared<GPUTexture>& _texture,
			ResourceState _beforeState,
			ResourceState _afterState,
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1,
			UInt32 _baseLayer = 0,
			UInt32 _layerCount = -1);

		static void TransitionTextureState(const Shared<Texture>& _texture,
			ResourceState _beforeState,
			ResourceState _afterState,
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1,
			UInt32 _baseLayer = 0,
			UInt32 _layerCount = -1);


		static void TransitionBufferState(
			const Shared<GPUBuffer>& _buffer,
			ResourceState _beforeState,
			ResourceState _afterState
		);

		static void GenerateMips(const Shared<Texture>& _texture);

		static void ExecutePreFrameCommands();

		static void Submit();

		inline static void CaptureResource(Array<Shared<GPUResource>> _resources)
		{
			capturedResourcesQueue.push({ currentLoop, std::move(_resources) });
		}

		inline static ImGuiRenderer* GetImGuiRenderer() { return imguiRenderer.get(); }

		//static Renderer& Get() { return *instance; }
		inline static RendererAPIType GetAPI() { return renderDevice->GetAPI(); }
		inline static RenderDevice* GetRenderDevice() { return renderDevice.get(); }
		inline static RenderContext* GetRenderContext() { return renderContext.get(); }
		inline static Skybox* GetSkybox() { return skybox.get(); }
		inline static RenderCommandList* GetActiveCommandList() { return renderContext->GetActiveCommandList().get(); }
		inline static RenderTargetPool* GetRenderTargetPool() { return renderTargetPool.get(); }
	private:
		Renderer() = default;
		static void InitRenderDevice(Daydream::RendererAPIType _API);

		inline static Unique<RenderDevice> renderDevice = nullptr;
		inline static Unique<RenderContext> renderContext = nullptr;
		inline static Unique<ImGuiRenderer> imguiRenderer = nullptr;
		inline static Unique<Skybox> skybox = nullptr;

		inline static Queue<ResourceCapturedData> capturedResourcesQueue;
		inline static Unique<RenderTargetPool> renderTargetPool;

		/////////////////////////////////  RenderThread  ///////////////////////////////// 
		inline static bool useRenderThread = 0;

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


