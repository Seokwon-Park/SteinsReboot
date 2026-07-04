#include "DaydreamPCH.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
    void RenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
    {
        currentGraphicsPipelineState = _pipelineState;
    }
}
