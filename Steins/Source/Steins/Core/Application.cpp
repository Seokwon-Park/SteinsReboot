#include "SteinsPCH.h"

#include "Application.h"

#include "Input.h"
#include "KeyCodes.h"
#include "Steins/ImGui/ImGuiLayer.h"
#include "Steins/Graphics/Core/Renderer.h"
#include "Steins/Graphics/Utility/ShaderCompileHelper.h"


#include "glad/glad.h"

namespace Steins
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::instance = nullptr;

	Application::Application(ApplicationSpecification _specification)
	{
		STEINS_CORE_ASSERT(!instance, "Application already exists!");
		instance = this;

		WindowProps prop;
		prop.width = 1280;
		prop.height = 720;
		prop.title = _specification.Name;
		prop.rendererAPI = _specification.rendererAPI;

		//일단 프로그램 윈도우 생성
		mainWindow = SteinsWindow::Create(prop);
		if (mainWindow == nullptr)
		{
			STEINS_CORE_ASSERT(false, "No Main Window")
		}
		mainWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
		mainWindow->SetVSync(true);

		//prop.width = 960;
		//prop.height = 540;
		//prop.title = "TestWindow";
		//prop.rendererAPI = _specification.rendererAPI;

		//testWindow = SteinsWindow::Create(prop);
		//testWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
		
		//렌더러 초기화
		Renderer::Init(_specification.rendererAPI);
		//렌더러에 윈도우 
		Renderer::RegisterWindow(prop.title, mainWindow.get());
		Renderer::SetCurrentWindow(mainWindow.get());
		//Renderer::RegisterWindow("TestWindow", testWindow.get());

		imGuiLayer = new ImGuiLayer();
		AttachOverlay(imGuiLayer);

		ShaderCompileHelper::Init();
	}

	Application::~Application()
	{
		mainWindow->SetSwapchain(nullptr);
		mainWindow = nullptr;
		layerStack.Release();
		Renderer::Shutdown();
	}

	void Application::AttachLayer(Layer* _layer)
	{
		layerStack.PushLayer(_layer);
	}

	void Application::AttachOverlay(Layer* _overlay)
	{
		layerStack.PushOverlay(_overlay);
	}

	void Application::ReadConfig(std::string_view _fileName)
	{
		std::ifstream file(_fileName.data());
		STEINS_CORE_ASSERT(!file.is_open(), "Cannot Open Configuration File!")

			std::string line;

		/*while (std::getline(file, line)) {
			if (line.find("API=") != std::string::npos) {
				api = line.substr(line.find("=") + 1);
				break;
			}
		}*/
	}

	bool Application::Init()
	{
		isRunning = true;

		return true;
	}
	bool Application::Run()
	{
		while (isRunning)
		{
			timeStep.UpdateTime();
			float deltaTime = timeStep.GetDeltaTime();

			for (Layer* layer : layerStack)
			{
				layer->OnUpdate(deltaTime);
			}

			Renderer::BeginSwapchainFramebuffer();

			imGuiLayer->BeginImGui();
			{
				for (Layer* layer : layerStack)
					layer->OnImGuiRender();
			}
			imGuiLayer->EndImGui();

			Renderer::EndSwapchainFramebuffer();
			//auto [x, y] = Input::GetMousePosition();
			////STEINS_CORE_TRACE("{0}, {1}", x, y);

			//if (Input::GetMouseButtonPress(Mouse::ButtonLeft))
			//{
			//	STEINS_CORE_TRACE("MOUSE BUTTON DOWN TEST");
			//}

			if (Input::GetKeyDown(Key::A))
			{
				STEINS_CORE_TRACE("KEY DOWN TEST");
			}

			//if (Input::GetKeyPress(Key::B))
			//{
			//	STEINS_CORE_TRACE("KEY PRESS TEST");
			//}

			//if (Input::GetKeyUp(Key::C))
			//{
			//	STEINS_CORE_TRACE("KEY UP TEST");
			//}

			mainWindow->OnUpdateKeyState();
			mainWindow->OnUpdate();
			//testWindow->OnUpdate();
		}
		return true;
	}
	bool Application::Exit()
	{
		return true;
	}

	void Application::OnEvent(Event& _event)
	{
		EventDispatcher dispatcher(_event);
		//_event의 타입이 WindowCloseEvent에 해당하면 OnWindowClose를 실행시킨다.
		dispatcher.Dispatch<WindowFocusEvent>(BIND_EVENT_FN(OnWindowFocused));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (Array<Layer*>::iterator itr = layerStack.end(); itr != layerStack.begin();)
		{
			if (_event.handled) // if Event is already handled then break;
				break;
			//check event for layer
			(*--itr)->OnEvent(_event);
		}

		//STEINS_CORE_TRACE("{0}", _event.ToString());
	}

	bool Application::OnWindowClose(WindowCloseEvent& _event)
	{
		isRunning = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& _event)
	{
		if (_event.GetWidth() == 0 || _event.GetHeight() == 0)
		{
			isMinimized = true;
			return false; 
		}

		//STEINS_CORE_INFO("Window Resized : [ {0} , {1} ]", _event.GetWidth(), _event.GetHeight());
		isMinimized = false;
		Renderer::OnWindowResize(_event.GetWidth(), _event.GetHeight());
		mainWindow->GetSwapchain()->ResizeSwapchain(_event.GetWidth(), _event.GetHeight());

		return false;
	}
	bool Application::OnWindowFocused(WindowFocusEvent& _e)
	{
		if (_e.GetIsFocused() == true)
		{
			STEINS_CORE_INFO("{0} Window is now focused", _e.GetWindowName());
			//	Renderer::SetWindow(_e.GetWindowName());
		}
		return false;
	}
}
