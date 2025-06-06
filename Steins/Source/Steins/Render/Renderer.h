#pragma once

#include "RenderDevice.h"

#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Shader.h"

namespace Steins
{
	class Renderer
	{
	public:
		static void Init(RendererAPIType _API);
		static void Shutdown();
		static void RegisterWindow(std::string _name, SteinsWindow* _window);
		static void OnWindowResize(UInt32 _width, UInt32 _height);
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(UInt32 _indexCount, const Matrix4x4 _transform = Matrix4x4());

		inline static RendererAPIType GetAPI() { return renderDevice->GetAPI(); }

		inline static RenderDevice* GetRenderDevice() { return renderDevice.get(); }
	private:
		static SteinsWindow* currentWindow;
		static Unique<RenderDevice> renderDevice;
		static std::unordered_map<std::string, SteinsWindow*> windows;

		struct SceneData
		{
			Matrix4x4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}


