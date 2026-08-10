---
layout: default
title: "01. Fixed Function Pipeline"
parent: "(DirectX9)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Programmable Shader

```cpp
// 이런 hlsl을 Programmable Shader라 한다
static const char* PIXEL_SHADER_HLSL = R"(
    sampler2D textureSampler : register(s0);

    struct PS_INPUT
    {
        float4 color    : COLOR0;
        float2 texcoord : TEXCOORD0;
    };

    float4 main(PS_INPUT input) : COLOR0
    {
        float4 textureColor = tex2D(textureSampler, input.texcoord);
        float4 finalColor = textureColor * input.color;
        return finalColor;
    }
)";
```

* Fixed Function Pipeline	
    * 쉐이더 설정 :	SetPixelShader(nullptr)	
    * 텍스처 샘플링	: 하드웨어가 자동 처리	
    * 색상 연산 : SetTextureStageState()	
    * 유연성 : 제한적
    * 복잡도 : 간단 (API 호출만)	
    * 성능 : 빠름 (간단한 작업)	
    * 디버깅 : API 상태 확인	
    * 호환성 : D3D7~D3D9 (옛날)	

* Programmable Shader
    * SetPixelShader(pixel_shader_)
    * sampler2D + tex2D() 사용
    * HLSL 코드로 직접 작성
    * 완전히 자유로움
    * 중간 (코드 작성 필요)
    * 빠름 (단, 최적화 가능)
    * 쉐이더 코드 디버깅
    * D3D9+ (현대적)

---

```cpp
// ========================================
// 예시 4: 완전한 UI 렌더링 시스템
// ========================================

class SimpleUIRenderer {
private:
    IDirect3DDevice9* device_;
    IDirect3DTexture9* background_tex_;
    IDirect3DTexture9* panel_tex_;
    IDirect3DTexture9* button_tex_;
    IDirect3DVertexBuffer9* vb_;
    
public:
    void RenderUI() {
        // 공통 설정
        SetupRenderStates();
        
        // 레이어별 렌더링
        RenderBackground();      // 1단계: 배경
        RenderDimOverlay();      // 2단계: 어두운 오버레이
        RenderPanel();           // 3단계: UI 패널
        RenderButtons();         // 4단계: 버튼들
        RenderText();            // 5단계: 텍스트
        
        // 상태 복원
        RestoreRenderStates();
    }
    
private:
    void SetupRenderStates() {
        // 2D 렌더링 모드
        device_->SetRenderState(D3DRS_ZENABLE, FALSE);
        device_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        device_->SetRenderState(D3DRS_LIGHTING, FALSE);
        device_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        
        // 알파 블렌딩
        device_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        
        // 텍스처 필터링
        device_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        device_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        device_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }
    
    void RenderBackground() {
        // 배경 이미지 (1920×1080)
        DrawTexture(background_tex_, 0, 0, 1920, 1080, 0xFFFFFFFF);
    }
    
    void RenderDimOverlay() {
        // 검정색 반투명 레이어 (50% 불투명)
        DrawSolidRect(0, 0, 1920, 1080, 0x80000000);  // ARGB
    }
    
    void RenderPanel() {
        // 중앙 패널 (600×400, 중앙 정렬)
        int x = (1920 - 600) / 2;  // 660
        int y = (1080 - 400) / 2;  // 340
        DrawTexture(panel_tex_, x, y, 600, 400, 0xFFFFFFFF);
    }
    
    void RenderButtons() {
        // 버튼 여러 개 그리기
        DrawTexture(button_tex_, 760, 500, 200, 50, 0xFFFFFFFF);  // 확인
        DrawTexture(button_tex_, 960, 500, 200, 50, 0xFFFFFFFF);  // 취소
    }
    
    // 헬퍼 함수: 텍스처 그리기
    void DrawTexture(IDirect3DTexture9* tex, 
                     float x, float y, float w, float h,
                     DWORD color) {
        // 버텍스 데이터 생성
        OverlayVertex vertices[4];
        vertices[0] = {x - 0.5f,     y - 0.5f,     0.0f, 1.0f, color, 0.0f, 0.0f};
        vertices[1] = {x + w - 0.5f, y - 0.5f,     0.0f, 1.0f, color, 1.0f, 0.0f};
        vertices[2] = {x - 0.5f,     y + h - 0.5f, 0.0f, 1.0f, color, 0.0f, 1.0f};
        vertices[3] = {x + w - 0.5f, y + h - 0.5f, 0.0f, 1.0f, color, 1.0f, 1.0f};
        
        // 버텍스 버퍼 업데이트
        void* pData;
        vb_->Lock(0, sizeof(vertices), &pData, D3DLOCK_DISCARD);
        memcpy(pData, vertices, sizeof(vertices));
        vb_->Unlock();
        
        // 텍스처 설정
        device_->SetTexture(0, tex);
        device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        device_->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        device_->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        
        // 그리기
        device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        device_->SetStreamSource(0, vb_, 0, sizeof(OverlayVertex));
        device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
    
    // 헬퍼 함수: 단색 사각형 그리기 (텍스처 없이)
    void DrawSolidRect(float x, float y, float w, float h, DWORD color) {
        // 버텍스만으로 그리기 (텍스처 없음)
        OverlayVertex vertices[4];
        vertices[0] = {x - 0.5f,     y - 0.5f,     0.0f, 1.0f, color, 0.0f, 0.0f};
        vertices[1] = {x + w - 0.5f, y - 0.5f,     0.0f, 1.0f, color, 0.0f, 0.0f};
        vertices[2] = {x - 0.5f,     y + h - 0.5f, 0.0f, 1.0f, color, 0.0f, 0.0f};
        vertices[3] = {x + w - 0.5f, y + h - 0.5f, 0.0f, 1.0f, color, 0.0f, 0.0f};
        
        void* pData;
        vb_->Lock(0, sizeof(vertices), &pData, D3DLOCK_DISCARD);
        memcpy(pData, vertices, sizeof(vertices));
        vb_->Unlock();
        
        // 텍스처 비활성화, 버텍스 색상만 사용
        device_->SetTexture(0, nullptr);
        device_->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        device_->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        device_->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        device_->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        
        device_->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
        device_->SetStreamSource(0, vb_, 0, sizeof(OverlayVertex));
        device_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    }
};
```