#pragma once

#include <filesystem>

namespace Daydream
{
	//namespace FileSystem = std::filesystem;
	//using Path = FileSystem::path;

	class Path
	{
	public:
		Path() = default;
		Path(const std::filesystem::path& _path) : path(_path) {}
		Path(std::filesystem::path&& _path) : path(std::move(_path)) {}
		Path(StringView _str) : path(_str) {}
		Path(const String& _str) : path(_str) {}
		Path(const char* _cstr) : path(_cstr) {}

		Path(const Path& other) noexcept = default;
		Path& operator=(const Path& other) noexcept = default;

		Path(Path&& other) noexcept = default;
		Path& operator=(Path&& other) noexcept = default;

		Path operator/(const Path& _other) const
		{
			Path result = *this;
			result.path /= _other.path;
			return result;
		}

		Path& operator/=(const Path& _other)
		{
			path /= _other.path;
			return *this;
		}

		Path operator+(const Path& _other) const
		{
			Path result = *this;
			result.path += _other.path;
			return result;
		}

		Path& operator+=(const Path& _other)
		{
			path += _other.path;
			return *this;
		}

		Path operator/(const char* _path) const
		{
			Path result = *this;
			result.path /= _path;
			return result;
		}

		Path& operator/=(const char* _path)
		{
			path /= _path;
			return *this;
		}

		Path operator+(const char* _path) const
		{
			Path result = *this;
			result.path += _path;
			return result;
		}

		Path& operator+=(const char* _path)
		{
			path += _path;
			return *this;
		}

		bool operator==(const Path& _other) const
		{
			return path == _other.path;
		}

		bool operator!=(const Path& _other) const
		{
			return !(*this == _other);
		}

		inline String ToString() const { return path.string(); }
		inline WideString ToWString() const { return path.wstring(); }

		// Get Path String always use '/'
		inline String ToGenericString() const { return path.generic_string(); }

		String GetFileName() const;
		String GetDirectoryName() const;
		inline String GetLastComponentName() const { return path.filename().string(); }
		inline String GetFileNameWithoutExt() const { return path.stem().string(); }

		inline Path GetParentPath() const { return path.parent_path(); }
		inline void MoveToParent() { path = path.parent_path(); }

		inline Path GetExtension() const { return path.extension(); }
		inline String GetExtensionString() const { return path.extension().string(); }
		inline bool IsEmpty() const { return path.empty(); }

		operator const std::filesystem::path&() const { return path; }
	private:
		std::filesystem::path path;
	};
}

