#pragma once

#include "UIPanel.h"
#include "Daydream/Graphics/Resources/Framebuffer.h"

namespace Daydream
{
	class ViewportPanel : public UIPanel
	{
	public:
		ViewportPanel();
		~ViewportPanel() {};

		virtual void OnImGuiRender() override;
	protected:

	private:

	};
}
