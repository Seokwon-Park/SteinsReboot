#include "DaydreamPCH.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Renderer2D.h"

#include <Daydream/Scene/Components/ModelRendererComponent.h>
#include "Daydream/Scene/Scene.h"
#include "Daydream/Graphics/Utility/ModelLoader.h"
#include "Daydream/Graphics/Utility/ShaderCompileHelper.h"

#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Resources/Skybox.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"


namespace Daydream
{
	//Renderer* Renderer::instance = nullptr;

	void Renderer::Init(RendererAPIType _API)
	{
		InitRenderDevice(_API);

		renderContext = renderDevice->CreateContext();

		imguiRenderer = renderDevice->CreateImGuiRenderer();

		texturePool = MakeUnique<Texture2DPool>();

		commandQueues.resize(MaxCommandListsInFlight);
		for (auto& commandList : commandQueues)
		{
			commandList = MakeUnique<RenderCommandQueue>();
		}

		// memory_order_relaxed는 동기화 순서가 중요하지 않은 단순 초기화에 사용
		for (auto& queueBusy : commandQueueBusyFlags)
		{
			queueBusy.store(false, std::memory_order_relaxed);
		}

		recordingQueueIndex = 0;

		renderThread = MakeUnique<RenderThread>();
		if (useRenderThread)
		{
			renderThread->Start();
		}

		ShaderCompileHelper::Init();
		/*	RenderCommand::Init(renderDevice.get());*/
	}

	void Renderer::InitRenderDevice(Daydream::RendererAPIType _API)
	{
		renderDevice = RenderDevice::Create(_API);
		DAYDREAM_CORE_ASSERT(renderDevice, "Failed to create graphics device!");
		renderDevice->Init();

		const RendererAPIInfo& info = renderDevice->GetAPIInfo();
		DAYDREAM_RENDERER_INFO("{}", info.APIName);
		DAYDREAM_RENDERER_INFO("  Vendor: {0}", info.vendor);
		DAYDREAM_RENDERER_INFO("  GPU Info: {0}", info.physicalDeviceInfo);
		DAYDREAM_RENDERER_INFO("  Version: {0}", info.version);
	}

	void Renderer::PostInit()
	{
		skybox = MakeUnique<Skybox>();
		skybox->CreateResources();
		EnqueuePreFrameCommand([]() {skybox->GenerateDefault(); });
	}

	void Renderer::Shutdown()
	{
		//Renderer2D::Shutdown();
		ShaderCompileHelper::Shutdown();
		if (useRenderThread)
		{
			renderThread->Stop();
			renderThread = nullptr;
		}
		skybox = nullptr;
		imguiRenderer->Shutdown();
		renderContext.reset();
		renderDevice.reset();
		//delete instance;
		//instance = nullptr;
	}

	bool Renderer::CreateSwapchainForWindow(DaydreamWindow& _window)
	{
		SwapchainDesc desc;
		desc.width = _window.GetWidth();
		desc.height = _window.GetHeight();
		desc.imageCount = Renderer::MaxFramesInFlight;
		desc.format = RenderFormat::R8G8B8A8_UNORM;
		desc.isFullscreen = false;
		desc.isVSync = _window.IsVSync();

		Shared<Swapchain> swapchain = renderDevice->CreateSwapchain(_window, desc);
		if (swapchain == nullptr) return false;
		_window.SetSwapchain(swapchain);
		return true;
	}

	void Renderer::OnSwapchainResize(Swapchain* _swapchain, UInt32 _width, UInt32 _height)
	{
		//renderContext->SetViewport(0, 0, _width, _height);
		_swapchain->ResizeSwapchain(_width, _height);
	}

	void Renderer::SetRenderThreadEnabled(bool _enabled)
	{
		useRenderThread = _enabled;
		if (!renderThread)
		{
			return;
		}

		if (useRenderThread)
		{
			renderThread->Start();
		}
		else
		{
			renderThread->Stop();
		}
	}

	void Renderer::BeginFrame(Swapchain* _swapchain)
	{
		EnqueueCommand([_swapchain]()
			{
				_swapchain->BeginFrame();
				renderContext->SetActiveCommandList(_swapchain->GetCurrentCommandList());
				//clear CapturedBuffer Safe
				//renderContext->ReleaseCapturedBuffer();

			});
	}

	void Renderer::EndFrame(Swapchain* _swapchain)
	{
		EnqueueCommand([_swapchain]()
			{
				_swapchain->EndFrame();
				_swapchain->Present();
			});
	}

	void Renderer::BeginRendering(const RenderingInfo& _renderingInfo)
	{
		EnqueueCommand([_renderingInfo]()
			{
				renderContext->BeginRendering(_renderingInfo);
			});
	}

	void Renderer::EndRendering(const RenderingInfo& _renderingInfo)
	{
		EnqueueCommand([_renderingInfo]()
			{
				renderContext->EndRendering(_renderingInfo);
			});
	}

	void Daydream::Renderer::BeginRendering(Swapchain* _swapchain, Color _clearColor)
	{
		EnqueueCommand([_swapchain, _clearColor]()
			{
				RenderingInfo renderingInfo{};
				renderingInfo.renderArea.width = _swapchain->GetWidth();
				renderingInfo.renderArea.height = _swapchain->GetHeight();

				AttachmentDesc desc;
				desc.view = _swapchain->GetCurrentRenderTargetView();
				desc.clearValue.colorClearValue = _clearColor;
				renderingInfo.colorAttachments.push_back(desc);

				renderContext->BeginRendering(renderingInfo);
			});
	}

	//void Renderer::BeginRenderPass(const RenderPass>& _renderPass, const Framebuffer>& _framebuffer)
	//{
	//	EnqueueCommand([_renderPass, _framebuffer]()
	//		{
	//			renderContext->BeginRenderPass(_renderPass, _framebuffer);
	//		});
	//}
	//void Renderer::EndRenderPass(const RenderPass>& _renderPass)
	//{
	//	EnqueueCommand([_renderPass]()
	//		{
	//			renderContext->EndRenderPass(_renderPass);
	//		});
	//}

	//void Renderer::BeginSwapchainRenderPass(Swapchain* _swapchain)
	//{
	//	EnqueueCommand([_swapchain]()
	//		{
	//			renderContext->BeginRenderPass(_swapchain->GetRenderPass(), _swapchain->GetCurrentFramebuffer());
	//		});
	//}

	//void Renderer::EndSwapchainRenderPass(Swapchain* _swapchain)
	//{
	//	EnqueueCommand([_swapchain]()
	//		{
	//			renderContext->EndRenderPass(_swapchain->GetRenderPass());
	//		});
	//}

	void Renderer::BindPipelineState(const GraphicsPipelineState* _pipelineState)
	{
		EnqueueCommand([_pipelineState]()
			{
				renderContext->BindPipelineState(_pipelineState);
			});
	}



	//void Renderer::SetTexture2D(const String& _name, const Texture2D> _texture)
	//{
	//	EnqueueCommand([_name, _texture]()
	//		{
	//			renderContext->SetTexture2D(_name, _texture);
	//		});
	//}
	//void Renderer::SetTextureCube(const String& _name, const TextureCube> _textureCube)
	//{
	//	EnqueueCommand([_name, _textureCube]()
	//		{
	//			renderContext->SetTextureCube(_name, _textureCube);
	//		});
	//}
	void Renderer::BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _samplerState)
	{
		EnqueueCommand([_name, _textureView, _samplerState]()
			{
				renderContext->BindShaderResourceView(_name, _textureView, _samplerState);
			});
	}

	void Renderer::BindConstantBuffer(const String& _name, const ConstantBuffer* _buffer)
	{
		EnqueueCommand([_name, _buffer]()
			{
				renderContext->BindConstantBuffer(_name, _buffer->GetGPUBuffer());
			});
	}

	void Renderer::BindConstantBuffer(const String& _name, const Shared<ConstantBuffer>& _buffer)
	{
		BindConstantBuffer(_name, _buffer.get());
	}

	void Renderer::BindMesh(const Mesh* _mesh)
	{
		EnqueueCommand([_mesh]()
			{
				renderContext->BindVertexBuffer(_mesh->GetVertexBuffer()->GetGPUBuffer(), _mesh->GetVertexBuffer()->GetStride());
				renderContext->BindIndexBuffer(_mesh->GetIndexBuffer()->GetGPUBuffer());
			});
	}

	void Renderer::BindMaterial(const Material* _material)
	{
		EnqueueCommand([_material]()
			{
				const auto& textureInfo = _material->GetTextureBindings();
				for (const auto& [name, texture] : textureInfo)
				{
					renderContext->BindShaderResourceView(name, texture.cache->GetOrCreateDefaultSRV(), BuiltIn::Samplers::LinearRepeat());
				}
			});
	}

	void Renderer::DrawIndexed(UInt32 _indexCount)
	{
		EnqueueCommand([_indexCount]()
			{
				renderContext->DrawIndexed(_indexCount);
			});
	}

	//void Renderer::RequestResizeFramebuffer(const const Framebuffer>& _framebuffer, UInt32 _width, UInt32 _height)
	//{
	//	EnqueuePreFrameCommand([_framebuffer, _width, _height]()
	//		{
	//			EnqueueCommand([_framebuffer, _width, _height]()
	//				{
	//					//_framebuffer->Recreate(_width, _height);
	//					DAYDREAM_RENDERER_INFO("Framebuffer is Recreated Width : {}, Height {}", _width, _height);
	//				}
	//			);
	//		});
	//}

	void Renderer::CopyBuffer(const Buffer* _src, const Buffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset)
	{

		EnqueueCommand([_src, _dst, _copySize, _srcOffset, _dstOffset]()
			{
				renderContext->CopyBuffer(_src->GetGPUBuffer(), _dst->GetGPUBuffer(), _copySize, _srcOffset, _dstOffset);
			});
	}

	void Renderer::CopyDataToTexture2D(const Texture2D* _dst, const void* _data)
	{
		EnqueueCommand([_dst, _data]()
			{
				renderContext->CopyDataToTexture(_dst->GetGPUTexture(), _data);
			});
	}


	void Renderer::CopyTexture2D(const Texture2D* _src, const Texture2D* _dst)
	{
		EnqueueCommand([_src, _dst]()
			{
				TextureCopyRegion region = {};
				region.extent[0] = _src->GetWidth();
				region.extent[1] = _src->GetHeight();
				region.extent[2] = 1; // 2D니까 깊이는 1

				region.srcSubresource.layerCount = 1; // 2D니까 레이어도 1
				renderContext->CopyTexture(_src->GetGPUTexture(), _dst->GetGPUTexture(), region);
			});
	}

	void Renderer::CopyTexture2DToTextureCube(const Texture2D* _srcTexture2D, const TextureCube* _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel)
	{
		EnqueueCommand([_dstCubemap, _faceIndex, _srcTexture2D, _mipLevel]()
			{
				TextureCopyRegion region = {};
				region.extent[0] = _srcTexture2D->GetWidth();
				region.extent[1] = _srcTexture2D->GetHeight();
				region.extent[2] = 1;

				region.srcSubresource.mipLevel = 0;
				region.srcSubresource.baseLayer = 0;
				region.srcSubresource.layerCount = 1;

				region.dstSubresource.mipLevel = _mipLevel;   
				region.dstSubresource.baseLayer = _faceIndex;
				region.dstSubresource.layerCount = 1;

				renderContext->CopyTexture(_srcTexture2D->GetGPUTexture(), _dstCubemap->GetGPUTexture(), region);
			});
	}

	void Renderer::CopyTextureCubeToTexture2D(const TextureCube* _srcCubemap, const Texture2D* _dstTexture2D, UInt32 _faceIndex, UInt32 _mipLevel)
	{
		EnqueueCommand([_srcCubemap, _faceIndex, _dstTexture2D, _mipLevel]()
			{
				TextureCopyRegion region = {};
				region.extent[0] = _srcCubemap->GetWidth();
				region.extent[1] = _srcCubemap->GetHeight();
				region.extent[2] = 1;

				region.srcSubresource.mipLevel = _mipLevel;
				region.srcSubresource.baseLayer = _faceIndex;
				region.srcSubresource.layerCount = 1;

				region.dstSubresource.mipLevel = 0;
				region.dstSubresource.baseLayer = 0;
				region.dstSubresource.layerCount = 1;

				renderContext->CopyTexture(_srcCubemap->GetGPUTexture(), _dstTexture2D->GetGPUTexture(), region);
			});
	}

	void Renderer::TransitionTextureState(const Texture* _texture, ResourceState _beforeState, ResourceState _afterState, UInt32 _baseMip, UInt32 _mipLevels, UInt32 _baseLayer, UInt32 _layerCount)
	{
		EnqueueCommand([_texture, _beforeState, _afterState, _baseMip, _mipLevels, _baseLayer, _layerCount]()
			{
				renderContext->TransitionTextureState(_texture->GetGPUTexture(), _beforeState, _afterState, _baseMip, _mipLevels, _baseLayer, _layerCount);
			});
	}

	void Renderer::TransitionTextureState(const Shared<Texture>& _texture, ResourceState _beforeState, ResourceState _afterState, UInt32 _baseMip, UInt32 _mipLevels, UInt32 _baseLayer, UInt32 _layerCount)
	{
		TransitionTextureState(_texture.get(), _beforeState, _afterState, _baseMip, _mipLevels, _baseLayer, _layerCount);
	}


	void Renderer::TransitionBufferState(const GPUBuffer* _buffer, ResourceState _beforeState, ResourceState _afterState)
	{
		EnqueueCommand([_buffer, _beforeState, _afterState]()
			{
				renderContext->TransitionBufferState(_buffer, _beforeState, _afterState);
			});
	}

	void Renderer::TransitionBufferState(const Shared<Buffer>& _buffer, ResourceState _beforeState, ResourceState _afterState)
	{
		EnqueueCommand([_buffer, _beforeState, _afterState]()
			{
				renderContext->TransitionBufferState(_buffer->GetGPUBuffer(), _beforeState, _afterState);
			});
	}


	void Renderer::GenerateMips(Texture* _texture)
	{
		EnqueueCommand([_texture]()
			{
				renderContext->GenerateMips(_texture->GetGPUTexture());
			});
	}

	void Renderer::ExecutePreFrameCommands()
	{
		while (!singleTimeCommandQueue.empty())
		{
			singleTimeCommandQueue.front()();
			singleTimeCommandQueue.pop();
		}
	}

	void Renderer::Submit()
	{
		// 현재 메인스레드가 작성한 큐의 인덱스와 포인터 가져오기
		const UInt32 submittedQueueIndex = recordingQueueIndex;
		RenderCommandQueue* submittedQueue = commandQueues[submittedQueueIndex].get();

		if (useRenderThread && renderThread)
		{
			// 큐의 상태를 사용중으로 잠금
			// memory_order_release는 이 시점 이전의 모든 메모리 기록이 다른 스레드에게 확실히 보이도록 보장
			commandQueueBusyFlags[submittedQueueIndex].store(true, std::memory_order_release);

			// 렌더스레드에 비동기 실행 요청 + 작업이 끝나면 실행할 함수
			renderThread->Submit(submittedQueue, [submittedQueueIndex]()
				{
					// [렌더 스레드에서 실행됨] 작업이 끝났으므로 큐 상태를 '사용 가능(false)'으로 변경
					commandQueueBusyFlags[submittedQueueIndex].store(false, std::memory_order_release);
					// wait하고 있을 commandQueueStateCV에게 작업이 끝났음을 알림
					commandQueueStateCV.notify_all();
				});

			// 새로 명령을 추가할 큐의 인덱스로 업데이트
			recordingQueueIndex = (recordingQueueIndex + 1) % MaxCommandListsInFlight;
			std::unique_lock<std::mutex> lock(commandQueueStateMutex);
			commandQueueStateCV.wait(lock, []()
				{
					// 메인 스레드가 다음으로 쓰려고 하는 큐를, 렌더 스레드가 아직도 지우지 못하고있다면
					// 메인 스레드는 CPU를 점유하지 않고 여기서 대기
					// 렌더 스레드가 아까 제출할 콜백을 통해 false로 바꿔주면 깨어나서 다음 프레임 로직을 진행
					return !commandQueueBusyFlags[recordingQueueIndex].load(std::memory_order_acquire);
				});
		}
		else
		{
			//싱글 스레드일 경우 그냥 실행
			submittedQueue->Execute();
		}

		currentLoop += 1;
	}
}
