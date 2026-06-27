#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace ImNode = ax::NodeEditor;

namespace Daydream
{
	struct GraphNodeBuilder
	{
		GraphNodeBuilder(ImTextureRef _texture = nullptr, int _textureWidth = 0, int _textureHeight = 0);

		void Begin(ImNode::NodeId _id);
		void End();

		void Header(const ImVec4& _color = ImVec4(1, 1, 1, 1));
		void EndHeader();

		void Input(ImNode::PinId _id);
		void EndInput();

		void Middle();

		void Output(ImNode::PinId _id);
		void EndOutput();

	private:
		enum class Stage
		{
			Invalid,
			Begin,
			Header,
			Content,
			Input,
			Output,
			Middle,
			End
		};

		bool SetStage(Stage _stage);

		void Pin(ImNode::PinId _id, ImNode::PinKind _kind);
		void EndPin();

		ImTextureRef headerTextureId;
		Int32         headerTextureWidth;
		Int32         headerTextureHeight;
		ImNode::NodeId      currentNodeId;
		Stage       currentStage;
		UInt32       headerColor;
		ImVec2      nodeMin;
		ImVec2      nodeMax;
		ImVec2      headerMin;
		ImVec2      headerMax;
		ImVec2      contentMin;
		ImVec2      contentMax;
		bool        hasHeader;
	};

	enum class IconType : ImU32 
	{
		Flow,
		Circle, 
		Square, 
		Grid,
		RoundSquare, 
		Diamond
	};

	void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));
	void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);
}
