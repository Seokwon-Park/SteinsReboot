#pragma once

#include "Base/Container/Array.h"
#include "Path.h"


namespace Daydream
{
	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static Array<Path> GetDirectoryEntries(const Path& _dirPath);
		static Array<Path> GetDirectoryEntriesRecursive(const Path& _dirPath);
		static bool MakeDirectory(const Path& _dirPath);
		static bool MakeTextFile(const Path& _filePath, StringView _text);

		inline static String GetAbsolutePathString(const Path& _path) { return std::filesystem::absolute(_path).string(); }
		inline static bool IsDirectory(const Path& _path) { return std::filesystem::is_directory(_path); }
		inline static bool IsFile(const Path& _path) { return std::filesystem::is_regular_file(_path); }
		inline static bool IsExist(const Path& _path) { return std::filesystem::exists(_path); }
		// check A is newer than b
		static UInt64 GetFileLastWriteTime(const Path& _path);
	protected:

	private:

	};
}
