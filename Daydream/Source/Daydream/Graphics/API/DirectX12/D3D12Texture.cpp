#include "DaydreamPCH.h"
#include "D3D12Texture.h"

#include "D3D12Utility.h"

namespace Daydream
{
	D3D12GPUTexture::D3D12GPUTexture(D3D12RenderDevice* _device, const TextureDesc& _desc)
		:GPUTexture(_desc)
	{
		device = _device;

		DXGI_FORMAT format = GraphicsUtility::DirectX::ConvertToDXGIFormat(desc.format);

		D3D12MA::ALLOCATION_DESC allocationDesc{};
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = desc.width;
		textureDesc.Height = desc.height;
		if (_desc.type == TextureType::TextureCube)
			textureDesc.DepthOrArraySize = 6;
		else if (_desc.type == TextureType::TextureCubeArray)
			textureDesc.DepthOrArraySize = 6 * _desc.layerCount;
		else if (_desc.type == TextureType::Texture2DArray)
			textureDesc.DepthOrArraySize = _desc.layerCount;
		else
			textureDesc.DepthOrArraySize = 1; // 일반 2D 또는 2DMultisample
		textureDesc.MipLevels = _desc.mipLevels;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; 
		textureDesc.Flags = GraphicsUtility::DirectX12::ConvertToD3D12BindFlags(desc.textureUsage);

		HRESULT hr = device->GetMemoryAllocator()->CreateResource(
			&allocationDesc,
			&textureDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			allocation.GetAddressOf(),
			IID_PPV_ARGS(texture.GetAddressOf()));

		if (_desc.type == TextureType::TextureCube)
			texture->SetName(L"TextureCube");
		else if (_desc.type == TextureType::TextureCubeArray)
			texture->SetName(L"TextureCubeArray");
		else if (_desc.type == TextureType::Texture2DArray)
			texture->SetName(L"Texture2DArray");
		else
			texture->SetName(L"Texture2D");
	}

	D3D12GPUTexture::D3D12GPUTexture(D3D12RenderDevice* _device, const TextureDesc& _desc, ComPtr<ID3D12Resource> _d3d12BackBuffer)
		:GPUTexture(_desc)
	{
		texture = _d3d12BackBuffer;
	}

	//D3D12Texture2D::D3D12Texture2D(D3D12RenderDevice* _device, const TextureDesc& _desc)
	//	:Texture2D(_desc)
	//{
	//	device = _device;

	//	DXGI_FORMAT textureFormat = GraphicsUtility::DirectX::ConvertToDXGIFormat(desc.format);
	//	DXGI_FORMAT srvFormat = textureFormat;
	//	DXGI_FORMAT dsvFormat = textureFormat;

	//	D3D12_RESOURCE_DESC textureDesc = {};
	//	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//	textureDesc.Width = desc.width;
	//	textureDesc.Height = desc.height;
	//	textureDesc.DepthOrArraySize = 1;
	//	textureDesc.MipLevels = desc.mipLevels;
	//	textureDesc.Format = textureFormat;
	//	textureDesc.SampleDesc.Count = 1;
	//	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // vk::ImageTiling::eOptimal에 해당
	//	textureDesc.Flags = GraphicsUtility::DirectX12::ConvertToD3D12BindFlags(desc.bindFlags);

	//	texture = device->CreateTexture(textureDesc, D3D12_RESOURCE_STATE_COPY_DEST);

	//	if (GraphicsUtility::HasFlag(desc.bindFlags, TextureUsage::ShaderResource) && GraphicsUtility::HasFlag(desc.bindFlags, TextureUsage::DepthStencil))
	//	{
	//		srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//		dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	}

	//	if (!(textureDesc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) &&
	//		GraphicsUtility::HasFlag(desc.bindFlags, TextureUsage::ShaderResource))
	//	{ 
	//		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//		srvDesc.Format = srvFormat;
	//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//		srvDesc.Texture2D.MipLevels = desc.mipLevels;
	//		srvDesc.Texture2D.MostDetailedMip = 0;
	//		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//		device->GetCBVSRVUAVHeapAlloc().Alloc(&srvCpuHandle, &srvGpuHandle);
	//		device->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, srvCpuHandle);
	//	}
	//	if (textureDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	//	{
	//		device->GetRTVHeapAlloc().Alloc(&rtvCpuHandle);
	//		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//		rtvDesc.Format = textureDesc.Format;
	//		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//		//// Determine ViewDimension based on texture type
	//		//if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize == 1) {
	//		//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//		//	// If MSAA: rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	//		//}
	//		//else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize > 1) {
	//		//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	//		//	rtvDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize;
	//		//	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	//		//	rtvDesc.Texture2DArray.MipSlice = 0; // Usually mip 0 for RTV
	//		//}
	//		// Handle 3D if needed, etc.

	//		device->GetDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, rtvCpuHandle);
	//	}
	//	if (textureDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	//	{
	//		device->GetDSVHeapAlloc().Alloc(&dsvCpuHandle); // DSVs only need CPU handle
	//		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//		dsvDesc.Format = dsvFormat;
	//		dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // Can be D3D12_DSV_FLAG_READ_ONLY_DEPTH or _STENCIL
	//		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//		
	//		// Determine ViewDimension based on texture type
	//		//if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize == 1) {
	//		//	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//		//	// If MSAA: dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	//		//}
	//		//else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize > 1) {
	//		//	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
	//		//	dsvDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize;
	//		//	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	//		//	dsvDesc.Texture2DArray.MipSlice = 0; // Usually mip 0 for DSV
	//		//}

	//		device->GetDevice()->CreateDepthStencilView(texture.Get(), &dsvDesc, dsvCpuHandle);
	//	}

	//	if (textureDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
	//	{
	//		D3D12_GPU_DESCRIPTOR_HANDLE tempGpuHandle;
	//		device->GetCBVSRVUAVHeapAlloc().Alloc(&uavCpuHandle, &tempGpuHandle); // UAV needs both CPU and GPU handle

	//		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	//		uavDesc.Format = textureDesc.Format;
	//		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

	//		// Determine ViewDimension based on texture type
	//		//if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize == 1) {
	//		//	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	//		//	uavDesc.Texture2D.MipSlice = 0; // Usually mip 0 for UAV
	//		//}
	//		//else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize > 1) {
	//		//	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
	//		//	uavDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize;
	//		//	uavDesc.Texture2DArray.FirstArraySlice = 0;
	//		//	uavDesc.Texture2DArray.MipSlice = 0;
	//		//}
	//		//else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D) {
	//		//	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	//		//	uavDesc.Texture3D.MipSlice = 0;
	//		//	uavDesc.Texture3D.FirstWSlice = 0;
	//		//	uavDesc.Texture3D.WSize = resDesc.DepthOrArraySize;
	//		//}

	//		device->GetDevice()->CreateUnorderedAccessView(texture.Get(), nullptr, &uavDesc, uavCpuHandle);
	//	}
	//}


	//D3D12Texture2D::~D3D12Texture2D()
	//{
	//	if (rtvCpuHandle.ptr != 0)
	//	{
	//		device->GetRTVHeapAlloc().Free(rtvCpuHandle);
	//		rtvCpuHandle.ptr = 0;
	//	}
	//	if (dsvCpuHandle.ptr != 0)
	//	{
	//		device->GetDSVHeapAlloc().Free(dsvCpuHandle);
	//		rtvCpuHandle.ptr = 0;
	//	}
	//	if (srvCpuHandle.ptr != 0)
	//	{
	//		device->GetCBVSRVUAVHeapAlloc().Free(srvCpuHandle, srvGpuHandle);
	//		srvCpuHandle.ptr = 0;
	//		srvGpuHandle.ptr = 0;
	//	}
	//	if (uavCpuHandle.ptr != 0)
	//	{
	//		device->GetCBVSRVUAVHeapAlloc().Free(uavCpuHandle, uavGpuHandle);
	//		uavCpuHandle.ptr = 0;
	//		uavGpuHandle.ptr = 0;
	//	}
	//}
	//void D3D12Texture2D::SetSampler(Shared<Sampler> _sampler)
	//{
	//	textureSampler = static_cast<D3D12Sampler*>(_sampler.get());
	//}

}
