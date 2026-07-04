#pragma once

#include "Daydream.h"
#include "imgui_node_editor.h"

namespace ImNode = ax::NodeEditor;

namespace Daydream
{
	enum class PinType
	{
		Flow,
		Bool,
		Int,
		Float,
		String,
		Object,
		Function,
		Delegate,
	};

	enum class PinKind
	{
		Output,
		Input
	};

	enum class NodeType
	{
		Blueprint,
		Simple,
		Tree,
		Comment,
		Houdini
	};

	struct Node;

	struct Pin
	{
		ImNode::PinId id;
		Node* node;
		String name;
		PinType type;
		PinKind kind;

		Pin(int _id, const char* _name, PinType _type) :
			id(_id), node(nullptr), name(_name), type(_type), kind(PinKind::Input)
		{
		}
	};

	struct Node
	{
		ImNode::NodeId id;
		String name;
		Array<Pin> inputs;
		Array<Pin> outputs;
		ImColor color;
		NodeType type;
		ImVec2 size;

		std::string State;
		std::string SavedState;

		Node(int _id, const char* _name, ImColor _color = ImColor(255, 255, 255)) :
			id(_id), name(_name), color(_color), type(NodeType::Blueprint), size(0, 0)
		{
		}
	};

	struct Link
	{
		ImNode::LinkId id;

		ImNode::PinId startPinID;
		ImNode::PinId endPinID;

		ImColor color;

		Link(ImNode::LinkId _id, ImNode::PinId _startPinId, ImNode::PinId _endPinId) :
			id(_id), startPinID(_startPinId), endPinID(_endPinId), color(255, 255, 255)
		{
		}
	};

	struct NodeIdLess
	{
		bool operator()(const ImNode::NodeId& lhs, const ImNode::NodeId& rhs) const
		{
			return lhs.AsPointer() < rhs.AsPointer();
		}
	}; 

	class ImGuiNodeEditor
	{
	public:
		ImGuiNodeEditor();
		~ImGuiNodeEditor();

		void Init(FunctionPtr<void()> _contextCallback);
		void OnImGuiRender();

		void AddNode(int _id, const char* _name, ImColor _color = ImColor(255, 255, 255));
	protected:

	private:
		void ShowStyleEditor(bool* _show = nullptr);

		Node* FindNode(ImNode::NodeId _id)
		{
			for (auto& node : nodes)
				if (node->id == _id)
					return node.get();

			return nullptr;
		}

		Link* FindLink(ImNode::LinkId _id)
		{
			for (auto& link : links)
				if (link.id == _id)
					return &link;

			return nullptr;
		}

		Pin* FindPin(ImNode::PinId _id)
		{
			if (!_id)
				return nullptr;

			for (auto& node : nodes)
			{
				for (auto& pin : node->inputs)
					if (pin.id == _id)
						return &pin;

				for (auto& pin : node->outputs)
					if (pin.id == _id)
						return &pin;
			}

			return nullptr;
		}

		bool IsPinLinked(ImNode::PinId _id)
		{
			if (!_id)
				return false;

			for (auto& link : links)
				if (link.startPinID == _id || link.endPinID == _id)
					return true;

			return false;
		}

		bool CanCreateLink(Pin* a, Pin* b)
		{
			if (!a || !b || a == b || a->kind == b->kind || a->type != b->type || a->node == b->node)
				return false;

			return true;
		}


		ImColor GetIconColor(PinType type)
		{
			switch (type)
			{
			default:
			case PinType::Flow:     return ImColor(255, 255, 255);
			case PinType::Bool:     return ImColor(220, 48, 48);
			case PinType::Int:      return ImColor(68, 201, 156);
			case PinType::Float:    return ImColor(147, 226, 74);
			case PinType::String:   return ImColor(124, 21, 153);
			case PinType::Object:   return ImColor(51, 150, 215);
			case PinType::Function: return ImColor(218, 0, 183);
			case PinType::Delegate: return ImColor(255, 48, 48);
			}
		};

		void BuildNode(Node* node)
		{
			for (auto& input : node->inputs)
			{
				input.node = node;
				input.kind = PinKind::Input;
			}

			for (auto& output : node->outputs)
			{
				output.node = node;
				output.kind = PinKind::Output;
			}
		}

		int GetNextId()
		{
			return nextId++;
		}

		ImNode::LinkId GetNextLinkId()
		{
			return ImNode::LinkId(GetNextId());
		}

		void DrawPinIcon(const Pin& _pin, bool _connected, int _alpha);

		FunctionPtr<void()> contextMenuCallback;

		bool styleEditorEnable = true; 

		ImNode::Config config;

		int nextId = 1;
		const int pinIconSize = 24;
		Array<Unique<Node>> nodes;
		Array<Link> links;
		Texture2D* headerBackground = nullptr;
		ImTextureRef saveIcon = nullptr;
		ImTextureRef restoreIcon = nullptr;
		const float touchTime = 1.0f;
		SortedMap<ImNode::NodeId, float, NodeIdLess> nodeTouchTime;
		bool showOrdinals = false;

		ImNode::NodeId contextNodeId = 0;
		ImNode::LinkId contextLinkId = 0;
		ImNode::PinId  contextPinId = 0;
		bool createNewNode = false;
		Pin* newNodeLinkPin = nullptr;
		Pin* newLinkPin = nullptr;

		ImNode::EditorContext* editor = nullptr;
	};
}
