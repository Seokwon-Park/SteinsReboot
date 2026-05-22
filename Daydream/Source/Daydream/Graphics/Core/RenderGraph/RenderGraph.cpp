#include "DaydreamPCH.h"
#include "RenderGraph.h"

#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{
	RenderGraph::RenderGraph()
	{

	}

	RenderGraph::~RenderGraph()
	{

	}

	RenderGraphResourceHandle RenderGraph::AddResource(const String& _name, const RenderGraphResourceDesc& _desc)
	{
		ResourceNode node{};
		node.name = _name;
		node.format = _desc.format;
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

			for (UInt32 resId : pass.reads)
			{
				for (UInt32 resId : pass.reads) updateLifetime(resId);
				for (UInt32 resId : pass.writes) updateLifetime(resId);
			}
		}

		return executionOrder.size() == passes.size();
	}

	void RenderGraph::Execute()
	{
		for (UInt32 passId : executionOrder)
		{
			auto allocateRenderTargetHandle = [&](UInt32 _resId)
				{
					auto& res = resources[_resId];
					// 만약 리소스가 이번 패스부터 쓰이기 시작한다면
					if (res.firstPass == passId)
					{
						// 렌더타겟 풀에서 할당받음
						res.resourceHandle = Renderer::GetRenderTargetPool()->RequestRenderTargetView(
							res.width, res.height, res.format
						);
					}
					
				};

			PassNode& pass = passes[passId];
			for (UInt32 resId : pass.writes)
			{
				ResourceNode& resource = resources[resId];
				RenderingInfo renderingInfo{};

				renderingInfo.renderArea.x = 0;
				renderingInfo.renderArea.y = 0;
				renderingInfo.renderArea.width = resource.width;
				renderingInfo.renderArea.height = resource.height;

				AttachmentDesc attachDesc{};
				attachDesc.view = resource.resourceHandle.GetRenderTargetView();

				renderingInfo.colorAttachments.push_back(attachDesc);
			}
			


			Renderer::BindPipelineState(passes[passId].pipelineState);
			//if (passIndex >= passes.size()) continue;
			//if (passes[passIndex].execute)
			//{
			//	passes[passIndex].execute();
			//}
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

