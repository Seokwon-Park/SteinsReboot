#include "DaydreamPCH.h"
#include "TextureCube.h"

#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Utility/ImageLoader.h"

namespace Daydream
{

	TextureCube::TextureCube(Shared<GPUTexture> _texture)
		:Texture(_texture)
	{
	}

	//Shared<TextureCube> TextureCube::Create(const Array<Path>& _paths, const TextureDesc& _desc)
	//{
	//	Array<const void*> imageDatas;
	//	Array<ImageData> temp;
	//	TextureDesc finalDesc = _desc;
	//	for (auto path : _paths)
	//	{
	//		ImageData imageData = ImageLoader::LoadImageFile(path);

	//		if (std::get<Array<UInt8>>(imageData.data).data() != nullptr)
	//		{
	//			finalDesc.width = imageData.width;
	//			finalDesc.height = imageData.height;

	//			temp.push_back(imageData);
	//		}
	//	}
	//	for (int i = 0; i < temp.size(); i++)
	//	{
	//		imageDatas.push_back(std::get<Array<UInt8>>(temp[i].data).data());
	//	}
	//	Shared<TextureCube> textureCube = Renderer::GetRenderDevice()->CreateTextureCube(imageDatas, finalDesc);
	//	for (int i = 0; i < 6; i++)
	//	{
	//		Path path = _paths[i];
	//		textureCube->textures[i] = ResourceManager::GetResource<Texture2D>(path.make_preferred().string());
	//	}
	//	return textureCube;
	//}

	//Shared<TextureCube> TextureCube::Create(const Array<Shared<Texture2D>>& _textures, const TextureDesc& _desc)
	//{
	//	TextureDesc desc = _desc;
	//	desc.type = TextureType::Texture2D;
	//	Shared<GPUTexture> gpuTexture = Renderer::GetRenderDevice()->CreateGPUTexture(desc);
	//	//textureCube->textures = _textures;
	//	return textureCube;
	//}

	Shared<TextureCube> TextureCube::Create(const Texture2DDesc& _desc)
	{
		TextureDesc desc{};
		desc.width = _desc.width;
		desc.height = _desc.height;
		desc.layerCount = 6;
		desc.mipLevels = _desc.mipLevels;
		desc.sampleCount = _desc.sampleCount;
		desc.format = _desc.format;
		desc.textureUsage = _desc.textureUsage;
		desc.type = TextureType::TextureCube;
		Shared<GPUTexture> gpuTexture = Renderer::GetRenderDevice()->CreateGPUTexture(desc);
		Shared<TextureCube> textureCube = MakeShared<TextureCube>(gpuTexture);

		return textureCube;
	}


	TextureCube::~TextureCube()
	{
		//textures.clear();
	}

	//void TextureCube::Update(UInt32 _faceIndex, Shared<Texture2D> _texture)
	//{
	//	textures[_faceIndex] = _texture;
	//	//textures[_faceIndex]->SetSampler(ResourceManager::GetResource<Sampler>("LinearClampToEdge"));
	//}

	//void* TextureCube::GetImGuiHandle(UInt32 _faceIndex)
	//{
	//	if (textures[_faceIndex] == nullptr)
	//	{
	//		return nullptr;
	//	}
	//	return textures[_faceIndex]->GetImGuiHandle();
	//}
}