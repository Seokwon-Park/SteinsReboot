#include "DaydreamPCH.h"
#include "ShaderGroupRegistry.h"

#include "Daydream/Graphics/Resources/ShaderGroup.h"

namespace Daydream
{
	ShaderGroupRegistry::ShaderGroupRegistry()
	{

	}

	void ShaderGroupRegistry::CreateBuiltinResources()
	{
		//registry["Model"] = ShaderGroup::Create("ModelVS", "ModelPS");

		registry["Model"] = ShaderGroup::CreateBuiltin("ModelVS.hlsl", "PBRModelPS.hlsl");
		registry["Cubemap"] = ShaderGroup::CreateBuiltin("CubemapVS.hlsl", "CubemapPS.hlsl");
		registry["Equirectangular"] = ShaderGroup::CreateBuiltin("CubemapVS.hlsl", "EquirectangularPS.hlsl");
		registry["Irradiance"] = ShaderGroup::CreateBuiltin("CubemapVS.hlsl", "IrradiancePS.hlsl");
		registry["Prefilter"] = ShaderGroup::CreateBuiltin("CubemapVS.hlsl", "PrefilterPS.hlsl");
		registry["Sprite"] = ShaderGroup::CreateBuiltin("SpriteVS.hlsl", "SpritePS.hlsl");
		registry["Resize"] = ShaderGroup::CreateBuiltin("ResizeVS.hlsl", "ResizePS.hlsl");
		registry["BRDF"] = ShaderGroup::CreateBuiltin("BRDFVS.hlsl", "BRDFPS.hlsl");
		registry["Depth"] = ShaderGroup::CreateBuiltin("DepthVS.hlsl");
		registry["GBuffer"] = ShaderGroup::CreateBuiltin("GBufferVS.hlsl", "GBufferPS.hlsl");
		registry["Mask"] = ShaderGroup::CreateBuiltin("MaskVS.hlsl", "MaskPS.hlsl");
		registry["DeferredLighting"] = ShaderGroup::CreateBuiltin("DeferredLightingVS.hlsl", "DeferredLightingPS.hlsl");
	}
}