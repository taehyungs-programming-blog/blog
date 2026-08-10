---
layout: default
title: "07. SkyBox"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/7)

```cpp
// ...

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_OUT VS(VertexTextureNormalTangent input)
{
	VS_OUT output;

	// Local -> World -> View -> Projection
	float4 viewPos = mul(float4(input.position.xyz, 0)/*카메라 회전만 반영*/, V);
	float4 clipSpacePos = mul(viewPos, P);
    // 사실상 아주 간단
    // 포지션을 카메라와 동일하게 해주고
	output.position = clipSpacePos.xyzw;
    // z값을 1(최외곽으로 밀어준다)
        // rasterizer에서 x/w, y/w, z/w할꺼기에 z에 w를 넣으면 1이됨
	output.position.z = output.position.w * 0.999999f;

	output.uv = input.uv;

	return output;
}


float4 PS(VS_OUT input) : SV_TARGET
{
	float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
	return color;
}

technique11 T0
{
	pass P0
	{
        // 컬링을 반대방향으로
		SetRasterizerState(FrontCounterClockwiseTrue);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};

```