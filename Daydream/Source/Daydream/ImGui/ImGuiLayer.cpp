#include "DaydreamPCH.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "backends/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>

#include "Daydream/Core/Application.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Asset/AssetManager.h"

namespace Daydream
{
	namespace
	{
		ImDrawData* CloneDrawData(const ::ImDrawData* _srcDrawData)
		{
			if (_srcDrawData == nullptr)
			{
				return nullptr;
			}

			::ImDrawData* clonedDrawData = IM_NEW(ImDrawData)(*_srcDrawData);
			clonedDrawData->CmdLists = ImVector<ImDrawList*>();
			clonedDrawData->CmdLists.reserve(_srcDrawData->CmdListsCount);
			for (int i = 0; i < _srcDrawData->CmdListsCount; ++i)
			{
				clonedDrawData->CmdLists.push_back(_srcDrawData->CmdLists[i]->CloneOutput());
			}
			clonedDrawData->CmdListsCount = clonedDrawData->CmdLists.Size;

			return clonedDrawData;
		}


		void DestroyClonedDrawData(ImDrawData* _drawData)
		{
			if (_drawData == nullptr)
			{
				return;
			}

			for (int i = 0; i < _drawData->CmdListsCount; ++i)
			{
				IM_DELETE(_drawData->CmdLists[i]);
			}
			//IM_FREE(_drawData->CmdLists.Data);
			IM_DELETE(_drawData);
		}
	}

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImFont* font1 = io.Fonts->AddFontFromFileTTF("Asset/Font/NotoSansKR-Regular.ttf", 20.0f);
		io.FontDefault = font1;

		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	ImGui::UpdatePlatformWindows();
		//	ImGui::RenderPlatformWindowsDefault();
		//}

		SetDarkThemeColors();

		Renderer::GetImGuiRenderer()->Init(&Application::GetInstance().GetMainWindow());

		//ImGui_ImplGlfw_InitForOpenGL(window, true);
		//ImGui_ImplOpenGL3_Init("#version 410");
	}
	void ImGuiLayer::OnDetach()
	{
	}
	void ImGuiLayer::OnEvent(Event& _event)
	{
		if (isBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			_event.handled |= _event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			_event.handled |= _event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
	void ImGuiLayer::BeginImGui()
	{
		Renderer::GetImGuiRenderer()->NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::EndImGui()
	{
		ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::GetInstance();
		//io.DisplaySize = ImVec2(static_cast<Float32>(app.GetMainWindow().GetWidth()), static_cast<Float32>(app.GetMainWindow().GetHeight()));

		ImGui::Render();
		ImDrawData* clonedDrawData = CloneDrawData(ImGui::GetDrawData());
		Renderer::EnqueueCommand([clonedDrawData]() {
			std::lock_guard<std::mutex> lock(imguiRenderMutex);
			Renderer::GetImGuiRenderer()->RenderDrawData(Renderer::GetActiveCommandList(), clonedDrawData);
			DestroyClonedDrawData(clonedDrawData);
			});
	}

	void ImGuiLayer::UpdateImGuiWindows()
	{
		ImGui::UpdatePlatformWindows();
		{
			// [메인 스레드] 렌더 스레드가 메인 뷰포트를 다 그릴 때까지 여기서 잠깐 대기!
			std::lock_guard<std::mutex> lock(imguiRenderMutex);
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}

