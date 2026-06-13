#pragma once

#include "Base/Time/TimeStep.h"

#include "Daydream/Core/Window.h"
#include "Daydream/Core/WindowManager.h"
#include "Daydream/Core/LayerStack.h"
#include "Daydream/Event/KeyEvent.h"
#include "Daydream/Event/ApplicationEvent.h"

#include "Daydream/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Daydream
{
	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int _index) const
		{
			DAYDREAM_CORE_ASSERT(_index < count, "Out of index");
			return args[_index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Daydream Application";
		std::string WorkingDirectory = "";
		UInt32 width = 1280;
		UInt32 height = 720;
		RendererAPIType rendererAPI = RendererAPIType::OpenGL;
	};

	class DAYDREAM_API Application
	{
	public:
		Application(ApplicationSpecification _specification);
		virtual ~Application();

		inline static Application& GetInstance() { return *instance; }
		inline DaydreamWindow& GetMainWindow() { return *mainWindow; }
		inline DaydreamWindow* GetMainWindowPtr() { return mainWindow.get(); }

		void AttachLayer(Layer* _layer);
		void AttachOverlay(Layer* _overlay);

		void ReadConfig(const String& _fileName);

		ImGuiLayer* GetImGuiLayer() { return imGuiLayer; }
	protected:
		virtual bool Init();
		virtual bool Run();
		virtual bool Exit();

		bool InitWindow();
		bool InitRenderer();
	private:
		friend int ::main(int argc, char** argv);

		void OnEvent(Event& _e);
		bool OnWindowClosed(WindowCloseEvent& _e);
		bool OnWindowResized(WindowResizeEvent& _e);
		bool OnWindowFocused(WindowFocusEvent& _e);

		Unique<DaydreamWindow> mainWindow;
		//Unique<DaydreamWindow> testWindow;

		bool isRunning = false;
		bool isMinimized = false;
		static Application* instance;

		ApplicationSpecification appSpec;
		WindowDesc prop;

		//Layers
		ImGuiLayer* imGuiLayer;
		LayerStack layerStack;

		TimeStep timeStep;

		inline static std::thread::id mainThreadID;
	};

	// To be defined in client
	Application* CreateApplication();
}

