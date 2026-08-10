---
layout: default
title: "14. Height map"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/14/14.HeightMap)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-14-1.png"/>
</p>

* 구의 가장 외각은 Normal map이 적용안된거 처럼 보인다
* 적용해 보자 -> height map
	* 참고로 height mapping은 vertex shader에서 이루어 진다
* (참고) AmbientOcclusion
	* map을 만들어 미리 빛이 잘 들어오지 않는 부분을 표시해 둔다.
	* 이것도 같이 구현예정!

---

```cpp
PixelShaderInput main(VertexShaderInput input)
{
    // ...
    
    if (useHeightMap)
    {
        float height = g_heightTexture.SampleLevel(g_sampler, input.texcoord, 0).r;
        height = height * 2.0 - 1.0;
        pos += float4(output.normalWorld * height * heightScale, 0.0);
    }
```

* 바닥도 적용해 보자.

```cpp
// Ground
{
    // MeshData ground = GeometryGenerator::MakeSquare(20.0f, {40.0f, 40.0f});
    // MeshData ground = GeometryGenerator::MakeSquareGrid(1, 1, 20.0f, {40.0f, 40.0f});

    // 바닥의 그리드를 높여준다
    MeshData ground = GeometryGenerator::MakeSquareGrid(4096, 4096, 20.0f, {40.0f, 40.0f});
```


- 마지막으로 lod까지 적용하면 완벽!

```cpp
PixelShaderOutput main(PixelShaderInput input)
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float dist = length(eyeWorld - input.posWorld);
    float distMin = 3.0;
    float distMax = 10.0;
    float lod = 10.0 * saturate(dist / (distMax - distMin));
    //float lod = 0.0;
```

{% endraw %}