#include "DaydreamPCH.h"
#include "RenderGraph.h"

#include "Daydream/Asset/AssetManager.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Resources/Struct/TransformData.h"
#include "Daydream/Graphics/Resources/BuiltInResources.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	RenderGraph::RenderGraph()
	{

		quadMesh = AssetManager::GetAsset<Mesh>(AssetDefaults::DefaultQuadMeshHandle);
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

		node.currentState = ResourceState::Undefined;

		resources.push_back(std::move(node));
		return { static_cast<UInt32>(resources.size() - 1) };
	}
	RenderGraphResourceHandle RenderGraph::AddExternalWriteResource(const String& _name, const Shared<Texture2D>& _texture)
	{
		ResourceNode node{};
		node.name = _name;
		node.format = _texture->GetFormat();
		node.width = _texture->GetWidth();
		node.height = _texture->GetHeight();

		node.allocation.texture = _texture;

		TextureViewDesc rtvDesc{};
		rtvDesc.type = TextureViewType::RenderTarget;
		rtvDesc.baseMip = 0;
		rtvDesc.mipLevels = 1;
		rtvDesc.baseLayer = 0;
		rtvDesc.layerCount = 1;
		node.allocation.renderTargetView = TextureView::Create(_texture, rtvDesc);

		TextureViewDesc srvDesc{};
		srvDesc.type = TextureViewType::ShaderResource;
		srvDesc.baseMip = 0;
		srvDesc.mipLevels = 1;
		srvDesc.baseLayer = 0;
		srvDesc.layerCount = 1;
		node.allocation.shaderResourceView = TextureView::Create(_texture, srvDesc);

		node.isExternal = true;
		node.currentState = ResourceState::RenderTarget;

		resources.push_back(node);
		return { static_cast<UInt32>(resources.size() - 1) };
	}

	RenderGraphPassHandle RenderGraph::AddPass(const String& _name, const RenderGraphPassDesc& _desc)
	{
		PassNode node{};
		node.name = _name;
		node.pipelineState = _desc.pipelineState;
		node.drawType = _desc.drawType;
		node.drawList = _desc.drawList;
		node.constantBufferData = _desc.constantBufferData;
		node.shaderResourceViews = _desc.shaderResourceViews;
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
		passes[_pass.id].colorWrites.push_back(_resource.id);
	}

	void RenderGraph::WriteDepthStencil(RenderGraphPassHandle _pass, RenderGraphResourceHandle _resource)
	{
		if (!_pass.IsValid() || !_resource.IsValid())return;
		if (_pass.id >= passes.size() || _resource.id >= resources.size())return;
		if (!GraphicsUtility::IsDepthFormat(resources[_resource.id].format))
		{
			DAYDREAM_CORE_ERROR("Write Depth Resource must be depth format");
			return;
		}
		passes[_pass.id].depthStencilWrite = _resource.id;
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
					if (_resId == UINT32_MAX) return;
					ResourceNode& res = resources[_resId];
					if (res.firstPass == UINT32_MAX)
					{
						res.firstPass = passId;
					}
					res.lastPass = passId;
				};

			for (UInt32 resId : pass.reads) updateLifetime(resId);
			for (UInt32 resId : pass.colorWrites) updateLifetime(resId);
			updateLifetime(pass.depthStencilWrite);
		}

		return executionOrder.size() == passes.size();
	}

	void RenderGraph::Execute()
	{
		for (UInt32 passId : executionOrder)
		{
			PassNode& pass = passes[passId];

			RenderingInfo renderingInfo{};

			UInt32 passWidth = 0;
			UInt32 passHeight = 0;

			auto prepareResource = [&](ResourceNode& resource) -> Texture2DAllocation&
				{
					if (passWidth == 0)
					{
						passWidth = resource.width;
						passHeight = resource.height;
					}

					if (resource.isExternal) return resource.allocation;

					else if (passWidth != resource.width || passHeight != resource.height)
					{
						DAYDREAM_RENDERER_ERROR("Resource {} has different size!", resource.name);
					}

					if (resource.firstPass == passId)
					{
						resource.allocation = Renderer::GetTexturePool()->AllocateTexture2DHandle(
							resource.width, resource.height, resource.format
						);
					}

					return resource.allocation;
				};


			for (UInt32 resId : pass.colorWrites)
			{ 
				ResourceNode& resource = resources[resId];
				Texture2DAllocation& payload = prepareResource(resource);

				AttachmentDesc attachDesc{};
				attachDesc.view = payload.renderTargetView.get();
				renderingInfo.colorAttachments.push_back(attachDesc);

				Renderer::TransitionTextureState(payload.texture.get(), resource.currentState, ResourceState::RenderTarget);
				resource.currentState = ResourceState::RenderTarget;
			}

			if (pass.depthStencilWrite != UINT_MAX)
			{
				ResourceNode& resource = resources[pass.depthStencilWrite];
				Texture2DAllocation& payload = prepareResource(resource);

				AttachmentDesc attachDesc{};
				attachDesc.view = payload.depthStencilView.get();
				renderingInfo.depthAttachment = attachDesc;

				Renderer::TransitionTextureState(payload.texture.get(), resource.currentState, ResourceState::DepthWrite);
				resource.currentState = ResourceState::DepthWrite;
			}

			DAYDREAM_CORE_ASSERT(passWidth != 0 && passHeight!= 0, "wrong pass size");

			for (UInt32 resId : pass.reads)
			{
				ResourceNode& resource = resources[resId];

				Renderer::TransitionTextureState(resource.allocation.texture.get(), resource.currentState, ResourceState::ShaderResource);
				resource.currentState = ResourceState::ShaderResource;
			}

			renderingInfo.renderArea.x = 0;
			renderingInfo.renderArea.y = 0;
			renderingInfo.renderArea.width = passWidth;
			renderingInfo.renderArea.height = passHeight;

			Renderer::BeginRendering(renderingInfo);
			Renderer::BindPipelineState(passes[passId].pipelineState);

			for (UInt32 resId : pass.reads)
			{
				ResourceNode& resource = resources[resId];

				Renderer::BindShaderResourceView(resource.name, resource.allocation.shaderResourceView.get(), BuiltIn::Samplers::LinearClampToEdge());
			}

			for (auto& cbData : pass.constantBufferData)
			{
				Shared<ConstantBuffer> constantBuffer = Renderer::GetConstantBufferPool()->RequestBuffer(cbData.size);
				constantBuffer->UpdateData(cbData.data, cbData.size);
				Renderer::BindConstantBuffer(cbData.bindName, constantBuffer);
				Renderer::GetConstantBufferPool()->ReturnResource(cbData.size, std::move(constantBuffer));
			}


			for (auto& srvData : pass.shaderResourceViews)
			{
				Renderer::BindShaderResourceView(srvData.bindName, srvData.SRV, BuiltIn::Samplers::LinearClampToEdge());
			}

			switch (pass.drawType)
			{
			case PassDrawType::DrawMesh:
			{
				for (auto& renderItem : pass.drawList)
				{
					TransformConstantBufferData transformData;
					transformData.world = renderItem.worldMatrix.Transposed();
					transformData.worldInverseTranspose = transformData.world.Inversed().Transposed();

					Shared<ConstantBuffer> constantBuffer = Renderer::GetConstantBufferPool()->RequestBuffer(sizeof(TransformConstantBufferData));
					constantBuffer->UpdateData(&transformData, sizeof(transformData));
					Renderer::BindConstantBuffer("World", constantBuffer);
					Renderer::GetConstantBufferPool()->ReturnResource(constantBuffer->GetSize(), std::move(constantBuffer));
					
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

					Shared<ConstantBuffer> constantBuffer = Renderer::GetConstantBufferPool()->RequestBuffer(sizeof(TransformConstantBufferData));
					constantBuffer->UpdateData(&transformData, sizeof(transformData));
					Renderer::BindConstantBuffer("World", constantBuffer);
					Renderer::GetConstantBufferPool()->ReturnResource(constantBuffer->GetSize(), std::move(constantBuffer));

					Renderer::BindMesh(renderItem.mesh);
					Renderer::DrawIndexed(renderItem.mesh->GetIndexCount());
				}
				break;
			}
			case PassDrawType::FullScreenQuad:
			{
				Renderer::BindMesh(quadMesh);
				Renderer::DrawIndexed(quadMesh->GetIndexCount());
			}
			break;
			case PassDrawType::Compute:
				break;
			default:
				break;
			}
			Renderer::EndRendering(renderingInfo);



			//pass를 그리고 난 다음 읽는데 쓴 resource들이 더이상 필요가 없는지 확인
			for (UInt32 resId : pass.reads)
			{
				ResourceNode& resource = resources[resId];

				if (resource.isExternal) continue;

				if (resource.lastPass == passId)
				{
					Texture2DPoolKey key{};
					key.width = resource.width;
					key.height = resource.height;
					key.format = resource.format;

					Renderer::GetTexturePool()->ReturnResource(key, std::move(resource.allocation));
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
				for (UInt32 writtenResId : passes[writerId].colorWrites)
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

