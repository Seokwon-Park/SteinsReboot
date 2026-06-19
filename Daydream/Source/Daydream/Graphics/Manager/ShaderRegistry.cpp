#include "DaydreamPCH.h"
#include "ShaderRegistry.h"

#include "Daydream/Graphics/Resources/Shader.h"
#include "Daydream/Graphics/Resources/ShaderGroup.h"

namespace Daydream
{
	ShaderManager::ShaderManager()
	{

	}

	void ShaderManager::LoadShadersFromDirectory(Path _directory, bool _isRecursive)
	{
		//if (!FileSystem::IsExist(_directory) || !_directory.IsDirectory())
		//{
		//	// 경로가 없거나 디렉토리가 아니면 리턴
		//	return;
		//}

		//for (const Path& entryPath : FileSystem::GetDirectoryEntries(_directory))
		//{
		//	if (entryPath.IsFile())
		//	{
		//		// 지원하는 확장자인지 확인
		//		String pathString = entryPath.ToString();
		//		String shaderName = entryPath.GetFileNameWithoutExt();
		//		String extension = entryPath.GetExtensionString();
		//		for (const auto& supportedExtension : supportedExtensions) 
		//		{
		//			if (extension == supportedExtension) 
		//			{
		//				ShaderType shaderType;
		//				if (pathString.find("VS.") != std::string::npos)
		//				{
		//					shaderType = ShaderType::Vertex;
		//				}
		//				if (pathString.find("PS.") != std::string::npos)
		//				{
		//					shaderType = ShaderType::Pixel;
		//				}

		//				if (pathString.find("HS.") != std::string::npos)
		//				{
		//					shaderType = ShaderType::Hull;
		//				}

		//				if (pathString.find("DS.") != std::string::npos)
		//				{
		//					shaderType = ShaderType::Domain;
		//				}
		//				if (pathString.find("GS.") != std::string::npos)
		//				{
		//					shaderType = ShaderType::Geometry;
		//				}
		//				//registry[shaderName] = Shader::Create(pathString, shaderType, ShaderLoadMode::File);
		//				break;
		//			}
		//		}
		//	}
		//	// 재귀 옵션이 true이고, 현재 항목이 하위 디렉토리라면
		//	else if (_isRecursive && entryPath.IsDirectory())
		//	{
		//		// 자기 자신을 다시 호출하여 하위 폴더 탐색
		//		LoadShadersFromDirectory(entryPath, true);
		//	}
		//}
	}

}
