#include "SkyboxPanel.h"

namespace Daydream
{
	SkyboxPanel::SkyboxPanel()
	{
		equirectangularDropTarget = AssetManager::GetAssetByPath<Texture2D>("Resource/NoTexture.png");
	}

	SkyboxPanel::~SkyboxPanel()
	{
		skybox = nullptr;
		equirectangularDropTarget = nullptr;
	}

	void SkyboxPanel::OnImGuiRender()
	{
		if (skybox == nullptr) return;

		ImGui::Begin("SkyboxSettings");
		ImGui::Checkbox("Render Skybox", &isUsingSkybox);


		if (UI::DrawToggleButtonGroup("Diffuse", &diffuseIndex, resolutionOptions))
		{
			skybox->SetDiffuseResolution(resolutionFactors[diffuseIndex]);
		}
		if (UI::DrawToggleButtonGroup("Specular", &specularIndex, resolutionOptions))
		{
			skybox->SetSpecularResolution(resolutionFactors[specularIndex]);
		}

		if (ImGui::Button("Bake IBL"))
		{
			skybox->GenerateIrradianceCubemap();
			skybox->GeneratePrefilterCubemap();
		}

		if (ImGui::Button("CreateEquirectangular"))
		{
			isHDR = true;
		}

		if (isHDR)
		{
			if (UI::DrawToggleButtonGroup("Skybox", &skyboxIndex, resolutionOptions))
			{
				skybox->SetDiffuseResolution(resolutionFactors[skyboxIndex]);
			}
			ImGui::Image(equirectangularDropTarget->GetImGuiHandle(), ImVec2{ 400,200 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetManager::AssetTypeToString(AssetType::Texture2D).c_str()))
				{
					AssetHandle* pHandle = (AssetHandle*)payload->Data;
					AssetHandle handle = *pHandle;
					auto texture = AssetManager::GetAsset<Texture2D>(handle);
					skybox->GenerateHDRCubemap(texture);
					isHDR = false;
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::SliderInt("Mip", &mipLevel, 0, skybox->GetPrefilterMipLevel() - 1);
		for (int i = 0; i < 6; i++)
		{
			ImGui::Text(faceLabels[i]);
			if (skybox->GetSkyboxTexture() != nullptr)
			{
				ImGui::Image(skybox->GetSkyboxFaceSRV(i)->GetUIHandle(), ImVec2{ 100,100 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetManager::AssetTypeToString(AssetType::Texture2D).c_str()))
					{
						AssetHandle* pHandle = (AssetHandle*)payload->Data;
						AssetHandle handle = *pHandle;
						auto texture = AssetManager::GetAsset<Texture2D>(handle);

						//skybox->UpdateSkyboxFace(i, texture);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				ImGui::Image(skybox->GetIrradianceFaceSRV(i)->GetUIHandle(), ImVec2{ 100,100 });
				ImGui::SameLine();
				ImGui::Image(skybox->GetPrefilterFaceSRV(i, mipLevel)->GetUIHandle(), ImVec2{ 100,100 });
			}
		}
		//UI::DrawMaterialController("SkyboxTextures", skyboxMaterial.get());

		ImGui::Text("BRDF");
		ImGui::Image(skybox->GetBRDFSRV()->GetUIHandle(), ImVec2{ 100,100 });
		ImGui::End();
	}
}