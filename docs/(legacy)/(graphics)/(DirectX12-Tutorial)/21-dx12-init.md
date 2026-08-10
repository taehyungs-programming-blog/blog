---
layout: default
title: "21. Normal Mapping + Tangent 좌표계"
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

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/16)

---

## 이론

* **Normal Mapping** 이란? : 
    * DirectX의 Normal Map은 3D 그래픽스에서 표면의 디테일과 질감을 향상시키는 데 사용되는 기술입니다. 
    * Normal Map은 표면의 각 픽셀마다 법선 벡터(Normal Vector)의 정보를 담고 있으며, 이를 통해 빛의 반사와 그림자 효과를 더욱 사실적으로 표현할 수 있습니다.
    * 법선 벡터 정보는 빛이 객체에 닿았을 때의 반사 방향을 결정하는 데 사용되며, 이를 통해 표면의 거칠기나 세부 질감을 사실적으로 모방할 수 있습니다.
* 그런데 Normal Mapping만 있으면 접면의 수직한 Vector만 알수 있다.
    * 그럼 수직한 빛의 반사만 계산이 된다는건데 ... 이건 좀 ... -> **Tangent 좌표계**로 다양한 각도의 빛의 반사를 표현해 보자!

---

## 구현 + 설명

* Texture를 사용하기 위해 [3D Textures](https://3dtextures.me/)와 같은 사이트에서 Texture를 다운후 사용하면 생각보다 밋밋하다...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-20-2.png"/>
</p>

* 이유는 Texture에는 Light정보가 없어서 인데 Texture를 통해서 Normal Vector정보를 넘겨 입체감을 주는 표현방식을 **Normal-Mapping(Bump-Mapping)**이라한다.

```cpp
shared_ptr<Mesh> Resources::LoadCubeMesh()
{
	//...

	vector<Vertex> vec(24);
    /*
        struct Vertex
        {
            Vertex() {}

            Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
                : pos(p), uv(u), normal(n), tangent(t)
            {
            }

            Vec3 pos;
            Vec2 uv;
            Vec3 normal;
            Vec3 tangent;
        };
    */

	// 앞면
    // 한쪽면의 텍스쳐 내에 Normal Vector값이 일정하니 빛 반사를 모두 일정하게 하며 평면적으로 보이게 된다.
    // 아래서 세 번째 요소가 Normal Vector이다
	vec[0] = Vertex(Vec3(-w2, -h2, -d2),        // position
                    Vec2(0.0f, 1.0f),           // uv
                    Vec3(0.0f, 0.0f, -1.0f),    // normal
                    Vec3(1.0f, 0.0f, 0.0f));    // tangent(아직 왜쓰는지 안나옴.)
	vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    // 보면 앞면의 Normal-Vector가 모두 Vec3(0.0f, 0.0f, -1.0f)로 동일하다

    // ...
```

* 만약 Cube가 Sphere 처럼 Normal이 다양하다면 입체감이 나타날텐데... 그럼 모든 Texture에 맞춰서 Normal을 세팅해 줘야할까? 이런 노가다가없다...
* 이 문제의 해결책으로 Texture자체에 Normal정보를 입혀둔 Texture가 있다.

* (참고) Normal-Mapping된 Texture를 열어보면 푸른색 이미지가 나타나는데, 왜 그럴까?
    * Normal-Mapping Texture는 Local좌표계를 사용하는 것이 아니라 Tagent 좌표계를 사용하게 된다.
    * Tagent(접하는 평면)의 좌표계란 말인데, 말 그대로 Object의 표면과 접하는 좌표계이다.

* Tagent 좌표계의 좌표는 x(right), y(look), z(up) (순서대로 rgb) 인데 대부분 z(up)으로 향하기에(아무래도 색을 반사하기 위한 좌표계이기에 up성분이 많음 right, look도 약간은 있으나 색을 비스듬히 반사시키려는 정도?)
    * 어떻게 사용될까? -> 일단 현재 Light를 그리는 좌표계가 View좌표계이기에 Tagent 좌표계에서 View좌표계로 변환을 시켜주고 그 Normal Vector를 이용해 Light를 그리면 된다.

* 여기부턴 오히려 코드보는게 편함.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-20-1.png"/>
</p>

* TangentView -> WorldView 좌표변환을 위해서 Binormal Vector를 알아야하는데 Normal이 접한 평면의 수직각의 Vector라면 Tangent Vector와 Normal의 내적이라 생각하면 된다.

```
// Normal-Mapping Texture의 Tangent Space(좌표계)에서 View Space로 옮기려면
// 아래 Matrix를 연산해주면된다.
Tx Ty Tz    (Tangent x, y, z)
Bx By Bz    (Binormal x, y, z)
Nx Ny Nz    (Normal x, y, z)
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene()
{
	// ...

    shared_ptr<Shader> shader = make_shared<Shader>();
    shared_ptr<Texture> texture = make_shared<Texture>();
    shared_ptr<Texture> texture2 = make_shared<Texture>();
    shader->Init(L"..\\Resources\\Shader\\default.hlsli");
    texture->Init(L"..\\Resources\\Texture\\Leather.jpg");
    
    // Normal Texture를 넣는다
    texture2->Init(L"..\\Resources\\Texture\\Leather_Normal.jpg");
```

```cpp
cbuffer MATERIAL_PARAMS : register(b2)
{
    // ...

    // 쉐이더에서는 Texutre의 null체크가 불가능하기에
    // on이라는 변수를 두어 null인지 아닌지 확인한다.
    int     g_tex_on_0;
    int     g_tex_on_1;

    // ...
};

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);       // t1에 normal texture를 넣어줄 예정
Texture2D g_tex_2 : register(t2);

// ...
```

```cpp
void SetTexture(uint8 index, shared_ptr<Texture> texture) 
{ 
    _textures[index] = texture;
    // texture가 없다면 0 
    // 있다면 1로 넣어달라
    _params.SetTexOn(index, (texture == nullptr ? 0 : 1));
}
```

```cpp
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;

    // vertex shader에서 넘어온 normal, tagent, binormal을 넣는다
        // 참고로 여기서 넘어온 n, t, b값은 vertex의 ntb값이지 normaltexture의 ntb값이 아님(주의)
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}
```

```cpp
float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;

        // normal texture로 nbt를 구해서 연산
            // TBN 순서로 생성된 Matrix를 곱해주게 된다.
        float3x3 matTBN = { input.viewTangent, 
                            input.viewBinormal, 
                            input.viewNormal };
                            /*
                                Tx Ty Tz    (Tangent x, y, z)
                                Bx By Bz    (Binormal x, y, z)
                                Nx Ny Nz    (Normal x, y, z)
                            */
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    LightColor totalColor = (LightColor)0.f;

    for (int i = 0; i < g_lightCount; ++i)
    {
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