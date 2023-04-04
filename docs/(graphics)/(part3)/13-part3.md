---
layout: default
title: "13. Normal Mapping"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/13/13.NormalMapping)

* 용어정리
    * Albedo(diffuse) - Texture중 object의 기본색상을 그리는 Texture
    * Normal Map - Normal Vector Texture

---

* 우선 normal vector 활용 Texture위에 반구를 그려보자

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);

    int i = 0;

    float3 normalWorld = input.normalWorld;
    
    if (useNormalMap) // NormalWorld를 교체
    {
        // float3 normalTex = g_normalTexture.SampleLevel(g_sampler, input.texcoord, 0.0).rgb;
        // normalTex = 2.0 * normalTex - 1.0; // 범위 조절 [-1.0, 1.0]

        // 반지름이 0.3이고 원점이 (0.5, 0.5, 0.3)인 구의 normal만 이용해서 렌더링 해보기
        // x가 u이고 y가 v라면 z는 sqrt(0.3*0.3 - x*x - y*y)
        // z의 방향 주의
        
        float3 center = float3(0.5, 0.5, 0.3);
        float2 f = input.texcoord - center.xy;
        float f2 = dot(f, f);
        if(f2 < 0.3 * 0.3)
        {
            float z = sqrt(0.3 * 0.3 - f2);
            float3 p = float3(f * float2(1, -1), -z);
            normalWorld = normalize(p);
        }
        else
        {
            normalWorld = float3(0, 0, -1);
        }
    }
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-13-1.png"/>
</p>

---

* 이제 normal map 이미지를 사용해 보자... 하기 전! 알아야 할 내용이 있다.

* 사용하는 Vertex를 보면 tangent가 추가됐음을 알수 있는데
    * 이 tangent가 뭔지 알아야 한다.

```cpp
struct Vertex {
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;
    //Vector3 biTangentModel;
    // biTangent는 쉐이더에서 계산
};
```

```cpp
void BasicMeshGroup::Initialize(ComPtr<ID3D11Device> &device,
                                ComPtr<ID3D11DeviceContext> &context,
                                const std::vector<MeshData> &meshes) {

    //...

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
```

---

### tangent에 대한 설명

* 예를들어 사각형이 있다고 가정하자

```cpp
MeshData GeometryGenerator::MakeSquare(const float scale,
                                       const Vector2 texScale) {
    vector<Vector3> positions;
    vector<Vector3> colors;
    vector<Vector3> normals;
    vector<Vector2> texcoords; // 텍스춰 좌표

    // 앞면
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
```

```
<<Texture>>
------------------------------>   tangent
|
|
|
|
|
|

 bi-tangent
```

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);

    int i = 0;

    float3 normalWorld = input.normalWorld;
    
    if (useNormalMap) // NormalWorld를 교체
    {
        float3 normalTex = g_normalTexture.SampleLevel(g_sampler, input.texcoord, 0.0).rgb;
        normalTex = 2.0 * normalTex - 1.0; // 범위 조절 [-1.0, 1.0]
        
	// 아래 화면처럼 Texture가 고정인 경우를 가정했기에
        float3 T = float3(1, 0, 0); // Tangent는 x방향이기에
        float3 B = float3(0, -1, 0); // Bi-Tangent는 -y방향이기에
        float3 N = float3(0, 0, -1); // Normal은 -z방향이기에
        
        // TBN을 변환행렬로 만든다
        
        float3x3 TBN = float3x3(T, B, N);
        
        // Sampling한 Texture를 TBN으로 변환
        normalWorld = normalize(mul(normalTex, TBN));
    }
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-13-2.png"/>
</p>

---

* 그럼 Texture가 화면에 고정되지 않은 일반적 케이스를 고려해 보자.

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);

    int i = 0;

    float3 normalWorld = input.normalWorld;
    
    if (useNormalMap) // NormalWorld를 교체
    {
        float3 normalTex = g_normalTexture.SampleLevel(g_sampler, input.texcoord, 0.0).rgb;
        normalTex = 2.0 * normalTex - 1.0; // 범위 조절 [-1.0, 1.0]
        
        float3 N = normalWorld;
        // Tangent와 Normal이 정확히 수직이 아닐수 있기에 Normal 성분을 뺀다
        float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
        float3 B = cross(N, T);
```

{% endraw %}