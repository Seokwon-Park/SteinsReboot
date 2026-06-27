#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "UI/ImGuiWidgets.h"

namespace Daydream
{
	class UIPanel
	{
	public:
		virtual ~UIPanel() = default;

		virtual void OnImGuiRender() = 0;
	protected:
	private:
	};
}