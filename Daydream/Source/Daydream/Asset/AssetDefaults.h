#pragma once

#include "Daydream/Core/UUID.h"

namespace Daydream
{
    struct AssetDefaults
    {
        static const AssetHandle AlbedoHandle;   // 흰색 1x1
        static const AssetHandle NormalHandle;    // (0.5, 0.5, 1.0)
        static const AssetHandle RoughnessHandle; // 보통 흰색 or 회색
        static const AssetHandle MetallicHandle;  // 보통 검은색
        static const AssetHandle AOHandle;        // 보통 흰색

        static const AssetHandle MaterialHandle;  

        static const AssetHandle QuadMeshHandle;        
        static const AssetHandle BoxMeshHandle;   
        static const AssetHandle SkyboxSphereHandle;

        static const AssetHandle DeferredPBRPipelineHandle;
        static const AssetHandle GBufferPipelineHandle;
        static const AssetHandle EnvironmentPipelineHandle;
        static const AssetHandle EquirectangularPipelineHandle;
        static const AssetHandle IrradiancePipelineHandle;
        static const AssetHandle PrefilterPipelineHandle;
        static const AssetHandle SpritePipelineHandle;
        static const AssetHandle ResizePipelineHandle;
        static const AssetHandle BRDFPipelineHandle;
        static const AssetHandle DepthPipelineHandle;
        static const AssetHandle MaskPipelineHandle;
        static const AssetHandle ForwardPipelineHandle;
    };
}
