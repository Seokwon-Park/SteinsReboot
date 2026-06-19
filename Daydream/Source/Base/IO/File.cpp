#include "DaydreamPCH.h"
#include "File.h"

namespace Daydream
{
	bool File::OpenForWrite()
	{
		if (!FileSystem::IsExist(path))
		{
			FileSystem::MakeDirectory(path.GetParentPath());
		}

		out.open(path, std::ios::out | std::ios::binary);
		return out.is_open();
	}

	bool File::OpenForRead()
	{
		in.open(path, std::ios::in | std::ios::binary);
		return in.is_open();
	}

	void File::Close()
	{
		if (out.is_open())
			out.close();
		if (in.is_open())
			in.close();
	}
}