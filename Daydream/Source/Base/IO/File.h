#pragma once

#include <fstream>
#include "Path.h"

namespace Daydream
{
	class FileWriter
	{
	public:
		FileWriter(const Path& _path) : path(_path) { out.open(path, std::ios::binary); }
		~FileWriter() { Close(); }

		inline bool IsOpened() const { return out.is_open(); }
		inline void Close() { if (out.is_open()) out.close(); }

		template<typename T>
		void Write(const T& _value) { out.write((const char*)&_value, sizeof(T)); }

		void Write(const String& _str)
		{
			Write((UInt64)_str.size());
			out.write(_str.data(), _str.size());
		}

		void Write(const void* _data, UInt64 _size)
		{
			out.write((const char*)_data, _size);
		}

	private:
		Path path;
		std::ofstream out;
	};

	class FileReader
	{
	public:
		FileReader(const Path& _path) : path(_path) { in.open(path, std::ios::binary); }
		~FileReader() { Close(); }

		inline bool IsOpened() const { return in.is_open(); }
		inline void Close() { if (in.is_open()) in.close(); }


		template<typename T>
		void Read(T& _outValue) { in.read((char*)&_outValue, sizeof(T)); }

		void Read(String& _outStr)
		{
			UInt64 len = 0;
			Read(len);
			_outStr.resize(len);
			in.read(_outStr.data(), len);
		}

		void Read(void* _data, UInt64 _size)
		{
			in.read((char*)_data, _size);
		}
	private:
		Path path;
		std::ifstream in;
	};
}

