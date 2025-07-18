#pragma once

#include "Steins/Graphics/Resources/Texture.h"
#include "D3D12RenderDevice.h"

namespace Steins
{
	class D3D12Texture2D : public Texture2D
	{
	public:
		D3D12Texture2D(D3D12RenderDevice* _device, const FilePath& _path);

		virtual void Bind(UInt32 _slot) const override;

		virtual void* GetNativeHandle() override { return reinterpret_cast<void*>(gpuHandle.ptr); };
		//const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return gpuHandle; };
	private:
		D3D12RenderDevice* device;
		ComPtr<ID3D12Resource> uploadBuffer;
		ComPtr<ID3D12Resource> texture;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};
}
