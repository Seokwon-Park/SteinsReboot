#include "DaydreamPCH.h"
#include "RenderGraph.h"

#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Resources/Struct/TransformConstantBufferData.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	RenderGraph::RenderGraph()
	{
		transformCB = ConstantBuffer::Create(sizeof(TransformConstantBufferData));
	}

	RenderGraph::~RenderGraph()
	{

	}

	RenderGraphResourceHandle RenderGraph::AddResource(const String& _name, const RenderGraphResourceDesc& _desc)
	{
		ResourceNode node{};
		node.name = _name;
		node.format = _desc.format;
		node.width = _desc.width;
		node.height = _desc.height;
		node.firstPass = UINT32_MAX;
		node.lastPass = 0;

		resources.push_back(std::move(node));
		return { static_cast<UInt32>(resources.size() - 1) };
	}
	RenderGraphPassHandle RenderGraph::AddPass(const String& _name, const RenderGraphPassDesc& _desc)
	{
		PassNode node{};
		node.name = _name;
		node.pipelineState = _desc.pipelineState;
		node.drawType = _desc.drawType;
		node.drawList = _desc.drawList;

		//node contains Shared pointer
		passes.push_back(std::move(node));
		return { static_cast<UInt32>(passes.size() - 1) };
	}

	void RenderGraph::Read(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource)
	{
		if (!_pass.IsValid() || !_resource.IsValid())return;
		if (_pass.id >= passes.size() || _resource.id >= resources.size())return;
		passes[_pass.id].reads.push_back(_resource.id);
	}
	void RenderGraph::Write(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource)
	{
		if (!_pass.IsValid() || !_resource.IsValid())return;
		if (_pass.id >= passes.size() || _resource.id >= resources.size())return;
		passes[_pass.id].writes.push_back(_resource.id);
	}

	bool RenderGraph::Compile()
	{
		executionOrder.clear();

		Array<Array<UInt32>> edges;
		Array<UInt32> inDegree;
		BuildDependencyGraph(edges, inDegree);

		//렌더링 순서 위상정렬
		Queue<UInt32> readyPasses;
		for (UInt32 i = 0; i < inDegree.size(); i++)
		{
			//선행 작업이 없으면 추가
			if (inDegree[i] == 0)
			{
				readyPasses.push(i);
			}
		}

		while (!readyPasses.empty())
		{
			UInt32 passId = readyPasses.front();
			readyPasses.pop();
			executionOrder.push_back(passId);

			for (UInt32 next : edges[passId])
			{
				if (--inDegree[next] == 0)
				{
					readyPasses.push(next);
				}
			}
		}

		for (UInt32 passId : executionOrder)
		{
			PassNode& pass = passes[passId];

			auto updateLifetime = [&](UInt32 _resId)
				{
					ResourceNode& res = resources[_resId];
					if (res.firstPass == UINT32_MAX)
					{
						res.firstPass = passId;
					}
					res.lastPass = passId;
				};

			for (UInt32 resId : pass.reads) updateLifetime(resId);
			for (UInt32 resId : pass.writes) updateLifetime(resId);
		}

		return executionOrder.size() == passes.size();
	}

	void RenderGraph::Execute()
	{
		for (UInt32 passId : executionOrder)
		{
			PassNode& pass = passes[passId];
			
			RenderingInfo renderingInfo{};
			UInt32 passWidth = resources[pass.writes[0]].width;
			UInt32 passHeight = resources[pass.writes[0]].height;
			for (UInt32 resId : pass.writes)
			{
				ResourceNode& resource = resources[resId];
				if (passWidth != resource.width || passHeight != resource.height)
				{
					DAYDREAM_RENDERER_ERROR("Resource {} has different size!", resource.name);
					return;
				}

				if (resource.firstPass == passId)
				{
					resource.resourceHandle = Renderer::GetRenderTargetPool()->RequestTexture2DHandle(
						resource.width, resource.height, resource.format
					);
				}
								
				AttachmentDesc attachDesc{};
				if (!GraphicsUtility::IsDepthFormat(resource.format))
				{
					attachDesc.view = resource.resourceHandle.GetRenderTargetView();
					renderingInfo.colorAttachments.push_back(attachDesc);

					Renderer::TransitionTextureState(resource.resourceHandle.GetTexture(), ResourceState::Undefined, ResourceState::RenderTarget);
				}
				else
				{
					attachDesc.view = resource.resourceHandle.GetDepthStencilView();
					renderingInfo.depthAttachment = attachDesc;
					Renderer::TransitionTextureState(resource.resourceHandle.GetTexture(), ResourceState::Undefined, ResourceState::DepthWrite);
				}
			}

			renderingInfo.renderArea.x = 0;
			renderingInfo.renderArea.y = 0;
			renderingInfo.renderArea.width = passWidth;
			renderingInfo.renderArea.height = passHeight;

			Renderer::BindPipelineState(passes[passId].pipelineState);

			switch (pass.drawType)
			{
			case PassDrawType::DrawMesh:
			{
				for (auto& renderItem : pass.drawList)
				{
					TransformConstantBufferData transformData;
					transformData.world = renderItem.worldMatrix.Transposed();
					transformData.worldInverseTranspose = transformData.world.Inversed().Transposed();
					Renderer::UpdateConstantBuffer(transformCB, transformData);
					Renderer::BindConstantBuffer("World", transformCB);
					Renderer::BindMesh(renderItem.mesh);
					Renderer::BindMaterial(renderItem.material);
					Renderer::DrawIndexed(renderItem.mesh->GetIndexCount());
				}
				break;
			}
			case PassDrawType::DrawDepthStencil:
			{
				for (auto& renderItem : pass.drawList)
				{
					TransformConstantBufferData transformData;
					transformData.world = renderItem.worldMatrix.Transposed();
					transformData.worldInverseTranspose = transformData.world.Inversed().Transposed();
					Renderer::UpdateConstantBuffer(transformCB, transformData);
					Renderer::BindConstantBuffer("World", transformCB);
					Renderer::BindMesh(renderItem.mesh);
					Renderer::DrawIndexed(renderItem.mesh->GetIndexCount());
				}
				break;
			}
			case PassDrawType::FullScreenQuad:
				break;
			case PassDrawType::Compute:
				break;
			default:
				break;
			}

			//pass를 그리고 난 다음 읽는데 쓴 resource들이 더이상 필요가 없는지 확인
			for (UInt32 resId : pass.reads)
			{
				ResourceNode& resource = resources[resId];
				if (resource.lastPass == passId)
				{
					Renderer::GetRenderTargetPool()->ReturnTexture2DHandle(std::move(resource.resourceHandle), Renderer::GetCurrentLoop());
				}
			}
		}
	}

	void RenderGraph::Reset()
	{
		resources.clear();
		passes.clear();
		executionOrder.clear();
	}

	void RenderGraph::BuildDependencyGraph(Array<Array<UInt32>>& _edges, Array<UInt32>& _inDegree) const
	{
		_edges.clear();
		_inDegree.clear();

		_edges.resize(passes.size());
		_inDegree.resize(passes.size(), 0);

		//위상정렬을 위한 그래프를 구성한다.
		for (UInt32 writerId = 0; writerId < passes.size(); writerId++)
		{
			for (UInt32 readerId = 0; readerId < passes.size(); readerId++)
			{
				//쓰는pass랑 읽는pass가 같은 경우는 continue
				if (writerId == readerId)
				{
					continue;
				}

				//writer가 작성해야되는 리소스들 중에서
				bool dependent = false;
				for (UInt32 writtenResId : passes[writerId].writes)
				{
					auto it = std::find(passes[readerId].reads.begin(), passes[readerId].reads.end(), writtenResId);

					//readerPass가 writerPass로부터읽어야 하는 리소스가 있으면
					if (it != passes[readerId].reads.end())
					{
						dependent = true;
						break;
					}
				}

				//reader는 writer 작업이 선행되어야 하므로 reader의 indegree를 추가해주고 단방향 간선을 생성
				if (dependent)
				{
					_edges[writerId].push_back(readerId);
					_inDegree[readerId]++;
				}
			}
		}
	}


}

