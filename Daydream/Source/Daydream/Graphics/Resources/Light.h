#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"

namespace Daydream
{
    enum LightType
    {
        Directional = 0,
        Point = 1,
        Spot = 2
    };

    struct DirectionalLight
    {
        Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
        Float32 intensity = 1.0f;
        Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
        Float32 padding1 = 0.0f;
    };

    struct PointLight
    {
        Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
        Float32 range = 0.0f;
        Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
        Float32 intensity = 1.0f;
    };

    struct SpotLight
    {
        Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
        Float32 range = 10.0f;
        Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
        Float32 intensity = 1.0f;
        Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
        Float32 innerConeCos = 0.0f;
        Vector3 padding1 = Vector3(0.0f, 0.0f, 0.0f);
        Float32 outerConeCos = 0.0f;
    };

	class Light
	{
    public: 
        Light();

        UInt32 type = LightType::Directional;
        Float32 intensity = 100.0f;
        Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
        Float32 range = 10.0f;   // Point, Spot 타입이 사용
        Float32 spotInnerAngle = 20.0f;  // Spot 타입만 사용
        Float32 spotOuterAngle = 30.0f;  // Spot 타입만 사용
	};
}