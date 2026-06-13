#pragma once

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "D3D11RenderDevice.h"

namespace Daydream
{
	class D3D11GraphicsPipelineState : public GraphicsPipelineState
	{
	public:
		D3D11GraphicsPipelineState(D3D11RenderDevice* _device, GraphicsPipelineStateDesc _desc);

		virtual void Bind() const override;
		void BindPipelineState() const;
		//virtual Shared<Material> CreateMaterial() override;
	private:
		D3D11RenderDevice* device;

		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11GeometryShader* geometryShader = nullptr;
		ID3D11HullShader* hullShader = nullptr;
		ID3D11DomainShader* domainShader = nullptr;

		ComPtr<ID3D11RasterizerState> rasterizer;
		ComPtr<ID3D11DepthStencilState> depthStencil;
		ComPtr<ID3D11InputLayout> inputLayout;

	};
}
