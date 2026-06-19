#pragma once

#include <fstream>
#include "Path.h"

namespace Daydream
{
	class File
	{
	public:
		File(const Path& _path) : path(_path) {}

		bool OpenForWrite();
		bool OpenForRead();
		void Close();

		template<typename T>
		void Write(const T& _value)
		{
			out.write((const char*)&_value, sizeof(T));
		}

		void Write(const String& _str)
		{
			UInt32 len = (UInt32)_str.length();
			Write(len);
			out.write(_str.data(), len);
		}

		template<typename T>
		void Write(const Array<T>& _arr)
		{
			UInt32 size = (UInt32)(_arr.size() * sizeof(T));
			Write(size);
			out.write((const char*)_arr.data(), size);
		}


		template<typename T>
		void Read(T& _outValue)
		{
			in.read((char*)&_outValue, sizeof(T));
		}

		void Read(String& _outStr)
		{
			UInt32 len;
			Read(len); 
			_outStr.resize(len); 
			in.read(_outStr.data(), len);
		}
		template<typename T>
		void Read(Array<T>& _outArr)
		{
			UInt32 sizeBytes;
			Read(sizeBytes);

			_outArr.resize(sizeBytes / sizeof(T));
			in.read((char*)_outArr.data(), sizeBytes);
		}

	private:
		Path path;
		std::ofstream out;
		std::ifstream in;
	};
}

