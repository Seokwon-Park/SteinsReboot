#pragma once

#include "stb_image.h"

namespace Daydream
{
	struct ImageData
	{
		Int32 width = 0;
		Int32 height = 0;
		std::variant<Array<UInt8>, Array<Float32>> data;

		const void* GetRawDataPtr() const
		{
			return std::visit([](const auto& array) -> const void*
				{
					return array.data();
				}, data);
		}

		UInt64 GetTotalByte() const
		{
			return std::visit([](const auto& array) -> UInt64
				{
					return array.size() * sizeof(array[0]);
				}, data);
		}
	};

	class ImageLoader
	{
	public:
		template<typename T>
		static ImageData LoadAndFillImageData(const Path& _path)
		{
			ImageData imageData;
			T* pixels = nullptr;
			String pathString = _path.ToString();
			if constexpr (std::is_same_v<T, stbi_uc>) { // T가 unsigned char라면
				pixels = stbi_load(pathString.c_str(), &imageData.width, &imageData.height, nullptr, 4); // 4채널로 강제
			}
			else
			{ // T가 float라면
				pixels = stbi_loadf(pathString.c_str(), &imageData.width, &imageData.height, nullptr, 4); // 4채널로 강제
			}

			DAYDREAM_CORE_ASSERT(pixels, "Failed to load image!");

			using ArrayType = Array<std::conditional_t<std::is_same_v<T, stbi_uc>, UInt8, Float32>>;
			imageData.data = ArrayType();
			auto& pixelData = std::get<ArrayType>(imageData.data);

			UInt64 imageSize = (UInt64)imageData.width * imageData.height * 4;
			pixelData.resize(imageSize);
			memcpy(pixelData.data(), pixels, imageSize * sizeof(T));

			stbi_image_free(pixels);

			return imageData;
		}

		static ImageData LoadImageFile(const Path& _path);
		//HDRIImageData LoadHDRIFile(const Path& _path);
	};
}