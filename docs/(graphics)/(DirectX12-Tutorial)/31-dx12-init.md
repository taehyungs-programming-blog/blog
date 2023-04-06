---
layout: default
title: "31. Shadow Mapping"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/25)

* **Shadow Mapping** - 사실상 원리는 간단하다. 빛의 기준에서 카메라를 만들고 그 카메라에 찍히지(깊이 값으로 계산) 않는 부분을 그림자로 처리하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-31-1.png"/>
</p>

* 빛에 카메라를 하나 두고, 찍히는 화면을 WorldViewProjMatrix를 곱해주고 ClipPos이후 W나누고 Projection 좌표계로 이동 후 z값을 구하면 Depth가 된다.(그냥 코드로 보는게 마음편함)
* 말은 어렵지만 크게 두 부분으로 나뉜다고 생각하면 된다. 

* 빛의 카메라에서 찍히는 화면(그림자를 어디에 그릴지 판별)
* 실제 카메라에서 찍히는 화면(그림자를 어떻게 그릴지 판별)
* 두 카메라의 코드가 어떻게 구현되는지 보면되는 일.

---

```cpp
// 우선 쉐도우의 shader를 먼저 보자면

#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"

struct VS_IN
{
    float3 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;

    // WVP Matrix를 곱해주어 clip pos를 만들고
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.clipPos = output.pos;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    // 실제로 그림자여부를 판별시 z(depth)에 clippos의 w를 나눠준다.
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

#endif
```

```cpp
// lighting.fx

// 우선 그림자는 Directional Light에만 적용하자
PS_OUT PS_DirLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    // 그림자
    if (length(color.diffuse) != 0)
    {
        // depth를 구한다.
        matrix shadowCameraVP = g_mat_0;
        float4 worldPos = mul(float4(viewPos.xyz, 1.f), g_matViewInv);
        float4 shadowClipPos = mul(worldPos, shadowCameraVP);
        float depth = shadowClipPos.z / shadowClipPos.w;

        // uv 좌표계를 구한다.
        // x [-1 ~ 1] -> u [0 ~ 1]
        // y [1 ~ -1] -> v [0 ~ 1]
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5 + 0.5;

        if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
        {
            // depth가 있고
            // Shadow Texture와 비교후 그림자를 반영
            float shadowDepth = g_tex_2.Sample(g_sam_0, uv).x;
            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
            {
                color.diffuse *= 0.5f;
                color.specular = (float4) 0.f;
            }
        }
    }

    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}
```

* 이제 C++을 보자.

```cpp
class Light : public Component
{
  // ...

  // Light에 Shadow용 카메라를 배치
	shared_ptr<GameObject> _shadowCamera;
};
```

```cpp
Light::Light() : Component(COMPONENT_TYPE::LIGHT)
{
	_shadowCamera = make_shared<GameObject>();
	_shadowCamera->AddComponent(make_shared<Transform>());
	_shadowCamera->AddComponent(make_shared<Camera>());
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	_shadowCamera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음
}
```

```cpp
void Light::FinalUpdate()
{
	_lightInfo.position = GetTransform()->GetWorldPosition();

  // light와 동일한 좌표로 맞춘다.
	_shadowCamera->GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition());
	_shadowCamera->GetTransform()->SetLocalRotation(GetTransform()->GetLocalRotation());
	_shadowCamera->GetTransform()->SetLocalScale(GetTransform()->GetLocalScale());

	_shadowCamera->FinalUpdate();
}
```

```cpp
void Light::RenderShadow()
{
  // static하지 않은 그림자의 영향을 받을 애들만 정렬
	_shadowCamera->GetCamera()->SortShadowObject();

  // 소팅된 오브젝트를 그린다.
	_shadowCamera->GetCamera()->Render_Shadow();
  /*
    void MeshRenderer::RenderShadow()
    {
      GetTransform()->PushData();
      GET_SINGLE(Resources)->Get<Material>(L"Shadow")->PushGraphicsData();
      _mesh->Render();
    }
  */
}
```

```cpp
void Engine::CreateRenderTargetGroups()
{
	// ...

	// Shadow Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"ShadowTarget",
			DXGI_FORMAT_R32_FLOAT, 4096, 4096,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		shared_ptr<Texture> shadowDepthTexture = GET_SINGLE(Resources)->CreateTexture(L"ShadowDepthStencil",
			DXGI_FORMAT_D32_FLOAT, 4096, 4096,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTexture);
	}
```