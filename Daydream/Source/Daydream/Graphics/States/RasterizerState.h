#pragma once

namespace Daydream
{
	struct RasterizerStateDesc
	{
		FillMode fillMode = FillMode::Solid;
		CullMode cullMode = CullMode::Back;
		bool frontCounterClockwise = false;
		bool depthClipEnable = true;
		bool scissorEnable = false;
		bool multisampleEnable = false;
		bool antialiasedLineEnable = false;
		int depthBias = 0;
		float depthBiasClamp = 0.0f;
		float slopeScaledDepthBias = 0.0f;
	};
}