#include "DaydreamPCH.h"
#include "AssetBrowserPanel.h"

namespace Daydream
{
	static Path rootPath = "Asset";
	AssetBrowserPanel::AssetBrowserPanel()
		:currentPath("Asset")
	{
	}

	
	AssetBrowserPanel::~AssetBrowserPanel()
	{
	}

	void AssetBrowserPanel::OnImGuiRender()
	{
		//// 예시: 자산 브라우저 패널
		ImGui::Begin("Asset Browser");
		//if (currentPath != rootPath)
		//{
		//	if (ImGui::Button("Back"))
		//	{
		//		currentPath = currentPath.parent_path();
		//	}
		//}

		float treeViewWidth = 200.0f;
		ImGui::BeginChild("FolderTreeView", ImVec2(treeViewWidth, 0));
		{
			// 루트 노드
			if (ImGui::TreeNodeEx(rootPath.ToString().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
				(rootPath == currentPath ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_DrawLinesToNodes))
			{
				if (ImGui::IsItemClicked())
				{
					currentPath = rootPath;
				}
				RenderFolderTree(rootPath, currentPath);
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("AssetGridView");
		{
			// 그리드 레이아웃 계산
			float padding = 16.0f;
			float thumbnailSize = 80.0f;
			float cellSize = thumbnailSize + padding;
			float panelWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = static_cast<int>(panelWidth / cellSize);
			if (columnCount < 1) columnCount = 1;

			// 테이블 기반 그리드 시작
			if (ImGui::BeginTable("AssetGridTable", columnCount))
			{
				for (const Path& path : FileSystem::GetDirectoryEntries(currentPath))
				{
					ImGui::TableNextColumn();

					std::string pathString = path.ToString();
					std::string filenameString = path.GetFileName();

					ImGui::PushID(pathString.c_str()); // 각 위젯에 고유 ID 부여

					Texture2D* thumbnail;

					// 폴더 또는 파일 아이콘 표시
					if (path.IsDirectory())
					{
						thumbnail = AssetManager::GetAssetByPath<Texture2D>("Resource/DirectoryIcon.png");
						ImGui::ImageButton(pathString.c_str(), (ImTextureID)thumbnail->GetImGuiHandle(), { thumbnailSize, thumbnailSize });
					}
					else // 파일인 경우
					{
						AssetType type = AssetManager::GetAssetTypeFromPath(path);
						String typeString = AssetManager::AssetTypeToString(type);
						Asset* asset;
						thumbnail = AssetManager::GetAssetByPath<Texture2D>("Resource\\FileIcon.png");
						switch (type)
						{
						case AssetType::None: // is not AssetFile
						{
							thumbnail = AssetManager::GetAssetByPath<Texture2D>("Resource\\FileIcon.png");
							ImGui::ImageButton(filenameString.c_str(), (ImTextureID)thumbnail->GetImGuiHandle(), { thumbnailSize, thumbnailSize });
							break;
						}
						case AssetType::Texture2D:
						{
							thumbnail = AssetManager::GetAssetByPath<Texture2D>(pathString);
							asset = thumbnail;
							if (thumbnail != nullptr)
							{
								ImGui::ImageButton(pathString.c_str(), (ImTextureID)thumbnail->GetImGuiHandle(), { thumbnailSize, thumbnailSize });
							}

							if (ImGui::BeginDragDropSource())
							{
								AssetHandle assetHandle = asset->GetAssetHandle();
								// 경로 데이터를 페이로드로 설정
								ImGui::SetDragDropPayload(typeString.c_str(), &assetHandle, sizeof(AssetHandle));

								// 드래그 중 미리보기
								ImGui::Text("%s", filenameString.c_str());
								ImGui::Image((ImTextureID)thumbnail->GetImGuiHandle(), ImVec2(50, 50));

								ImGui::EndDragDropSource();
							}
							break;
						}
						case AssetType::TextureCube:
							break;
						case AssetType::Shader:
							break;
						case AssetType::Scene:
							break;
						case AssetType::Model:
						{
							asset = AssetManager::GetAssetByPath<Model>(pathString);

							ImGui::ImageButton(pathString.c_str(), (ImTextureID)thumbnail->GetImGuiHandle(), { thumbnailSize, thumbnailSize });
							if (ImGui::BeginDragDropSource())
							{
								AssetHandle assetHandle = asset->GetAssetHandle();
								// 경로 데이터를 페이로드로 설정
								ImGui::SetDragDropPayload(typeString.c_str(), &assetHandle, sizeof(AssetHandle));

								// 드래그 중 미리보기
								ImGui::Text("%s", filenameString.c_str());
								ImGui::Image((ImTextureID)thumbnail->GetImGuiHandle(), ImVec2(50, 50));

								ImGui::EndDragDropSource();
							}
						}
						break;
						case AssetType::Script:
							break;
						default:
						{
							thumbnail = AssetManager::GetAssetByPath<Texture2D>("Resource\\FileIcon.png");
							ImGui::ImageButton(filenameString.c_str(), (ImTextureID)thumbnail->GetImGuiHandle(), { thumbnailSize, thumbnailSize });
							break;
						}
						}
					}

					// 더블 클릭 이벤트 처리
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (path.IsDirectory())
						{
							currentPath /= path.GetFileName(); // 하위 폴더로 이동
						}
					}

					// 파일 이름 표시
					ImGui::TextWrapped("%s", filenameString.c_str());

					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
		ImGui::End();

	}

	void AssetBrowserPanel::RenderFolderTree(const Path& path, Path& selectedPath)
	{
		for (const Path& entryPath : FileSystem::GetDirectoryEntries(path))
		{
			if (entryPath.IsDirectory())
			{
				const String filenameString = entryPath.GetFileName();

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DrawLinesToNodes;
				if (entryPath == selectedPath) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool isOpen = ImGui::TreeNodeEx(filenameString.c_str(), flags);

				if (ImGui::IsItemClicked()) {
					selectedPath = entryPath;
				}

				if (isOpen) {
					RenderFolderTree(entryPath, selectedPath); // 재귀 호출
					ImGui::TreePop();
				}
			}
		}
	}
}

