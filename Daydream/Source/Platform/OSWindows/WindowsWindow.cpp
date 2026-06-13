#include "DaydreamPCH.h"
#include "WindowsWindow.h"

#include "Daydream/Event/KeyEvent.h"
#include "Daydream/Event/MouseEvent.h"
#include "Daydream/Event/ApplicationEvent.h"

#include "Daydream/Graphics/API/OpenGL/OpenGLRenderDevice.h"
#include "Daydream/Graphics/API/DirectX11/D3D11RenderDevice.h"

namespace Daydream
{
	namespace
	{
		static bool isGLFWInitialized = false;
		static int glfwCount = 0;

		static void GLFWErrorCallback(int _error, const char* _description)
		{
			DAYDREAM_CORE_ERROR("GLFW Error ({0}): {1}", _error, _description);
		}
	}

	WindowsWindow::WindowsWindow(const WindowDesc& _desc)
	{
		desc = _desc;
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		windowData.title = desc.title;
		windowData.width = desc.width;
		windowData.height = desc.height;
		windowData.keyStates.resize(GLFW_KEY_LAST + 1, 0);
		windowData.keyDownChecker.resize(GLFW_KEY_LAST + 1, 0);
		windowData.mouseStates.resize(GLFW_MOUSE_BUTTON_LAST + 1, 0);
		windowData.mouseDownChecker.resize(GLFW_MOUSE_BUTTON_LAST + 1, 0);

		//std::wstring title(windowData.title.begin(), windowData.title.end());

		DAYDREAM_CORE_INFO("Create Window {0} ({1}, {2})", desc.title, desc.width, desc.height);

		if (false == isGLFWInitialized)
		{
			Int32 success = glfwInit();
			DAYDREAM_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			isGLFWInitialized = true;
		}

		//glfwWindowHint(GLFW_TITLEBAR, false);

		if (!desc.useDefaultClientAPI) 
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		glfwWindow = glfwCreateWindow((Int32)desc.width, (Int32)desc.height, desc.title.c_str(), nullptr, nullptr);
		glfwCount++;

		if (desc.useDefaultClientAPI)
		{
			glfwMakeContextCurrent(glfwWindow);
		}

		//MakeContextCurrent();
		
		glfwSetWindowUserPointer(glfwWindow, &windowData);

		windowHandle = glfwGetWin32Window(glfwWindow);

		glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* _window, int _width, int _height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				data.width = _width;
				data.height = _height;
				WindowResizeEvent event(data.title, _width, _height);
				data.eventCallbackFn(event);
			});
		glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* _window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				WindowCloseEvent event;
				data.eventCallbackFn(event);
			});

		glfwSetKeyCallback(glfwWindow, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				switch (_action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(_key, 0);
					data.eventCallbackFn(event);
					data.keyStates[_key] = DAYDREAM_PRESS;
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(_key);
					data.eventCallbackFn(event);
					data.keyStates[_key] = DAYDREAM_RELEASE;
					data.keyDownChecker[_key] = false;
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(_key, 1);
					data.eventCallbackFn(event);
					data.keyStates[_key] = DAYDREAM_REPEAT;
					break;
				}
				}
			});
		glfwSetCharCallback(glfwWindow, [](GLFWwindow* _window, UInt32 _keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);

				KeyTypedEvent event(_keycode);
				data.eventCallbackFn(event);
			});

		glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* _window, int _button, int _action, int _mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);

				switch (_action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(_button);
					data.mouseStates[_button] = DAYDREAM_PRESS;
					data.eventCallbackFn(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(_button);
					data.mouseStates[_button] = DAYDREAM_RELEASE;
					data.mouseDownChecker[_button] = false;
					data.eventCallbackFn(event);
					break;
				}
				}
			});
		glfwSetScrollCallback(glfwWindow, [](GLFWwindow* _window, double _xOffset, double _yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				MouseScrolledEvent event((float)_xOffset, (float)_yOffset);
				data.eventCallbackFn(event);
			});
		glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* _window, double _xPos, double _yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				MouseMovedEvent event((float)_xPos, (float)_yPos);
				data.eventCallbackFn(event);
			});

		glfwSetWindowFocusCallback(glfwWindow, [](GLFWwindow* _window, int _isFocused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(_window);
				WindowFocusEvent event(_isFocused, data.title);
				data.eventCallbackFn(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(glfwWindow);
		glfwCount--;
		if (glfwCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void WindowsWindow::SetVSync(bool _enabled)
	{
		//glfwSwapInterval((Int32)_enabled);
		windowData.isVSync = _enabled;
	}
	bool WindowsWindow::IsVSync() const
	{
		return windowData.isVSync;
	}
	void WindowsWindow::OnUpdateInputState()
	{
		for (int i = 0; i < GLFW_KEY_LAST; i++)
		{
			if (windowData.keyStates[i] == DAYDREAM_RELEASE)
			{
				windowData.keyStates[i] = DAYDREAM_IDLE;
			}
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
		{
			if (windowData.mouseStates[i] == DAYDREAM_RELEASE)
			{
				windowData.mouseStates[i] = DAYDREAM_IDLE;
			}
		}
	}
	void WindowsWindow::MakeContextCurrent() const
	{
		glfwMakeContextCurrent(glfwWindow);
	}
	void WindowsWindow::ReleaseContext() const
	{


		glfwMakeContextCurrent(nullptr);
	}
}
