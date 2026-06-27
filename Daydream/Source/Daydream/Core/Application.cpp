#include "DaydreamPCH.h"

#include "Application.h"

#include "Input.h"
#include "KeyCodes.h"
#include "Daydream/ImGui/ImGuiLayer.h"
#include "Daydream/Asset/AssetManager.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Scene/Components/ComponentRegistry.h"
#include "Daydream/Graphics/Manager/ResourceManager.h"


namespace Daydream
{
	Application* Application::instance = nullptr;

	Application::Application(ApplicationSpecification _specification)
	{
		DAYDREAM_CORE_ASSERT(!instance, "Application already exists!");
		instance = this;

		appSpec = _specification;

		prop.width = 1280;
		prop.height = 720;
		prop.title = _specification.Name;
		prop.useDefaultClientAPI = _specification.rendererAPI == RendererAPIType::OpenGL;

		imGuiLayer = nullptr;
		mainThreadID = std::this_thread::get_id();

		//prop.width = 960;
		//prop.height = 540;
		//prop.title = "TestWindow";
		//prop.rendererAPI = _specification.rendererAPI;

		//testWindow = DaydreamWindow::Create(prop);
		//testWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{

	}

	void Application::AttachLayer(Layer* _layer)
	{
		layerStack.PushLayer(_layer);
	}

	void Application::AttachOverlay(Layer* _overlay)
	{
		layerStack.PushOverlay(_overlay);
	}

	void Application::ReadConfig(const String& _fileName)
	{
		std::ifstream file(_fileName.data());
		DAYDREAM_CORE_ASSERT(!file.is_open(), "Cannot Open Configuration File!");

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

		if (!InitWindow())
		{
			DAYDREAM_CORE_ERROR("Failed To Initialize Main Window!");
			return false;
		}

		if (!InitRenderer())
		{
			DAYDREAM_CORE_ERROR("Failed To Initialize Renderer!");
			return false;
		}

		//에셋 매니저 초기화
		AssetManager::Init();
		AssetManager::LoadAssetMetadataFromDirectory("Asset");
		AssetManager::LoadAssetMetadataFromDirectory("Resource");

		AssetManager::LoadAssets(LoadPhase::Early);// 셰이더 때문에 renderer초기화 이후로 미룸

		ResourceManager::Init();

		imGuiLayer = new ImGuiLayer();
		AttachOverlay(imGuiLayer);

		ComponentRegistry::Init();
		Renderer::PostInit();

		return true;
	}

	bool Application::Run()
	{
		while (isRunning)
		{
			timeStep.UpdateTime();
			float deltaTime = timeStep.GetDeltaTime();

			Renderer::BeginFrame(GetMainWindowPtr());
			Renderer::ExecutePreFrameCommands(); // if ExecutePreFrameCommandQueue is not empty

			for (Layer* layer : layerStack)
			{
				layer->OnUpdate(deltaTime);
			}
			Renderer::BeginRendering(GetMainWindowPtr(), Color::Blue);
			imGuiLayer->BeginImGui();
			for (Layer* layer : layerStack)
				layer->OnImGuiRender();
			imGuiLayer->EndImGui();
			Renderer::EndRendering(RenderingInfo());
			//Renderer::EndSwapchainRenderPass(mainWindow->GetSwapchain());
			//auto [x, y] = Input::GetMousePosition();
			////DAYDREAM_CORE_TRACE("{0}, {1}", x, y);

			//if (Input::GetMouseDown(Mouse::ButtonLeft))
			//{
			//	DAYDREAM_CORE_TRACE("MOUSE BUTTON DOWN TEST");
			//}

			//if (Input::GetMouseReleased(Mouse::ButtonLeft))
			//{
			//	DAYDREAM_CORE_TRACE("MOUSE BUTTON RELEASE TEST");
			//}

			//if (Input::GetMousePressed(Mouse::ButtonLeft))
			//{
			//	DAYDREAM_CORE_TRACE("MOUSE DOWN TEST");
			//}

			//if (Input::GetKeyPress(Key::B))
			//{
			//	DAYDREAM_CORE_TRACE("KEY PRESS TEST");
			//}

			//if (Input::GetKeyUp(Key::C))
			//{
			//	DAYDREAM_CORE_TRACE("KEY UP TEST");
			//}
			Renderer::EndFrame(GetMainWindowPtr());
			imGuiLayer->UpdateImGuiWindows();
			Renderer::Submit();

			mainWindow->OnUpdateInputState();
			mainWindow->OnUpdate();

			//testWindow->OnUpdate();
		}
		return true;
	}
	bool Application::Exit()
	{
		ComponentRegistry::Shutdown();
		layerStack.Release();
		Renderer::WaitGPUIdle();
		ResourceManager::Shutdown();
		AssetManager::Shutdown();
		Renderer::Shutdown();
		WindowManager::Shutdown();
		mainWindow = nullptr;

		return true;
	}

	bool Application::InitWindow()
	{
		WindowManager::Init();

		//2. Create Main Window
		mainWindow = DaydreamWindow::Create(prop);
		if (mainWindow == nullptr)
		{
			DAYDREAM_CORE_ASSERT(false, "Main Window is not created!");
			return false;
		}
		mainWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
		mainWindow->SetVSync(true);

		//렌더러가 사용할 윈도우 설정
		//Renderer::RegisterWindow("TestWindow", testWindow.get());

		WindowManager::RegisterWindow(prop.title, &GetMainWindow());
		return true;
	}

	bool Application::InitRenderer()
	{
		//렌더러 초기화
		Renderer::Init(appSpec.rendererAPI);
		//렌더러에서 윈도우에 대한 스왑체인 생성
		if (!Renderer::CreateSwapchain(*mainWindow))
		{
			return false;
		}
		return true;
	}

	void Application::OnEvent(Event& _event)
	{
		EventDispatcher dispatcher(_event);
		//_event의 타입이 WindowCloseEvent에 해당하면 OnWindowClose를 실행시킨다.
		dispatcher.Dispatch<WindowFocusEvent>(BIND_EVENT_FN(OnWindowFocused));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResized));

		for (Array<Layer*>::iterator itr = layerStack.end(); itr != layerStack.begin();)
		{
			if (_event.handled) // if Event is already handled then break;
				break;
			//check event for layer
			(*--itr)->OnEvent(_event);
		}

		//DAYDREAM_CORE_TRACE("{0}", _event.ToString());
	}

	bool Application::OnWindowClosed(WindowCloseEvent& _event)
	{
		isRunning = false;
		return true;
	}
	bool Application::OnWindowResized(WindowResizeEvent& _event)
	{
		if (_event.GetWidth() == 0 || _event.GetHeight() == 0)
		{
			isMinimized = true;
			return false;
		}

		//DAYDREAM_CORE_INFO("Window Resized : [ {0} , {1} ]", _event.GetWidth(), _event.GetHeight());
		isMinimized = false;
		DaydreamWindow* window = WindowManager::GetWindow(_event.GetWindowName());
		if (window == nullptr)
		{
			DAYDREAM_CORE_ASSERT(false, "Window Resize Error!");
			return true;
		}
		Renderer::OnSwapchainResize(window, _event.GetWidth(), _event.GetHeight());

		return false;
	}
	bool Application::OnWindowFocused(WindowFocusEvent& _e)
	{
		if (_e.GetIsFocused() == true)
		{
			DAYDREAM_CORE_INFO("{0} Window is now focused", _e.GetWindowName());
			//	Renderer::SetWindow(_e.GetWindowName());
		}
		return false;
	}
}
