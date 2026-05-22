#pragma once

#include "D3D12RenderDevice.h"
#include "D3D12Texture.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class D3D12TextureView : public TextureView
	{
	public:
		explicit D3D12TextureView(D3D12RenderDevice* _device, D3D12GPUTexture* _texture,const TextureViewDesc& _desc);
		~D3D12TextureView() = default;

		virtual void* GetUIHandle() const override { return reinterpret_cast<void*>(GetGPUHandle().ptr); }

		void SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle) { cpuHandle = _cpuHandle; }
		void SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle) { gpuHandle = _gpuHandle; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return cpuHandle; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return gpuHandle; }

	private:
		TextureViewDesc desc;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};
	};
}
