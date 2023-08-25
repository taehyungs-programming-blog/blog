---
layout: default
title: "14. Map Tessellation"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/12)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-14-1.png"/>
</p>

* 뭔가 싶겠지만 당겨보면 Vertex해상도가 높아지고 멀어지면 해당도가 낮아진다
    * LOD(Level Of Detail)이라 설명하면 편하다

```cpp
void BasicTessellation::DrawScene()
{
	// ...

	_deviceContext->IASetInputLayout(InputLayouts::Pos.Get());

    // tessellation은 topology를 4로 두어 사각형으로 부터 시작한다
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    // ...
```

* 사실상 쉐이더만 봐도 된다

```cpp
// Hull Shader는 4개의 Patch를 받는다

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	float3 centerL = 0.25f * (patch[0].PosL + patch[1].PosL + patch[2].PosL + patch[3].PosL);
	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;

    // d - world 좌표에서 카메라까지 거리
	float d = distance(centerW, gEyePosW);

	// Tessellate the patch based on distance from the eye such that
	// the tessellation is 0 if d >= d1 and 60 if d <= d0.  The interval
	// [d0, d1] defines the range we tessellate in.

    // 거리는 20일때 최소 100일때 최대
	const float d0 = 20.0f;
	const float d1 = 100.0f;
	float tess = 64.0f * saturate((d1 - d) / (d1 - d0));

	// Uniformly tessellate the patch.

    // 얼마나 정밀하게 자를지 ?
	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;
	pt.EdgeTess[3] = tess;

	pt.InsideTess[0] = tess;
	pt.InsideTess[1] = tess;

	return pt;
}
```

```cpp
struct HullOut
{
	float3 PosL : POSITION;
};

// Control Point의 Hull Shader를 의미한다
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	hout.PosL = p[i].PosL;

	return hout;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("quad")]
DomainOut DS(PatchTess patchTess,
	float2 uv : SV_DomainLocation,
	const OutputPatch<HullOut, 4> quad)
{
	DomainOut dout;

	// Bilinear interpolation.
	float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
	float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);
	float3 p = lerp(v1, v2, uv.y);

	// Displacement mapping
	p.y = 0.3f * (p.z * sin(p.x) + p.x * cos(p.z));

	dout.PosH = mul(float4(p, 1.0f), gWorldViewProj);

	return dout;
}
```