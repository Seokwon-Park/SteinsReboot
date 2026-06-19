#include "DaydreamPCH.h"
#include "FileSystem.h"

namespace Daydream
{
	FileSystem::FileSystem()
	{
	}

	FileSystem::~FileSystem()
	{
	}
	Array<Path> FileSystem::GetDirectoryEntries(const Path& _dirPath)
	{
		Array<Path> entries;

		if (!IsDirectory(_dirPath))
		{
			DAYDREAM_CORE_WARN("GetDirectoryEntries failed: {0} is not a valid directory.", _dirPath.ToString());
			return entries; 
		}

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(_dirPath))
		{
			entries.emplace_back(entry.path());
		}

		return entries;
	}
	Array<Path> FileSystem::GetDirectoryEntriesRecursive(const Path& _dirPath)
	{
		Array<Path> entries;

		if (!IsDirectory(_dirPath))
		{
			DAYDREAM_CORE_WARN("GetDirectoryEntriesRecursive failed: {0} is not a valid directory.", _dirPath.ToString());
			return entries;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(_dirPath))
		{
			entries.emplace_back(entry.path());
		}

		return entries;
	}
	bool FileSystem::MakeDirectory(const Path& _dirPath)
	{
		return std::filesystem::create_directories(_dirPath);
	}
	bool FileSystem::MakeTextFile(const Path& _filePath, StringView _text)
	{
		std::ofstream fout(_filePath.ToString());

		// 파일이 제대로 열렸는지 검사 (디렉토리가 없거나 권한이 없으면 실패함)
		if (!fout.is_open())
		{
			DAYDREAM_CORE_ERROR("Failed to write text file: {0}", _filePath.ToString());
			return false;
		}

		fout.write(_text.data(), _text.length());
		fout.close();
		
		return true;
	}

	UInt64 FileSystem::GetFileLastWriteTime(const Path& _path)
	{
		if (!IsFile(_path))
		{
			DAYDREAM_CORE_WARN("Path is not File!");
			return 0;
		}
		auto lastTime = std::filesystem::last_write_time(_path);
		return static_cast<UInt64>(lastTime.time_since_epoch().count());
	}

	//{
	//	std::string yamlString = YAML::Dump(_node);

	//	return MakeTextFile(_filePath, yamlString);
	//}
	//bool FileSystem::MakeYamlFile(const Path& _filePath, const YAML::Emitter& _emitter)
	//{
	//	StringView yamlView(_emitter.c_str(), _emitter.size());

	//	return MakeTextFile(_filePath, yamlView);
	//}
}

