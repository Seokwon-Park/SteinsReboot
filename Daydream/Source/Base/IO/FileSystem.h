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

	protected:

	private:

	};
}
