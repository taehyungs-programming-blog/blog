---
layout: default
title: "15. 조명효과 + HLSL Add-in"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Visual Studio Add-in 설치

* HLSL Tools for Visual Studio

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-5-1.png"/>
</p>

---

## 조명효과

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/6_lighting)

* 구현위주로 설명.
* 들어가기 전 알아둘 점.
    * Model 좌표(자기 자신의 좌표계) -> (* 모델 행렬) -> 월드 좌표계 -> (* View 행렬) -> 뷰 좌표계 -> (* 프로젝션 행렬) -> Normal Coordinate 좌표계(스크린 좌표계)
    * 이 순서를 잊지 말 것
* Constant Buffer를 GPU로 넘길때 시간이 오래걸리니 그 시간을 줄이는데 노력하는 것이 핵심이다.

```cpp
float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;
    
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-for
    // https://forum.unity.com/threads/what-are-unroll-and-loop-when-to-use-them.1283096/
    
    /*
        * 특이한 점.
        * 아래를 보면 Direction, Point, Spot을 모두 그려주고있는데
        * 실제로는 셋 중 하나만 그리게 됨.
        * shader에서는 if문을 최대한 쓰지 않기에 그냥 모두 그리고 사용안되는 부분은 0을 넣어버림.
    */

    // NUM_DIR_LIGHTS == 1이기에 실제로 for루프를 컴파일러가 없애려 하는데
    // 없애지말고 무조건 살리라(하드코딩해라)는 뜻
    [unroll] 
        // 없으면 warning 발생 -> warning X3557: loop only executes for 1 iteration(s), forcing loop to unroll
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(lights[i], material, input.normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    }

    return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);

}
```

```cpp
// 쉐이더에서 include할 내용들은 .hlsli 파일에 작성
// Properties -> Item Type: Does not participate in build으로 설정

// BlinnPhong 구현의 전체 구조는 Luna DX12 교재와 비슷하지만 
// 세부 구현은 이해하기 편하도록 대학 강의 스타일로 단순화하였습니다.

/* 참고: C++ SimpleMath -> HLSL */
// Matrix -> matrix 또는 float4x4
// Vector3 -> float3
// float3 a = normalize(b);
// float a = dot(v1, v2);
// Satuarate() -> saturate() 사용
// float l = length(v);
// struct A{ float a = 1.0f; }; <- 구조체 안에서 초기화 불가
// Vector3(0.0f) -> float3(0.0, 0.0, 0.0) // 실수 뒤에 f 불필요
// Vector4::Transform(v, M) -> mul(v, M)

#define MAX_LIGHTS 3 // 쉐이더에서도 #define 사용 가능
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 1

// 재질
struct Material
{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1; // 16 bytes 맞춰주기 위해 추가
    float3 specular;
    float dummy2;
};

// 조명
struct Light
{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal,
                   float3 toEye, Material mat)
{
    // TODO:
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 specular = mat.specular * pow(max(hdotn, 0.0f), mat.shininess);
    
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, Material mat, float3 normal,
                                float3 toEye)
{
    // TODO:
    float3 lightVec = -L.direction;
    
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;
    
    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal,
                          float3 toEye)
{
    float3 lightVec = L.position - pos;

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);

    // 너무 멀면 조명이 적용되지 않음
    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
        // TODO:
        lightVec /= d;
        
        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
}

float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal,
                         float3 toEye)
{
    float3 lightVec = L.position - pos;

    // 쉐이딩할 지점부터 조명까지의 거리 계산
    float d = length(lightVec);

    // 너무 멀면 조명이 적용되지 않음
    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        // TODO:
        lightVec /= d;
        
        float ndotl = max(dot(lightVec, normal), 0.0f);
        float3 lightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        lightStrength *= att;
        
        float spotFactor = pow(max(-dot(lightVec, L.direction), 0.0f), L.spotPower);
        lightStrength *= spotFactor;
        
        return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
    }
    
    // if에 else가 없을 경우 경고 발생
    // warning X4000: use of potentially uninitialized variable
}

struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float3 normalModel : NORMAL; // 모델 좌표계의 normal    
    float2 texcoord : TEXCOORD0; // <- 다음 예제에서 사용
    
    // float3 color : COLOR0; <- 불필요 (쉐이딩)
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL;
    float2 texcoord : TEXCOORD;
    
    // float3 color : COLOR; <- 불필요 (쉐이딩)
};

```