#pragma once

#include <Daydream.h>

namespace Daydream
{
	class Sandbox2D : public Layer
	{
	public:
		Sandbox2D();
		virtual ~Sandbox2D() = default;

		virtual void OnUpdate(Float32 _deltaTime) override;
		virtual void OnImGuiRender() override;

	private:
		Shared<VertexBuffer> squareVB;
		Shared<VertexBuffer> squareVB2;
		Shared<IndexBuffer> squareIB;
		Shared<IndexBuffer> squareIB2;
		Shared<ConstantBuffer> viewProjMat;
		Shared<Texture2D> texture;
		Shared<Texture2D> texture2;

		Shared<Shader> vs;
		Shared<Shader> ps;

		Shared<Shader> vs3d;
		Shared<Shader> ps3d;

		Shared<GraphicsPipelineState> pso;
		Shared<GraphicsPipelineState> pso3d;
		Shared<RenderGraphPass> renderPass;

		Shared<Material> material;
		Shared<Material> material3d;
		Shared<Framebuffer> framebuffer;

		Shared<Mesh> mesh;
		Shared<Model> model;

		//OrthographicCamera camera = OrthographicCamera(-1.6f, 1.6f, -0.9f, 0.9f);
		Matrix4x4 cameraPos;
	};
}

