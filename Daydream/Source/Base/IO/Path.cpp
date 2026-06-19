#include "DaydreamPCH.h"
#include "Path.h"

namespace Daydream
{
	String Path::GetFileName() const
	{
		if (FileSystem::IsFile(path))
		{
			DAYDREAM_CORE_WARN("{} is Not File!", ToString());
			return "";
		}
		 return GetLastComponentName(); 
	}
	String Path::GetDirectoryName() const
	{
		// file - return directory
		if (FileSystem::IsDirectory(path))
		{
			return GetParentPath().GetLastComponentName();
		}
		// folder - return path
		return GetLastComponentName();
	}
}