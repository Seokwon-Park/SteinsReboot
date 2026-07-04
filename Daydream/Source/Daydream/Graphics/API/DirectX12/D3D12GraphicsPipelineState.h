#pragma once

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

#include "D3D12RenderDevice.h"

namespace Daydream
{
	class D3D12GraphicsPipelineState : public GraphicsPipelineState
	{
	public:
		// Constrcuter Destructer
		D3D12GraphicsPipelineState(D3D12RenderDevice* _device, GraphicsPipelineStateDesc _desc);
		virtual ~D3D12GraphicsPipelineState();

		ID3D12RootSignature* GetID3D12RootSignature() const { return rootSignature.Get(); }
		ID3D12PipelineState* GetID3D12PipelineState() const { return pipeline.Get(); }
		UInt32 GetDescriptorTableIndex(String _resourceName) const;
	protected:

	private:
		D3D12RenderDevice* device;
		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipeline;

		HashMap<String, UInt32> descriptorTable;

		Array<D3D12_ROOT_PARAMETER> rootParameters;
		Array<D3D12_DESCRIPTOR_RANGE> srvRanges;
		Array<D3D12_DESCRIPTOR_RANGE> samplerRanges;
	};
}
