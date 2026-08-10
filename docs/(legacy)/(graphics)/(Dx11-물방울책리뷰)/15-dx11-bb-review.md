---
layout: default
title: "15. 삼각형 Tessellation"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/12)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-15-1.png"/>
</p>

* Shader만 봐도 충분하다

```cpp
// ...

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

// Generate and output per-patch data such as the quad tessellation factors.
// if a tessellation factor for a patch is set to zero, the patch is culled from the rest of the pipeline.
    // 이번엔 삼각형을 넘겨준다
PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

    // 6, 12, 3등분 해주세요
	pt.EdgeTess[0] = 6;
	pt.EdgeTess[1] = 12;
	pt.EdgeTess[2] = 3;

	pt.InsideTess = 1;
	
	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
};

// This Hull Shader part is commonly used for a coordinate basis change, 
// for example changing from a quad to a Bezier bi-cubic.
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 3> p,
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
[domain("tri")]
DomainOut DS(PatchTess patchTess,
	float3 baryCoords : SV_DomainLocation,
	const OutputPatch<HullOut, 3> tri)
{
	DomainOut dout;

	float3 p = baryCoords.x * tri[0].PosL + baryCoords.y * tri[1].PosL + baryCoords.z * tri[2].PosL;

	// Displacement mapping
	p.y = 0;// 0.3f*( p.z*sin(p.x) + p.x*cos(p.z) );

	dout.PosH = mul(float4(p, 1.0f), gWorldViewProj);

	return dout;
}
```