#include "DaydreamPCH.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"
#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Manager/ResourceManager.h"

namespace Daydream
{
    void RenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
    {
        currentGraphicsPipelineState = _pipelineState;
    }

    //void RenderContext::SetTexture2D(const String& _name, Shared<Texture2D> _texture)
    //{
    //    //if (!_texture->HasSampler())
    //    //{
    //    //    _texture->SetSampler(ResourceManager::GetResource<Sampler>("LinearRepeat"));
    //    //}
    //}

    void RenderContext::CopyDataToTexture2D(const Texture2D* _target, const void*_data)
    {
        //Shared<UploadBuffer> uploadBuffer = UploadBuffer::Create(_data->size());
        //uploadBuffer->UpdateData(_data->data(), _data->size());
        //CopyBufferToTexture(uploadBuffer->GetGPUBuffer(), _target->GetGPUTexture());
    }


}
