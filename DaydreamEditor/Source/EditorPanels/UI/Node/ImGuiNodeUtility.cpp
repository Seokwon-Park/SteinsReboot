#include "DaydreamPCH.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGuiNodeUtility.h"
#include "imgui_internal.h"

namespace Daydream
{
	GraphNodeBuilder::GraphNodeBuilder(ImTextureRef _texture, int _textureWidth, int _textureHeight)
		:headerTextureId(_texture),
		headerTextureWidth(_textureWidth),
		headerTextureHeight(_textureHeight),
		currentNodeId(0),
		currentStage(Stage::Invalid),
		hasHeader(false)
	{

	}

	void GraphNodeBuilder::Begin(ImNode::NodeId id)
	{
        //이전 노드를 그렸던 잔재를 지우고 상태를 초기화
		hasHeader = false;
		headerMin = headerMax = ImVec2();

		ImNode::PushStyleVar(ImNode::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
            
        // 노드 선언
		ImNode::BeginNode(id);

		ImGui::PushID(id.AsPointer());
		currentNodeId = id;

		SetStage(Stage::Begin);
	}

	void GraphNodeBuilder::End()
	{
		SetStage(Stage::End);

        // 노드의 최종 '전체 크기(Bounding Box)를 계산해서 확정
		ImNode::EndNode();

		if (ImGui::IsItemVisible())
		{
            Int32 alpha = static_cast<Int32>(255 * ImGui::GetStyle().Alpha);

			ImDrawList* drawList = ImNode::GetNodeBackgroundDrawList(currentNodeId);
			const Float32 halfBorderWidth = ImNode::GetStyle().NodeBorderWidth * 0.5f;

			UInt32 newheaderColor = IM_COL32(0, 0, 0, alpha) | (headerColor & IM_COL32(255, 255, 255, 0));
			if ((headerMax.x > headerMin.x) && (headerMax.y > headerMin.y) && headerTextureId.GetTexID())
			{
                //텍스처(이미지)가 노드 너비에 맞게 늘어나거나 잘리도록 UV 텍스처 좌표를 수학적으로 계산
				const auto uv = ImVec2(
					(headerMax.x - headerMin.x) / (float)(4.0f * headerTextureWidth),
					(headerMax.y - headerMin.y) / (float)(4.0f * headerTextureHeight));

				drawList->AddImageRounded(headerTextureId,
					headerMin - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
					headerMax + ImVec2(8 - halfBorderWidth, 0),
					ImVec2(0.0f, 0.0f), uv,
					newheaderColor, ImNode::GetStyle().NodeRounding, ImDrawFlags_RoundCornersTop);

				if (contentMin.y > headerMax.y)
				{
					drawList->AddLine(
						ImVec2(headerMin.x - (8 - halfBorderWidth), headerMax.y - 0.5f),
						ImVec2(headerMax.x + (8 - halfBorderWidth), headerMax.y - 0.5f),
						ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
				}
			}
		}

		currentNodeId = 0;
		ImGui::PopID();
		ImNode::PopStyleVar();
		SetStage(Stage::Invalid);
	}

    void GraphNodeBuilder::Header(const ImVec4& _color)
    {
        headerColor = ImColor(_color);
        SetStage(Stage::Header);
    }

    void GraphNodeBuilder::EndHeader()
    {
        SetStage(Stage::Content);
    }

    void GraphNodeBuilder::Input(ImNode::PinId id)
    {
        if (currentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (currentStage == Stage::Input);

        SetStage(Stage::Input);

        if (applyPadding)
            ImGui::Spring(0);

        Pin(id, ImNode::PinKind::Input);

        ImGui::BeginHorizontal(id.AsPointer());
    }

    void GraphNodeBuilder::EndInput()
    {
        ImGui::EndHorizontal();

        EndPin();
    }

    void GraphNodeBuilder::Middle()
    {
        if (currentStage == Stage::Begin)
            SetStage(Stage::Content);

        SetStage(Stage::Middle);
    }

    void GraphNodeBuilder::Output(ImNode::PinId id)
    {
        if (currentStage == Stage::Begin)
            SetStage(Stage::Content);

        const auto applyPadding = (currentStage == Stage::Output);

        SetStage(Stage::Output);

        if (applyPadding)
            ImGui::Spring(0);

        Pin(id, ImNode::PinKind::Output);

        ImGui::BeginHorizontal(id.AsPointer());
    }

    void GraphNodeBuilder::EndOutput()
    {
        ImGui::EndHorizontal();

        EndPin();
    }

	bool GraphNodeBuilder::SetStage(Stage _stage)
    {
        if (_stage == currentStage)
            return false;

        auto oldStage = currentStage;
        currentStage = _stage;

        ImVec2 cursor;
        switch (oldStage)
        {
        case Stage::Begin:
            break;

        case Stage::Header:
            ImGui::EndHorizontal();
            headerMin = ImGui::GetItemRectMin();
            headerMax = ImGui::GetItemRectMax();

            // spacing between header and content
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

            break;

        case Stage::Content:
            break;

        case Stage::Input:
            ImNode::PopStyleVar(2);

            ImGui::Spring(1, 0);
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Middle:
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::Output:
            ImNode::PopStyleVar(2);

            ImGui::Spring(1, 0);
            ImGui::EndVertical();

            // #debug
            // ImGui::GetWindowDrawList()->AddRect(
            //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

            break;

        case Stage::End:
            break;

        case Stage::Invalid:
            break;
        }

        switch (_stage)
        {
        case Stage::Begin:
            ImGui::BeginVertical("node");
            break;

        case Stage::Header:
            hasHeader = true;

            ImGui::BeginHorizontal("header");
            break;

        case Stage::Content:
            if (oldStage == Stage::Begin)
                ImGui::Spring(0);

            ImGui::BeginHorizontal("content");
            ImGui::Spring(0, 0);
            break;

        case Stage::Input:
            ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

            ImNode::PushStyleVar(ImNode::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
            ImNode::PushStyleVar(ImNode::StyleVar_PivotSize, ImVec2(0, 0));

            if (!hasHeader)
                ImGui::Spring(1, 0);
            break;

        case Stage::Middle:
            ImGui::Spring(1);
            ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
            break;

        case Stage::Output:
            if (oldStage == Stage::Middle || oldStage == Stage::Input)
                ImGui::Spring(1);
            else
                ImGui::Spring(1, 0);
            ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

            ImNode::PushStyleVar(ImNode::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
            ImNode::PushStyleVar(ImNode::StyleVar_PivotSize, ImVec2(0, 0));

            if (!hasHeader)
                ImGui::Spring(1, 0);
            break;

        case Stage::End:
            if (oldStage == Stage::Input)
                ImGui::Spring(1, 0);
            if (oldStage != Stage::Begin)
                ImGui::EndHorizontal();
            contentMin = ImGui::GetItemRectMin();
            contentMax = ImGui::GetItemRectMax();

            //ImGui::Spring(0);
            ImGui::EndVertical();
            nodeMin = ImGui::GetItemRectMin();
            nodeMax = ImGui::GetItemRectMax();
            break;

        case Stage::Invalid:
            break;
        }

        return true;
    }

    void GraphNodeBuilder::Pin(ImNode::PinId _id, ImNode::PinKind _kind)
    {
        ImNode::BeginPin(_id, _kind);

    }

    void GraphNodeBuilder::EndPin()
    {
        ImNode::EndPin();
    }

    void Icon(const ImVec2& _size, IconType _type, bool _filled, const ImVec4& _color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& _innerColor/* = ImVec4(0, 0, 0, 0)*/)
    {
        if (ImGui::IsRectVisible(_size))
        {
            auto cursorPos = ImGui::GetCursorScreenPos();
            auto drawList = ImGui::GetWindowDrawList();
            DrawIcon(drawList, cursorPos, cursorPos + _size, _type, _filled, ImColor(_color), ImColor(_innerColor));
        }

        ImGui::Dummy(_size);
    }

    void DrawIcon(ImDrawList* _drawList, const ImVec2& _a, const ImVec2& _b, IconType _type, bool _filled, ImU32 _color, ImU32 _innerColor)
    {
        auto rect = ImRect(_a, _b);
        auto rect_x = rect.Min.x;
        auto rect_y = rect.Min.y;
        auto rect_w = rect.Max.x - rect.Min.x;
        auto rect_h = rect.Max.y - rect.Min.y;
        auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
        auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
        auto rect_center = ImVec2(rect_center_x, rect_center_y);
        const auto outline_scale = rect_w / 24.0f;
        const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle
        if (_type == IconType::Flow)
        {
            const auto origin_scale = rect_w / 24.0f;
            const auto offset_x = 1.0f * origin_scale;
            const auto offset_y = 0.0f * origin_scale;
            const auto margin = (_filled ? 2.0f : 2.0f) * origin_scale;
            const auto rounding = 0.1f * origin_scale;
            const auto tip_round = 0.7f; // percentage of triangle edge (for tip)
            const auto canvas = ImRect(
                rect.Min.x + margin + offset_x,
                rect.Min.y + margin + offset_y,
                rect.Max.x - margin + offset_x,
                rect.Max.y - margin + offset_y);
            const auto canvas_x = canvas.Min.x;
            const auto canvas_y = canvas.Min.y;
            const auto canvas_w = canvas.Max.x - canvas.Min.x;
            const auto canvas_h = canvas.Max.y - canvas.Min.y;
            const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
            const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
            const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
            const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
            const auto center_y = (top + bottom) * 0.5f;
            const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
            const auto tip_right = ImVec2(right, center_y);
            const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);
            _drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
            _drawList->PathBezierCubicCurveTo(
                ImVec2(left, top),
                ImVec2(left, top),
                ImVec2(left, top) + ImVec2(rounding, 0));
            _drawList->PathLineTo(tip_top);
            _drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
            _drawList->PathBezierCubicCurveTo(
                tip_right,
                tip_right,
                tip_bottom + (tip_right - tip_bottom) * tip_round);
            _drawList->PathLineTo(tip_bottom);
            _drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
            _drawList->PathBezierCubicCurveTo(
                ImVec2(left, bottom),
                ImVec2(left, bottom),
                ImVec2(left, bottom) - ImVec2(0, rounding));
            if (!_filled)
            {
                if (_innerColor & 0xFF000000)
                    _drawList->AddConvexPolyFilled(_drawList->_Path.Data, _drawList->_Path.Size, _innerColor);
                _drawList->PathStroke(_color, ImDrawFlags_Closed, 2.0f * outline_scale);
            }
            else
                _drawList->PathFillConvex(_color);
        }
        else
        {
            auto triangleStart = rect_center_x + 0.32f * rect_w;
            auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);
            rect.Min.x += rect_offset;
            rect.Max.x += rect_offset;
            rect_x += rect_offset;
            rect_center_x += rect_offset * 0.5f;
            rect_center.x += rect_offset * 0.5f;
            if (_type == IconType::Circle)
            {
                const auto c = rect_center;
                if (!_filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                    if (_innerColor & 0xFF000000)
                        _drawList->AddCircleFilled(c, r, _innerColor, 12 + extra_segments);
                    _drawList->AddCircle(c, r, _color, 12 + extra_segments, 2.0f * outline_scale);
                }
                else
                {
                    _drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, _color, 12 + extra_segments);
                }
            }
            if (_type == IconType::Square)
            {
                if (_filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);
                    _drawList->AddRectFilled(p0, p1, _color, 0, ImDrawFlags_RoundCornersAll);
                }
                else
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);
                    if (_innerColor & 0xFF000000)
                        _drawList->AddRectFilled(p0, p1, _innerColor, 0, ImDrawFlags_RoundCornersAll);
                    _drawList->AddRect(p0, p1, _color, 0, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
                }
            }
            if (_type == IconType::Grid)
            {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto w = ceilf(r / 3.0f);
                const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
                const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));
                auto tl = baseTl;
                auto br = baseBr;
                for (int i = 0; i < 3; ++i)
                {
                    tl.x = baseTl.x;
                    br.x = baseBr.x;
                    _drawList->AddRectFilled(tl, br, _color);
                    tl.x += w * 2;
                    br.x += w * 2;
                    if (i != 1 || _filled)
                        _drawList->AddRectFilled(tl, br, _color);
                    tl.x += w * 2;
                    br.x += w * 2;
                    _drawList->AddRectFilled(tl, br, _color);
                    tl.y += w * 2;
                    br.y += w * 2;
                }
                triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
            }
            if (_type == IconType::RoundSquare)
            {
                if (_filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f;
                    const auto cr = r * 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);
                    _drawList->AddRectFilled(p0, p1, _color, cr, ImDrawFlags_RoundCornersAll);
                }
                else
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                    const auto cr = r * 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);
                    if (_innerColor & 0xFF000000)
                        _drawList->AddRectFilled(p0, p1, _innerColor, cr, ImDrawFlags_RoundCornersAll);
                    _drawList->AddRect(p0, p1, _color, cr, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
                }
            }
            else if (_type == IconType::Diamond)
            {
                if (_filled)
                {
                    const auto r = 0.607f * rect_w / 2.0f;
                    const auto c = rect_center;
                    _drawList->PathLineTo(c + ImVec2(0, -r));
                    _drawList->PathLineTo(c + ImVec2(r, 0));
                    _drawList->PathLineTo(c + ImVec2(0, r));
                    _drawList->PathLineTo(c + ImVec2(-r, 0));
                    _drawList->PathFillConvex(_color);
                }
                else
                {
                    const auto r = 0.607f * rect_w / 2.0f - 0.5f;
                    const auto c = rect_center;
                    _drawList->PathLineTo(c + ImVec2(0, -r));
                    _drawList->PathLineTo(c + ImVec2(r, 0));
                    _drawList->PathLineTo(c + ImVec2(0, r));
                    _drawList->PathLineTo(c + ImVec2(-r, 0));
                    if (_innerColor & 0xFF000000)
                        _drawList->AddConvexPolyFilled(_drawList->_Path.Data, _drawList->_Path.Size, _innerColor);
                    _drawList->PathStroke(_color, true, 2.0f * outline_scale);
                }
            }
            else
            {
                const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);
                _drawList->AddTriangleFilled(
                    ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
                    ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                    ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
                    _color);
            }
        }
    }
}

