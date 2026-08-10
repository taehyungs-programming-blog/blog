---
layout: default
title: "33. Terrain"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/27)

## 이론

* **Terrain** - 지형정보를 어떻게 그릴지, Tessellation을 이용하게 된다.
    * Texture를 이용해 Terrain을 만들고자 한다.

---

## 구현

```cpp
shared_ptr<Mesh> Resources::LoadTerrainMesh(int32 sizeX, int32 sizeZ)
{
	vector<Vertex> vec;

	for (int32 z = 0; z < sizeZ + 1; z++)
	{
		for (int32 x = 0; x < sizeX + 1; x++)
		{
			Vertex vtx;
			vtx.pos = Vec3(static_cast<float>(x), 0, static_cast<float>(z));
			vtx.uv = Vec2(static_cast<float>(x), static_cast<float>(sizeZ - z));
			vtx.normal = Vec3(0.f, 1.f, 0.f);
			vtx.tangent = Vec3(1.f, 0.f, 0.f);

			vec.push_back(vtx);
		}
	}

	vector<uint32> idx;

	for (int32 z = 0; z < sizeZ; z++)
	{
		for (int32 x = 0; x < sizeX; x++)
		{
			//  [0]
			//   |	\
			//  [2] - [1]
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z)+(x));
			//  [1] - [2]
			//   	\  |
			//		  [0]
			idx.push_back((sizeX + 1) * (z)+(x + 1));
			idx.push_back((sizeX + 1) * (z + 1) + (x));
			idx.push_back((sizeX + 1) * (z + 1) + (x + 1));
		}
	}

	shared_ptr<Mesh> findMesh = Get<Mesh>(L"Terrain");
	if (findMesh)
	{
		findMesh->Init(vec, idx);
		return findMesh;
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->Init(vec, idx);
	Add(L"Terrain", mesh);
	return mesh;
}
```

* 위코드는 아래와 같은 Terrain을 만들고 있다고 생각하면 된다.
    * 그냥 무수히 많은 네모를 그림.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-33-1.jpg"/>
</p>

### Shader 수정

```cpp
// [Terrain Shader]
// g_int_1      : TileX
// g_int_2      : TileZ
// g_float_0    : Max Tessellation Level
// g_vec2_0     : HeightMap Resolution
// g_vec2_1     : Min/Max Tessellation Distance
// g_vec4_0     : Camera Position
// g_tex_0      : Diffuse Texture
// g_tex_1      : Normal Texture
// g_tex_2      : HeightMap Texture

// Constant HS
    // 카메라와 거리에 따라 정점의 개수를 조절한다
PatchTess ConstantHS(InputPatch<VS_OUT, 3> input, int patchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess)0.f;

    float minDistance = g_vec2_1.x; // Tesslation이 적용되는 최소 거리
    float maxDistance = g_vec2_1.y; // 최대 거리

    /*

    * 정점과 Edge 번호 *

       [0]
        |\
        |  \
    (1) |   \  (2)
        |    \
        |     \
     [2] ------- [1]
           (0)
    */

    float3 edge0Pos = (input[1].pos + input[2].pos) / 2.f;
    float3 edge1Pos = (input[2].pos + input[0].pos) / 2.f;
    float3 edge2Pos = (input[0].pos + input[1].pos) / 2.f;

    edge0Pos = mul(float4(edge0Pos, 1.f), g_matWorld).xyz;
    edge1Pos = mul(float4(edge1Pos, 1.f), g_matWorld).xyz;
    edge2Pos = mul(float4(edge2Pos, 1.f), g_matWorld).xyz;

    float edge0TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge0Pos, minDistance, maxDistance, 4.f);
    float edge1TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge1Pos, minDistance, maxDistance, 4.f);
    float edge2TessLevel = CalculateTessLevel(g_vec4_0.xyz, edge2Pos, minDistance, maxDistance, 4.f);

    output.edgeTess[0] = edge0TessLevel;
    output.edgeTess[1] = edge1TessLevel;
    output.edgeTess[2] = edge2TessLevel;
    output.insideTess = edge2TessLevel;

    return output;
}
```

```cpp
float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);

    if (distance < min)
        return maxLv;
    if (distance > max)
        return 1.f;

    float ratio = (distance - min) / (max - min);
    float level = (maxLv - 1.f) * (1.f - ratio);
    return level;
}
```

```cpp
[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> input, float3 location : SV_DomainLocation, PatchTess patch)
{
    DS_OUT output = (DS_OUT)0.f;

    float3 localPos = input[0].pos * location[0] + input[1].pos * location[1] + input[2].pos * location[2];
    float2 uv = input[0].uv * location[0] + input[1].uv * location[1] + input[2].uv * location[2];

    int tileCountX = g_int_1;
    int tileCountZ = g_int_2;
    int mapWidth = g_vec2_0.x;
    int mapHeight = g_vec2_0.y;

    // uv좌표계를 0~1사이로 만들어준다.
    float2 fullUV = float2(uv.x / (float)tileCountX, uv.y / (float)tileCountZ);
    float height = g_tex_2.SampleLevel(g_sam_0, fullUV, 0).x;
        // SampleLevel - Pixel Shader에서는 Sample을 사용
        // 그 외의 Shader에서는 SampleLevel을 사용

    // 높이맵 높이 적용
    localPos.y = height;

    float2 deltaUV = float2(1.f / mapWidth, 1.f / mapHeight);
    float2 deltaPos = float2(tileCountX * deltaUV.x, tileCountZ * deltaUV.y);

    float upHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x, fullUV.y - deltaUV.y), 0).x;
    float downHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x, fullUV.y + deltaUV.y), 0).x;
    float rightHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x + deltaUV.x, fullUV.y), 0).x;
    float leftHeight = g_tex_2.SampleLevel(g_sam_0, float2(fullUV.x - deltaUV.x, fullUV.y), 0).x;

    float3 localTangent = float3(localPos.x + deltaPos.x, rightHeight, localPos.z) - float3(localPos.x - deltaPos.x, leftHeight, localPos.z);
    float3 localBinormal = float3(localPos.x, upHeight, localPos.z + deltaPos.y) - float3(localPos.x, downHeight, localPos.z - deltaPos.y);

    output.pos = mul(float4(localPos, 1.f), g_matWVP);
    output.viewPos = mul(float4(localPos, 1.f), g_matWV).xyz;

    output.viewTangent = normalize(mul(float4(localTangent, 0.f), g_matWV)).xyz;
    output.viewBinormal = normalize(mul(float4(localBinormal, 0.f), g_matWV)).xyz;
    output.viewNormal = normalize(cross(output.viewBinormal, output.viewTangent));

    output.uv = uv;

    return output;
}

```

```cpp
PS_OUT PS_Main(DS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0 == 1)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(viewNormal.xyz, 0.f);
    output.color = color;

    return output;
}
```

---

## Shader에 필요한 값을 채워주자

```cpp
void Terrain::Init(int32 sizeX, int32 sizeZ)
{
	_sizeX = sizeX;
	_sizeZ = sizeZ;

	_material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");

	_material->SetInt(1, _sizeX);
	_material->SetInt(2, _sizeZ);
	_material->SetFloat(0, _maxTesselation);

	shared_ptr<Texture> heightMap = GET_SINGLE(Resources)->Load<Texture>(L"HeightMap", L"..\\Resources\\Texture\\Terrain\\height.png");
	Vec2 v = Vec2(heightMap->GetWidth(), heightMap->GetHeight());
	_material->SetVec2(0, Vec2(heightMap->GetWidth(), heightMap->GetHeight()));
	_material->SetVec2(1, Vec2(1000.f, 5000.f));
	_material->SetTexture(2, heightMap);

	shared_ptr<MeshRenderer> meshRenderer = GetGameObject()->GetMeshRenderer();
	{
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadTerrainMesh(sizeX, sizeZ);
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"Terrain");
		meshRenderer->SetMaterial(material);
	}
}
```

---

```cpp
	// Terrain
	{
		ShaderInfo info =
		{
			SHADER_TYPE::DEFERRED,
			RASTERIZER_TYPE::CULL_BACK,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\terrain.fx", info, arg);
		Add<Shader>(L"Terrain", shader);
	}
}
```