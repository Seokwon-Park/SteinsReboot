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

	class RenderGraph
	{
	public:
		RenderGraph();
		~RenderGraph();

		RenderGraphResourceHandle AddResource(const String& _name, const RenderGraphResourceDesc& _desc);
		RenderGraphPassHandle AddPass(const String& _name, const RenderGraphPassDesc& _desc);

		void Read(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource);
		void Write(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource);

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

			Texture2DPoolHandle resourceHandle;
		};

		struct PassNode
		{
			String name;

			const GraphicsPipelineState* pipelineState;
			RenderGraphDrawList drawList;
			PassDrawType drawType;

			Array<UInt32> reads;
			Array<UInt32> writes;
		};

		void BuildDependencyGraph(Array<Array<UInt32>>& _edges, Array<UInt32>& _inDegree) const;

		Array<ResourceNode> resources;
		Array<PassNode> passes;
		Array<UInt32> executionOrder;

		Shared<ConstantBuffer> transformCB;
	};
}
