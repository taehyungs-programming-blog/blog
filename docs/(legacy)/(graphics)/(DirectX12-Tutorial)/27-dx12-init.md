---
layout: default
title: "27. Deferred Rendering"
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

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/21)

* 지난강에서 해결하지 못한 빛의 처리에 대한 문제를 해결해보자

```cpp
float4 PS_Main(VS_OUT input) : SV_Target
{
    // ...

    // 문제는 이 빛의 처리인데
    // 현재는 해당 오브젝트에 빛을 연산해야하는지 말아야하는지 쉐이더 단계에서 알수 없기에 모든 빛을 계산해야만 했다.
    LightColor totalColor = (LightColor)0.f;

    for (int i = 0; i < g_lightCount; ++i)
    {
        // 여기서 해당 픽셀에 오브젝트가 있는지 없는지 판별이 불가능.
         LightColor color = CalculateLightColor(i, viewNormal, input.viewPos);
         totalColor.diffuse += color.diffuse;
         totalColor.ambient += color.ambient;
         totalColor.specular += color.specular;
    }

    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;
     return color;
}
```

* 대략적 해결방법은 쉐이딩 과정에서 Position, Normal, Dissuse 정보를 C++코드로 넘겼었다. 
* 받은 Position 정보를 토대로 해당 Pixel에 Object가 있으면 Light연산을 수행하고 없으면 무시하는 형태로 구현하려고 한다.
* 역시 설명은 너무 어렵다... 코드를 보자

```cpp
// forwar.fx

// 들어가기 전 몇가지를 추가해줄 건데 우선 Light의 영향을 받지 않는 쉐이더를 만들어 주자
struct VS_TEX_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEX_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_TEX_OUT VS_Tex(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Tex(VS_TEX_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);
    return color;
}
```

```cpp
// lighting.fx

// [Directional Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// Mesh : Rectangle

VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    // Directional Light의 경우 전체화면에 영향을 주어야 하기에
    output.pos = float4(input.pos * 2.f/*Rectange의 크기가 0.5이기에 2를 곱한다*/, 1.f);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_DirLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;
    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);       // 카메라 뒤의 물체(Pixel)는 clip(생략)
    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);
    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}
```

```cpp
// lighting.fx

// [Point Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_vec2_0 : RenderTarget Resolution
// Mesh : Sphere

VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;
    // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);
    float3 viewPos = g_tex_0.Sample(g_sam_0, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);       // 카메라 뒤의 물체(Pixel)는 clip(생략)

    int lightIndex = g_int_0;
    float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
    float distance = length(viewPos - viewLightPos);
    if (distance > g_light[lightIndex].range)
        clip(-1);       // 광원과의 거리를 체크해서 생략할지 판별

    float3 viewNormal = g_tex_1.Sample(g_sam_0, uv).xyz;
    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);
    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}
```

```cpp
// lighting.fx

// [Final]
// g_tex_0 : Diffuse Color Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// Mesh : Rectangle

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;
    float4 lightPower = g_tex_1.Sample(g_sam_0, input.uv);
    if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
        clip(-1);

    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 specular = g_tex_2.Sample(g_sam_0, input.uv);
    output = (color * lightPower) + specular;
    return output;
}
```

* 이제 쉐이더가 동작하게 C++코드를 수정해야한다

```cpp
// 쉐이더의 생성은 아래와 같이 생성

void Resources::CreateDefaultShader()
{
	// Skybox
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::LESS_EQUAL
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\skybox.fx", info);
/*
    void Shader::Init(const wstring& path, ShaderInfo info, const string& vs, const string& ps)
    {
        _info = info;

        CreateVertexShader(path, vs, "vs_5_0");
        CreatePixelShader(path, ps, "ps_5_0");

        D3D12_INPUT_ELEMENT_DESC desc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

    // ...
*/
		Add<Shader>(L"Skybox", shader);
	}

	// Deferred (Deferred)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\deferred.fx", info);
		Add<Shader>(L"Deferred", shader);
	}

	// Forward (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\forward.fx", info);
		Add<Shader>(L"Forward", shader);
	}

	// Texture (Forward)
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
        // 쉐이더에서 Vertex Shader 시작점, Pixel Shader시작점을 알려줄수 있음.
		shader->Init(L"..\\Resources\\Shader\\forward.fx", info, "VS_Tex", "PS_Tex");
		Add<Shader>(L"Texture", shader);
	}

	// DirLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\lighting.fx", info, "VS_DirLight", "PS_DirLight");
		Add<Shader>(L"DirLight", shader);
	}

	// PointLight
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_NONE,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
			BLEND_TYPE::ONE_TO_ONE_BLEND
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\lighting.fx", info, "VS_PointLight", "PS_PointLight");
		Add<Shader>(L"PointLight", shader);
	}

	// Final
	{
		ShaderInfo info =
		{
			SHADER_TYPE::LIGHTING,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE,
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"..\\Resources\\Shader\\lighting.fx", info, "VS_Final", "PS_Final");
		Add<Shader>(L"Final", shader);
	}
}
```

* UI 는 어떻게 생성될까?

```cpp
#pragma region UI_Test
	for (int32 i = 0; i < 5; i++)
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 160), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");
/*
    // 쉐이더는 위에서 생성한 Texture를 사용
    // Texture (Forward)
    {
        ShaderInfo info =
        {
            SHADER_TYPE::FORWARD,
            RASTERIZER_TYPE::CULL_NONE,
            DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE
        };

        shared_ptr<Shader> shader = make_shared<Shader>();
        // 쉐이더에서 Vertex Shader 시작점, Pixel Shader시작점을 알려줄수 있음.
        shader->Init(L"..\\Resources\\Shader\\forward.fx", info, "VS_Tex", "PS_Tex");
        // [ Start - forwar.fx ]
            // 딱해 해주는 처리는 없음
            VS_TEX_OUT VS_Tex(VS_TEX_IN input)
            {
                VS_TEX_OUT output = (VS_TEX_OUT)0;

                output.pos = mul(float4(input.pos, 1.f), g_matWVP);
                output.uv = input.uv;

                return output;
            }

            float4 PS_Tex(VS_TEX_OUT input) : SV_Target
            {
                float4 color = float4(1.f, 1.f, 1.f, 1.f);
                if (g_tex_on_0)
                    color = g_tex_0.Sample(g_sam_0, input.uv);

                return color;
            }
        // [ End - forwar.fx ]
        Add<Shader>(L"Texture", shader);
    }
*/

            // 각 UI에 texture를 매칭해주는데
			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
/*
    // Render Target Group에서 관리중인 RenderTarget을 받는다
    class RenderTargetGroup
    {
        // ...
        shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
        // ...
        vector<RenderTarget>			_rtVec;
*/

/*
    // Render Target Group의 생성은
    void Engine::CreateRenderTargetGroups()
    {
        // ...

        // Lighting Group
        {
            vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

            rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget",
                DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
                CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

            rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget",
                DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
                CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

            _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
            _rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING, rtVec, dsTexture);
        }
*/

/*
    // Texture 생성
    shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
        const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
    {
        shared_ptr<Texture> texture = make_shared<Texture>();
        texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
        Add(name, texture);

        return texture;
    }
*/

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion
```
