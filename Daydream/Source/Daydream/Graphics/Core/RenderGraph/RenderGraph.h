#pragma once

#include "RenderGraphPass.h"
#include "RenderGraphDrawList.h"

#include "Daydream/Graphics/Pools/TexturePool/Texture2DPool.h"

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

	struct ExternalSth
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



	struct ResourceNode
	{
		String name;
		RenderFormat format;
		UInt32 width;
		UInt32 height;

		UInt32 firstPass;
		UInt32 lastPass;

		Texture2DAllocation allocation;
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

	};

	class RenderGraph
	{
	public:
		RenderGraph();
		~RenderGraph();

		RenderGraphResourceHandle AddResource(const String& _name, const RenderGraphResourceDesc& _desc);
		void RemoveResource(RenderGraphResourceHandle _handle);
		RenderGraphResourceHandle AddExternalWriteResource(const String& _name, const Texture2DAllocation& _texture);
		RenderGraphPassHandle AddRenderPass(const String& _name, const RenderPassDesc& _desc);
		void RemoveRenderPass(RenderGraphPassHandle _handle);
		/*void AddPassDependency(RenderGraphPassHandle _beforePass, RenderGraphPassHandle _afterPass);*/

		void AddConstantBuffer(const String& _name, ConstantBuffer* _buffer);
		void AddShaderResourceView(const String& _name, TextureView* _shaderResourceView);

		const Array<ResourceNode>& GetResourceNodes() const { return resources; }
		const Array<RenderGraphPass>& GetRenderPasses() const { return passes; };

		void Read(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource);
		void Write(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource, AttachmentLoadOp _loadOp = AttachmentLoadOp::Clear, AttachmentStoreOp _storeOp = AttachmentStoreOp::Store);
		void WriteDepthStencil(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource, AttachmentLoadOp _loadOp = AttachmentLoadOp::Clear, AttachmentStoreOp _storeOp = AttachmentStoreOp::Store);

		bool Compile();
		void Execute();
		void Reset();

	private:
		void BuildDependencyGraph(Array<Array<UInt32>>& _edges, Array<UInt32>& _inDegree) const;

		Array<UInt32> freeResHandles;
		Array<UInt32> freePassHandles;

		Array<ResourceNode> resources;
		Array<RenderGraphPass> passes;
		Array<UInt32> executionOrder;
		HashMap<String, ConstantBuffer*> buffers;
		HashMap<String, Texture*> textures;
	};
}
