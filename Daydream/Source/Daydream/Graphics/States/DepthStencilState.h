#pragma once

namespace Daydream
{
    enum class CompareFunction
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    // 스텐실 테스트 결과에 따른 버퍼 갱신 동작
    enum class StencilOperation
    {
        Keep,               // 기존 값 유지
        Zero,               // 0으로 초기화
        Replace,            // 참조값(Reference Value)으로 덮어쓰기
        IncrementSaturate,  // 1 증가 (최대치에서 멈춤)
        DecrementSaturate,  // 1 감소 (0에서 멈춤)
        Invert,             // 비트 반전
        IncrementWrap,      // 1 증가 (최대치 넘으면 0으로 순환)
        DecrementWrap       // 1 감소 (0보다 작아지면 최대치로 순환)
    };

    // 스텐실 면(Face) 단위 세부 동작
    struct StencilOperationDesc
    {
        StencilOperation failOp = StencilOperation::Keep;       // 스텐실 테스트 실패 시
        StencilOperation depthFailOp = StencilOperation::Keep;  // 스텐실은 통과, 깊이 테스트 실패 시
        StencilOperation passOp = StencilOperation::Keep;       // 둘 다 통과 시
        CompareFunction compareFunc = CompareFunction::Always;  // 스텐실 판정 조건
    };

    // 통합된 깊이-스텐실 상태 구조체
    struct DepthStencilStateDesc
    {
        // 깊이 (Depth) 설정 영역
        Bool depthEnable = true;
        Bool depthWriteEnable = true;
        CompareFunction depthFunc = CompareFunction::Less;

        // 스텐실 (Stencil) 범용 설정 영역
        Bool stencilEnable = false;
        UInt8 stencilReadMask = 0xFF;
        UInt8 stencilWriteMask = 0xFF;

        // 스텐실 면(Face) 단위 동작 영역
        StencilOperationDesc frontFace;
        StencilOperationDesc backFace;
    };
}
