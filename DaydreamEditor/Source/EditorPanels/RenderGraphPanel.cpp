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
		ImNode::Config config;
		config.SettingsFile = "Simple.json";
		context = ImNode::CreateEditor(&config);
	}

	RenderGraphPanel::~RenderGraphPanel()
	{
		ImNode::DestroyEditor(context);
	}
	void RenderGraphPanel::OnImGuiRender()
	{
        int uniqueId = 1;

        ImGui::Begin("simple node editor");

        ImNode::SetCurrentEditor(context);
        ImNode::Begin("My Editor", ImVec2(0.0, 0.0f));
        ImNode::NodeId nodeA_Id = uniqueId++;
        ImNode::PinId  nodeA_InputPinId = uniqueId++;
        ImNode::PinId  nodeA_OutputPinId = uniqueId++;


        if (isFirstFrame)
            ImNode::SetNodePosition(nodeA_Id, ImVec2(10, 10));
        ImNode::BeginNode(nodeA_Id);
        ImGui::Text("Node A");
        ImNode::BeginPin(nodeA_InputPinId, ImNode::PinKind::Input);
        ImGui::Text("-> In");
        ImNode::EndPin();
        ImGui::SameLine();
        ImNode::BeginPin(nodeA_OutputPinId, ImNode::PinKind::Output);
        ImGui::Text("Out ->");
        ImNode::EndPin();
        ImNode::EndNode();

        // Submit Node B
        ImNode::NodeId nodeB_Id = uniqueId++;
        ImNode::PinId  nodeB_InputPinId1 = uniqueId++;
        ImNode::PinId  nodeB_InputPinId2 = uniqueId++;
        ImNode::PinId  nodeB_OutputPinId = uniqueId++;

        if (isFirstFrame)
            ImNode::SetNodePosition(nodeB_Id, ImVec2(210, 60));
        ImNode::BeginNode(nodeB_Id);
        ImGui::Text("Node B");
        ImGuiEx_BeginColumn();
        ImNode::BeginPin(nodeB_InputPinId1, ImNode::PinKind::Input);
        ImGui::Text("-> In1");
        ImNode::EndPin();
        ImNode::BeginPin(nodeB_InputPinId2, ImNode::PinKind::Input);
        ImGui::Text("-> In2");
        ImNode::EndPin();
        ImGuiEx_NextColumn();
        ImNode::BeginPin(nodeB_OutputPinId, ImNode::PinKind::Output);
        ImGui::Text("Out ->");
        ImNode::EndPin();
        ImGuiEx_EndColumn();
        ImNode::EndNode();


        for (auto& linkInfo : links)
            ImNode::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);

        if (ImNode::BeginCreate())
        {
            ImNode::PinId inputPinId, outputPinId;
            if (ImNode::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // ImNode::AcceptNewItem() return true when user release mouse button.
                    if (ImNode::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        links.push_back({ ImNode::LinkId(nextLinkId++), inputPinId, outputPinId });

                        // Draw new link.
                        ImNode::Link(links.back().Id, links.back().InputId, links.back().OutputId);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling ImNode::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        ImNode::EndCreate(); // Wraps up object creation action handling.


        // Handle deletion action
        if (ImNode::BeginDelete())
        {
            // There may be many links marked for deletion, let's loop over them.
            ImNode::LinkId deletedLinkId;
            while (ImNode::QueryDeletedLink(&deletedLinkId))
            {
                // If you agree that link can be deleted, accept deletion.
                if (ImNode::AcceptDeletedItem())
                {
                    // Then remove link from your data.
                    for (auto& link : links)
                    {
                        if (link.Id == deletedLinkId)
                        {
                            links.erase(&link);
                            break;
                        }
                    }
                }

                // You may reject link deletion by calling:
                // ImNode::RejectDeletedItem();
            }
        }
        ImNode::EndDelete(); // Wrap up deletion action

        ImNode::End();

        if (isFirstFrame)
            ImNode::NavigateToContent(0.0f);
        ImNode::SetCurrentEditor(nullptr);

        isFirstFrame = false;

        ImGui::End();
	}
}

