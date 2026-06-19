#pragma once

#if defined(DAYDREAM_PLATFORM_WINDOWS)

#include "Daydream/Core/Window.h"
#include "Daydream/Graphics/Core/RenderDevice.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace Daydream
{
	class WindowsWindow : public DaydreamWindow
	{
	public:
		// constrcuter destructer
		WindowsWindow(const WindowDesc& _desc);
		virtual ~WindowsWindow() override;

		void OnUpdate() override;

		inline UInt32 GetWidth() const override { return windowData.width; }
		inline UInt32 GetHeight() const override { return windowData.height; }

		inline virtual void SetEventCallback(const EventCallbackFn& _callbackFn) override { windowData.eventCallbackFn = _callbackFn; }
		virtual void SetVSync(bool _enabled) override;
		virtual bool IsVSync() const override;

		inline virtual int GetKeyState(int _key) const override { return windowData.keyStates[_key]; }
		inline virtual int GetMouseState(int _button) const override { return windowData.mouseStates[_button]; }

		inline Pair<Float32, Float32> GetMousePos() const override
		{
			double xpos, ypos;
			glfwGetCursorPos(glfwWindow, &xpos, &ypos);
			return MakePair<Float32, Float32>(static_cast<Float32>(xpos), static_cast<Float32>(ypos));
		}
		void* GetNativeWindow() const override { return glfwWindow; }

		inline void SetKeyState(int _key, int _state)override { windowData.keyStates[_key] = _state; }
		//// 이 함수는 window에서 releas
		inline bool GetIsKeyDown(int _key) const override { return windowData.keyDownChecker[_key]; }
		inline void SetKeyDown(int _key) override { windowData.keyDownChecker[_key] = true; }

		inline virtual bool GetIsMouseDown(int _key) const override { return windowData.mouseDownChecker[_key]; }
		inline virtual void SetMouseDown(int _key) override { windowData.mouseDownChecker[_key] = true; }
		virtual void OnUpdateInputState() override;

	private:
		void Init();
		void Shutdown();

		GLFWwindow* glfwWindow = nullptr;

		HWND windowHandle = nullptr;

		struct WindowData
		{
			std::string title;
			UInt32 width = 0;
			UInt32 height = 0;
			bool isVSync;
			EventCallbackFn eventCallbackFn;
			Array<UInt16> keyStates;
			Array<UInt16> mouseStates;
			Array<bool> keyDownChecker;
			Array<bool> mouseDownChecker;
		};
		WindowData windowData;
	};
}

#endif

