#pragma once

#include "WindowDefine.h"
#include "Daydream/Event/Event.h"
#include "Daydream/Graphics/Core/Swapchain.h"

namespace Daydream
{
	struct WindowDesc
	{
		String title;
		UInt32 width;
		UInt32 height;
		//if use OpenGL, this flag must be true.
		Bool useDefaultClientAPI = false;

		WindowDesc(const std::string& _title = "Daydream Engine",
			UInt32 _width = 1280,
			UInt32 _height = 720)
			:title(_title), width(_width), height(_height), useDefaultClientAPI(false)
		{

		}
	};

	// why not glfw -> external .exe don't know about glfw3.h
	// Interface represending a desktop system based Window
	class DaydreamWindow
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		DaydreamWindow() {};
		virtual ~DaydreamWindow() {}

		virtual void OnUpdate() = 0;

		virtual UInt32 GetWidth() const = 0;
		virtual UInt32 GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& _callbackFn) = 0;
		virtual void SetVSync(bool _enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual int GetKeyState(int _key) const = 0;
		virtual int GetMouseState(int _mouse) const = 0;
		virtual Pair<Float32, Float32> GetMousePos() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual void SetKeyState(int _key, int _state) = 0;
		virtual bool GetIsKeyDown(int _key) const = 0;
		virtual void SetKeyDown(int _key) = 0;
		virtual bool GetIsMouseDown(int _key) const = 0;
		virtual void SetMouseDown(int _key) = 0;
		virtual void OnUpdateInputState() = 0;

		static Unique<DaydreamWindow> Create(const WindowDesc& _desc = WindowDesc());
	protected:
		WindowDesc desc;
	private:
	};


}

