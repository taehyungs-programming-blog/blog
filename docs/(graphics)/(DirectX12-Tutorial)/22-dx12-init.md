---
layout: default
title: "22. Sky Box(하늘표현)"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/17)

* 하늘을 구현하려면 어떻게 해야할까?

* 몇 가지 고려사항이 있는데
    * 캐릭터가 움직여서 하늘의 끝에 닿으면 안된다.
    * 하늘뒤에 어떠한 물체도 놓이게 해선 안된다(그럴필요가 없으니)
* 직접적 구현의 팁은
    * - 하늘의 위치는 카메라의 기준(0, 0, 0)과 동일 👉 이렇게 처리해야 카메라(유저)가 이동하면 하늘도 같이 이동하는 효과가 나타난다
    * 이렇게 생각하면 오히려 편하다. Sky는 결국 View기준에서 원점이기에 View의 원점좌표 기준으로 Sky를 넣으면 된다. (그냥 원점에 두면 된다는 말.)
    * SRT(Scale, Rotation, Translation)중 Scale, Translation은 적용하지 않고(어차피 카메라와 같은 위치에 있어야 하기에) Rotation만 적용이 되게 만들면 된다.
    * Local Space에서 World는 무시하고 View에서도 Rotation만 적용하면 된다.

* 우선 이 정도만 이해하고 직접 코드를 보는게 편하다

---

* 들어가기 전 알아야 할 개념이 있다.

## Culling

* **Culling** : 랜더링 과정에서 그릴지 말지를 결정(카메라에 찍힐 부분을 제외하고 필요없는 부분은 그리지 말자는 명령)
* 우선 이런 Culling이 왜 필요한가? -> 랜더링이란게 연산이 많이 필요하다. `Vertex`의 연산 `Pixel`사이의 `Rasterizer`연산등… 이런 연산을 조금이라도 줄이기 위해 **Culling**의 개념이 필요하다
* 조금만 더 설명을 붙이면 Vertex정보를 토대로 Pixel Shader를 통해 그리는데 Vertext정보에서 Rasterizer를 통해 그리지 않아도 되는 부분(Culling)은 생략된다고 생각하면 편하다
	* 추가고 Rasterizer는Vertex를 Pixel Shader로 넘길지 Vertex를 추가할지 등을 결정하게 된다.
* **Culling이 되었다** - `Rasterizer`단계에서 생략되어 그려지지 않게 되었다로 해석하면 되겠다

* 랜더링 파이프라인에서 아래와 같이 `CD3DX12_RASTERIZER_DESC`옵션을 넣는데 옵션의 의미는 아래와 같다

```cpp
void Shader::Init(const wstring& path, ShaderInfo info)
{
	// ..

    // CD3DX12_RASTERIZER_DESC의 정확한 의미는 아래 참조
	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// ...
```

```cpp
explicit CD3DX12_RASTERIZER_DESC( CD3DX12_DEFAULT )
{
	FillMode = D3D12_FILL_MODE_SOLID;
	CullMode = D3D12_CULL_MODE_BACK;		// 반시계 방향의 인덱스는 그리지 않겠다
	FrontCounterClockwise = FALSE;
	// Front를 시계 방향(Clockwise)로 둔다
	// 반 시계 방향(CounterClockwise)는 Culling(랜더링에서 그리지 않겠다.)

    /*
        // 우리가 넣은 큐브를 보자면
        // 앞면
        vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
        vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
        vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
        vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

        // 인덱스를 시계방향으로 넣고있음을 보여주는데
        [1][-w2, +h2] ------------------- [2][+w2, +h2]
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
        [0][-w2, -h2] -------------------- [3][+w2, -h2]

        // ...

        // 앞면
        idx[0] = 0; idx[1] = 1; idx[2] = 2;
        idx[3] = 0; idx[4] = 2; idx[5] = 3;
    */


	// ...
```

* 갑자기 culling은 왜 언급하나?? 👉 
    * 하늘(skybox)는 카메라(캐릭터) 입장에서는 큰 box(skybox안)에서 그려줘야한다.
    * 따라서 culling이 되지 않게 구현하기 위해선 인덱스 방향을 반대로 줘야 하늘이 나타난다.

```cpp
// 인덱스 버퍼 시계/반시계 방향에 따라 culling여부 판별
enum class RASTERIZER_TYPE
{
	CULL_NONE,      // 모두하고 모두 그려주겠다
	CULL_FRONT,
	CULL_BACK,      // 반 시계방향은 무시하겠다
	WIREFRAME,      // 일단은 무시
};

// depth stencil 비교연산 처리방법 결정
// 같은 최대 깊이더라도 하늘을 마지막에 그려야함.
enum class DEPTH_STENCIL_TYPE
{
	LESS,           // Depth가 작을때 그려준다
	LESS_EQUAL,     // Depth가 같더라도 그려준다(Sky의 경우 1로 둘 예정인데 그냥 1로 두면 무시됨. 이 옵션을 넣어야함)
	GREATER,
	GREATER_EQUAL,
};
```

```cpp
void Shader::Init(const wstring& path, ShaderInfo info)
{
	//..

	switch (info.rasterizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
        // (참고) D3D12_FILL_MODE_SOLID - 색을 모두 채운다 (일반적 모드)
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
        // (참고) WIREFRAME - 외각선만 나타나게 한다.
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (info.depthStencilType)
	{
    case DEPTH_STENCIL_TYPE::LESS:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;

        // 기본은 Depth가 낮을수록(멀리있을수록) 안그려주게되는데
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:

        // 하늘은 무조건 그려야하니 D3D12_COMPARISON_FUNC_LESS_EQUAL로 둔다
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	}

	DEVICE->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));

    // ...
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
	scene->AddGameObject(camera);
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shared_ptr<Texture> texture = make_shared<Texture>();
            // 쉐이더를 skybox.hlsli를 쓰고있음을 주목.
			shader->Init(L"..\\Resources\\Shader\\skybox.hlsli",
				{ RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::LESS_EQUAL });
			// 쉐이더 init시 RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::LESS_EQUAL 옵션을 넣는다.
			// RASTERIZER_TYPE::CULL_NONE : cull을 하지말라 (cull_front로 해도 동작함)
			texture->Init(L"..\\Resources\\Texture\\Sky01.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

    // ...
```

```cpp
// skybox.hlsli

#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "params.hlsli"

struct VS_IN
{
    float3 localPos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // float4(input.localPos, 0) : Translation은 하지 않고 Rotation만 적용한다(마지막 값을 0으로 둠)
    float4 viewPos = mul(float4(input.localPos, 0), g_matView);

	/*
        // 참고 View Matrix는 아래와 같고 right/up/look . x/y/z는 단위벡터
        // c*right/up/look 은 Translation 정보이다.
        // 따라서 마지막 항에 0을 넣으면(input.localPos, 0) Translation 정보는 사라지고
        // Rotation정보만 남게된다.
        right.x  up.x  look.x  0
        right.y  up.y  look.y  0
        right.z  up.z  look.z  0
        -c*right -c*up -c*look 1
	*/

    float4 clipSpacePos = mul(viewPos, g_matProjection);

	// clipSpacePos.xyww : z(깊이)에 w를 넣는다
    // w/w=1이기 때문에 항상 깊이가 1로 유지된다
    output.pos = clipSpacePos.xyww;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
     float4 color = g_tex_0.Sample(g_sam_0, input.uv);
     return color;
}

#endif
```