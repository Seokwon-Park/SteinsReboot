#pragma once

#include "Daydream.h"
#include "UIPanel.h"

namespace Daydream
{
	class SkyboxPanel : public UIPanel
	{
	public:
		SkyboxPanel();
		~SkyboxPanel();

		inline void SetSkybox(Skybox* _skybox) { skybox = _skybox; }

		inline bool IsUsingSkybox() const { return isUsingSkybox; };

		void OnImGuiRender();
	private:
		bool isUsingSkybox = true;
		bool isHDR = false;

		const char* faceLabels[6] = { "Right (+X)", "Left (-X)", "Top (+Y)", "Bottom (-Y)", "Front (+Z)", "Back (-Z)" };

		const Array<String> resolutionOptions = { "64", "128", "256", "512", "1024", "2048" };
		const Array<UInt32> resolutionFactors = { 64, 128, 256, 512, 1024, 2048 };

		int skyboxIndex = 3;
		int diffuseIndex = 0;
		int specularIndex = 3;

		Int32 mipLevel = 0;

		Skybox* skybox = nullptr;
		Texture2D* equirectangularDropTarget;
	};
}