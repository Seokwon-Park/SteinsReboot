#pragma once

#include "Daydream/Core/UUID.h"

namespace Daydream
{
    struct AssetDefaults
    {
        static const AssetHandle DefaultAlbedoHandle;   // 흰색 1x1
        static const AssetHandle DefaultNormalHandle;    // (0.5, 0.5, 1.0)
        static const AssetHandle DefaultRoughnessHandle; // 보통 흰색 or 회색
        static const AssetHandle DefaultMetallicHandle;  // 보통 검은색
        static const AssetHandle DefaultAOHandle;        // 보통 흰색

        static const AssetHandle DefaultMaterialHandle;  

        static const AssetHandle DefaultQuadMeshHandle;        
        static const AssetHandle DefaultBoxMeshHandle;        
    };
}
