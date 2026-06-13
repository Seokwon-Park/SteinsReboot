#pragma once

#include "RenderGraphDrawList.h"
#include "Daydream/Graphics/Pools/TexturePool/Texture2DPool.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
	enum class PassDrawType
	{
		DrawMesh,       // 3D 메쉬들을 그리는 패스 (예: GBuffer)
		DrawDepthStencil,      // 깊이만 그리는 패스 (예: Depth)
		FullScreenQuad, // 화면 전체를 덮는 2D 후처리 패스 (예: Lighting, Bloom)
		Compute         // 컴퓨트 셰이더 패스
	};

	struct ExternalConstantBufferData
	{
		String bindName;
		const void* data;
		UInt32 size;
	};

	struct ExternalShaderResourceView
	{
		String bindName;
		TextureView* SRV;
	};

	struct RenderGraphResourceDesc
	{
		RenderFormat format;
		UInt32 width;
		UInt32 height;
	};

	struct RenderGraphPassDesc
	{
		const GraphicsPipelineState* pipelineState;
		RenderGraphDrawList drawList;
		PassDrawType drawType;
		Array<ExternalConstantBufferData> constantBufferData;
		Array<ExternalShaderResourceView> shaderResourceViews;
	};

	struct RenderGraphResourceHandle
	{
		UInt32 id = UINT32_MAX;
		bool IsValid() const { return id != UINT32_MAX; }
	};

	struct RenderGraphPassHandle
	{
		UInt32 id = UINT32_MAX;
		bool IsValid() const { return id != UINT32_MAX; }
	};

	struct RenderGraphWriteBinding
	{
		UInt32 resourceId = UINT32_MAX;
		AttachmentLoadOp loadOp = AttachmentLoadOp::Clear;
		AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
	};

	class RenderGraph
	{
	public:
		RenderGraph();
		~RenderGraph();
		RenderGraphResourceHandle AddResource(const String& _name, const RenderGraphResourceDesc& _desc);
		RenderGraphResourceHandle AddExternalWriteResource(const String& _name, const Texture2DAllocation& _texture);
		RenderGraphPassHandle AddPass(const String& _name, const RenderGraphPassDesc& _desc);
		void AddPassDependency(RenderGraphPassHandle _beforePass, RenderGraphPassHandle _afterPass);

		void Read(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource);
		void Write(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource, AttachmentLoadOp _loadOp = AttachmentLoadOp::Clear, AttachmentStoreOp _storeOp = AttachmentStoreOp::Store);
		void WriteDepthStencil(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource, AttachmentLoadOp _loadOp = AttachmentLoadOp::Clear, AttachmentStoreOp _storeOp = AttachmentStoreOp::Store);

		bool Compile();
		void Execute();
		void Reset();

	private:
		struct ResourceNode
		{
			String name;
			RenderFormat format;
			UInt32 width;
			UInt32 height;

			UInt32 firstPass;
			UInt32 lastPass;

			Texture2DAllocation allocation;

			Bool isExternal = false;
		};

		struct PassNode
		{
			String name;

			const GraphicsPipelineState* pipelineState;
			RenderGraphDrawList drawList;
			PassDrawType drawType;

			// There's nothing to read from the previous pass, but if the order matters
			Array<UInt32> passDependency;
			Array<UInt32> reads;
			Array<RenderGraphWriteBinding> colorWrites;
			RenderGraphWriteBinding depthStencilWrite{};

			Array<ExternalConstantBufferData> constantBufferData;
			Array<ExternalShaderResourceView> shaderResourceViews;
		};

		void BuildDependencyGraph(Array<Array<UInt32>>& _edges, Array<UInt32>& _inDegree) const;

		Array<ResourceNode> resources;
		Array<PassNode> passes;
		Array<UInt32> executionOrder;

		Mesh* quadMesh;
	};
}
