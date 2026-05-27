#pragma once

#include "D3D12RenderDevice.h"
#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream
{
	class D3D12Sampler : public Sampler
	{
	public:
		D3D12Sampler(D3D12RenderDevice* _device, const SamplerDesc& _desc);
		virtual ~D3D12Sampler();

		inline D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerHandle() const { return samplerGpuHandle; }
	protected:

	private:
		D3D12RenderDevice* device;

		D3D12_CPU_DESCRIPTOR_HANDLE samplerCpuHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle = {};
	};
}
