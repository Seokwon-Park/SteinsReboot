#include "DaydreamPCH.h"
#include "ImGuiNodeEditor.h"
#include "ImGuiNodeUtility.h" 

namespace Daydream
{
	ImGuiNodeEditor::ImGuiNodeEditor()
	{
	}

	ImGuiNodeEditor::~ImGuiNodeEditor()
	{
		ImNode::DestroyEditor(editor);
	}

	void ImGuiNodeEditor::Init(FunctionPtr<void()> _contextCallback)
	{
		contextMenuCallback = _contextCallback;

		config.SettingsFile = "Simple.json";
		config.UserPointer = this;

		config.LoadNodeSettings = [](ImNode::NodeId _nodeId, char* _data, void* _userPointer) -> size_t
			{
				//auto self = static_cast<RenderGraphPanel*>(_userPointer);

				//auto node = self->FindNode(_nodeId);
				//if (!node)
				//    return 0;

				//if (data != nullptr)
				//    memcpy(data, node->State.data(), node->State.size());
				//return node->State.size();
				return 0;
			};

		config.SaveNodeSettings = [](ImNode::NodeId _nodeId, const char* _data, size_t size, ImNode::SaveReasonFlags _reason, void* _userPointer) -> bool
			{
				//auto self = static_cast<Example*>(userPointer);

				//auto node = self->FindNode(nodeId);
				//if (!node)
				//    return false;

				//node->State.assign(data, size);

				//self->TouchNode(nodeId);

				return true;
			};

		editor = ImNode::CreateEditor(&config);
		ImNode::SetCurrentEditor(editor);

		headerBackground = AssetManager::GetAssetByPath<Texture2D>("Asset/Texture/BlueprintBackground.png");


		nodes.push_back(MakeUnique<Node>(GetNextId(), "Branch"));
		nodes.back()->inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
		nodes.back()->outputs.emplace_back(GetNextId(), "True", PinType::Flow);
		nodes.back()->outputs.emplace_back(GetNextId(), "False", PinType::Flow);

		BuildNode(nodes.back().get());
	}
	void ImGuiNodeEditor::OnImGuiRender()
	{
		if (styleEditorEnable)
		{
			ShowStyleEditor(&styleEditorEnable);
		}

		ImGui::Begin("Node Editor Window");
		{
			ImNode::Begin("Node editor");
			auto cursorTopLeft = ImGui::GetCursorScreenPos();

			GraphNodeBuilder builder(headerBackground->GetDefaultSRV()->GetUIHandle(), headerBackground->GetWidth(), headerBackground->GetHeight());

			for (auto& node : nodes)
			{
				if (node->type != NodeType::Blueprint && node->type != NodeType::Simple)
					continue;

				const auto isSimple = node->type == NodeType::Simple;

				bool hasOutputDelegates = false;
				for (auto& output : node->outputs)
					if (output.type == PinType::Delegate)
						hasOutputDelegates = true;

				builder.Begin(node->id);
				if (!isSimple)
				{
					builder.Header(node->color);
					ImGui::Spring(0);
					ImGui::TextUnformatted(node->name.c_str());
					ImGui::Spring(1);
					ImGui::Dummy(ImVec2(0, 28));
					if (hasOutputDelegates)
					{
						ImGui::BeginVertical("delegates", ImVec2(0, 28));
						ImGui::Spring(1, 0);
						for (auto& output : node->outputs)
						{
							if (output.type != PinType::Delegate)
								continue;

							auto alpha = ImGui::GetStyle().Alpha;
							if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
								alpha = alpha * (48.0f / 255.0f);

							ImNode::BeginPin(output.id, ImNode::PinKind::Output);
							ImNode::PinPivotAlignment(ImVec2(1.0f, 0.5f));
							ImNode::PinPivotSize(ImVec2(0, 0));
							ImGui::BeginHorizontal(output.id.AsPointer());
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
							if (!output.name.empty())
							{
								ImGui::TextUnformatted(output.name.c_str());
								ImGui::Spring(0);
							}
							DrawPinIcon(output, IsPinLinked(output.id), (int)(alpha * 255));
							ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
							ImGui::EndHorizontal();
							ImGui::PopStyleVar();
							ImNode::EndPin();

							//DrawItemRect(ImColor(255, 0, 0));
						}
						ImGui::Spring(1, 0);
						ImGui::EndVertical();
						ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
					}
					else
						ImGui::Spring(0);
					builder.EndHeader();
				}

				for (auto& input : node->inputs)
				{
					auto alpha = ImGui::GetStyle().Alpha;
					if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
						alpha = alpha * (48.0f / 255.0f);

					builder.Input(input.id);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
					DrawPinIcon(input, IsPinLinked(input.id), (int)(alpha * 255));
					ImGui::Spring(0);
					if (!input.name.empty())
					{
						ImGui::TextUnformatted(input.name.c_str());
						ImGui::Spring(0);
					}
					if (input.type == PinType::Bool)
					{
						ImGui::Button("Hello");
						ImGui::Spring(0);
					}
					ImGui::PopStyleVar();
					builder.EndInput();
				}

				if (isSimple)
				{
					builder.Middle();

					ImGui::Spring(1, 0);
					ImGui::TextUnformatted(node->name.c_str());
					ImGui::Spring(1, 0);
				}

				for (auto& output : node->outputs)
				{
					if (!isSimple && output.type == PinType::Delegate)
						continue;

					auto alpha = ImGui::GetStyle().Alpha;
					if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
						alpha = alpha * (48.0f / 255.0f);

					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
					builder.Output(output.id);
					if (output.type == PinType::String)
					{
						static char buffer[128] = "Edit Me\nMultiline!";
						static bool wasActive = false;

						ImGui::PushItemWidth(100.0f);
						ImGui::InputText("##edit", buffer, 127);
						ImGui::PopItemWidth();
						if (ImGui::IsItemActive() && !wasActive)
						{
							ImNode::EnableShortcuts(false);
							wasActive = true;
						}
						else if (!ImGui::IsItemActive() && wasActive)
						{
							ImNode::EnableShortcuts(true);
							wasActive = false;
						}
						ImGui::Spring(0);
					}
					if (!output.name.empty())
					{
						ImGui::Spring(0);
						ImGui::TextUnformatted(output.name.c_str());
					}
					ImGui::Spring(0);
					DrawPinIcon(output, IsPinLinked(output.id), (int)(alpha * 255));
					ImGui::PopStyleVar();
					builder.EndOutput();
				}

				builder.End();
			}
			ImNode::Suspend();

			if (ImNode::ShowBackgroundContextMenu())
			{
				ImGui::OpenPopup("EditorContextMenu");
			}
			ImNode::Resume();

			ImNode::Suspend();

			if (ImGui::BeginPopup("EditorContextMenu"))
			{
				if (contextMenuCallback)
					contextMenuCallback();

				ImGui::EndPopup();
			}
			ImNode::Resume();


			ImNode::End();
		}
		ImGui::End();

	}

	void ImGuiNodeEditor::AddNode(int _id, const char* _name, ImColor _color)
	{
		nodes.push_back(MakeUnique<Node>(_id, _name, _color));
	}

	void ImGuiNodeEditor::ShowStyleEditor(bool* _show)
	{
		if (!ImGui::Begin("Style", _show))
		{
			ImGui::End();
			return;
		}

		auto paneWidth = ImGui::GetContentRegionAvail().x;

		auto& editorStyle = ImNode::GetStyle();
		ImGui::TextUnformatted("Values");
		if (ImGui::Button("Reset to defaults"))
			editorStyle = ImNode::Style();
		ImGui::Spacing();
		ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
		ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
		ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
		ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
		ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
		ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
		ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
		ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
		ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
		ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
		//ImVec2  SourceDirection;
		//ImVec2  TargetDirection;
		ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
		ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
		ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
		ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
		//ImVec2  PivotAlignment;
		//ImVec2  PivotSize;
		//ImVec2  PivotScale;
		//float   PinCorners;
		//float   PinRadius;
		//float   PinArrowSize;
		//float   PinArrowWidth;
		ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
		ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

		ImGui::Separator();

		static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
		ImGui::TextUnformatted("Filter Colors");
		ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
		ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
		ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);

		static ImGuiTextFilter filter;
		filter.Draw("##filter", paneWidth);

		ImGui::Spacing();

		ImGui::PushItemWidth(-160);
		for (int i = 0; i < ImNode::StyleColor_Count; ++i)
		{
			auto name = ImNode::GetStyleColorName((ImNode::StyleColor)i);
			if (!filter.PassFilter(name))
				continue;

			ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
		}
		ImGui::PopItemWidth();

		ImGui::End();
	}
	void ImGuiNodeEditor::DrawPinIcon(const Pin& _pin, bool _connected, int _alpha)
	{
		IconType iconType;
		ImColor  color = GetIconColor(_pin.type);
		color.Value.w = _alpha / 255.0f;
		switch (_pin.type)
		{
		case PinType::Flow:     iconType = IconType::Flow;   break;
		case PinType::Bool:     iconType = IconType::Circle; break;
		case PinType::Int:      iconType = IconType::Circle; break;
		case PinType::Float:    iconType = IconType::Circle; break;
		case PinType::String:   iconType = IconType::Circle; break;
		case PinType::Object:   iconType = IconType::Circle; break;
		case PinType::Function: iconType = IconType::Circle; break;
		case PinType::Delegate: iconType = IconType::Square; break;
		default:
			return;
		}

		Icon(ImVec2(static_cast<float>(pinIconSize), static_cast<float>(pinIconSize)), iconType, _connected, color, ImColor(32, 32, 32, _alpha));
	};
}

