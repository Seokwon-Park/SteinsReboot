#include "DaydreamPCH.h"
#include "RenderGraphPanel.h"

namespace Daydream
{
    namespace
    {
        void ImGuiEx_BeginColumn()
        {
            ImGui::BeginGroup();
        }

        void ImGuiEx_NextColumn()
        {
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
        }

        void ImGuiEx_EndColumn()
        {
            ImGui::EndGroup();
        }
    }
	RenderGraphPanel::RenderGraphPanel()
	{
        nodeEditor = MakeUnique<ImGuiNodeEditor>();
        nodeEditor->Init();
        //ImNode::NavigateToContent();

        
	}

	RenderGraphPanel::~RenderGraphPanel()
	{
	}
	void RenderGraphPanel::OnImGuiRender()
	{
        ImGuiWindowClass rgClass;
        rgClass.ClassId = ImGui::GetID("RenderGraphFamily");
        ImGui::Begin("Render Graph Editor");
        ImGuiID dockspace_id = ImGui::GetID("RGDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_None, &rgClass);
        ImGui::End();

        ImGui::SetNextWindowClass(&rgClass); 
        ImGui::Begin("Pass List");
        ImGui::End();

        ImGui::SetNextWindowClass(&rgClass); 
        nodeEditor->OnImGuiRender();
	}
}

