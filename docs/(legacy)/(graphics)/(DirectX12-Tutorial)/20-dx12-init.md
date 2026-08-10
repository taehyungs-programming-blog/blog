---
layout: default
title: "20. [구현] Lighting Shader"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/15)

* (Q) Light는 어느 좌표계에서 반영해야할까? World? View?
    * 어느 좌표계든 상관없다.
    * 단, Normal Vetor를 World좌표계로 했다면
    * Tangent Vector역시 World좌표계로 통일해야한다 정도만 주의하면 된다.
* 보통은 View좌표계에서 많이 Light을 계산한다

---

* Light의 Pixel Shader에 관한 부분

```cpp
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    // ViewPosition(좌표)와 NormalVector을 Output으로 넘긴다
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 색상을 초기화
    // 흰색으로 초기화 한 이유는 빛을 잘 받는지 확인하기 위함.
    float4 color = float4(1.f, 1.f, 1.f, 1.f);

    LightColor totalColor = (LightColor)0.f;
    /*
        struct LightColor
        {
            float4      diffuse;
            float4      ambient;
            float4      specular;
        };
    */

    for (int i = 0; i < g_lightCount/* 지원하는 Light의 개수 */; ++i)
    {
        // CalculateLightColor(빛 정보 연산)는 아래에 있음 참고
         LightColor color = CalculateLightColor(i, input.viewNormal, input.viewPos);
         totalColor.diffuse += color.diffuse;
         totalColor.ambient += color.ambient;
         totalColor.specular += color.specular;
    }

    // diffuse, ambient는 자신의 색상 (color.xyz)에 영향을 미치는(곱해지는) 방식이고
    // specular는 전반적으로 더해지는 형식
    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;

     return color;
}
```

```cpp
#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

LightColor CalculateLightColor(int lightIndex,      // 몇 번째 빛인가?
                                float3 viewNormal,  // (View Position 기준) Normal Vector
                                float3 viewPos)     // (View Position 기준) Pixel 위치
{
    LightColor color = (LightColor)0.f;

    float3 viewLightDir = (float3)0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;


    // lightType이 Directional Light일경우
    if (g_light[lightIndex].lightType == 0)
    {
        // 들어오는 빛 자체도 View 좌표계로 변환한다.
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));
        // diffuse 구할때 들어온 빛(L), NormalVector의 내적임을 기억하자
            // dot(-viewLightDir, viewNormal) : 내적 후
            // saturate : 0 ~ 1 사이 값으로 변경(음수제거)
                // 음수가 있나 싶겠지만, 빛이 거꾸로(아래서 위로, 가려진 부분) 올경우 음수가 된다.

        /*
              (L)  (N)
                \   |
                 \  |
                  \ |
                   \|
        -----------------------------
     ** -L과 N내적시 cos(a)가 나오게 됨 **
        */
    }

    // lightType이 Point Light일경우
    else if (g_light[lightIndex].lightType == 1)
    {
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        // 거리값에 대해 빛의 세기 반영
        float dist = distance(viewPos, viewLightPos);
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }

    // lightType이 Spot Light일경우
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;

            float3 viewLightVec = viewPos - viewLightPos;
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    // specular 계산
    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));

    /*
          (L)  (N)    (R)-> Relection(정반사)
            \   |    /
             \  |   /
              \ |  /
               \| /
    -----------------------------
    ** -L과 N의 외적의 결과를 2번 더하면 Reflection이 나온다 **
    */
    // 계산된 Reflection을 카메라로 반영해 줘야하는데
    // Reflection과 카메라의 방향을 내적해 주면 Specular 계산완료
    float3 eyeDir = normalize(viewPos);
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);  // 좀 더 극적인 효과를 위해서 pow를 넣음.
    // pow값을 변경해 가면서 변화를 확인해 보자.(pow가 없으면 specular가 너무 크게 적용돼 오히려 어색해진다)

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
}


#endif
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-19-1.png"/>
</p>