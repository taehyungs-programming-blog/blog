---
layout: default
title: "04. Material, Normal Mapping"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/4)

## Matertial

* 특별한건 없다. 
* 기존에 Shader에서 하던 Material관련 처리를 하나의 클래스로 모아보자

```cpp
#pragma once
#include "ResourceBase.h"

class Material : public ResourceBase
{
	using Super = ResourceBase;
public:
	Material();
	virtual ~Material();

	shared_ptr<Shader> GetShader() { return _shader; }

	MaterialDesc& GetMaterialDesc() { return _desc; }
	shared_ptr<Texture> GetDiffuseMap() { return _diffuseMap; }
	shared_ptr<Texture> GetNormalMap() { return _normalMap; }
	shared_ptr<Texture> GetSpecularMap() { return _specularMap; }

	void SetShader(shared_ptr<Shader> shader);
	void SetDiffuseMap(shared_ptr<Texture> diffuseMap) { _diffuseMap = diffuseMap; }
	void SetNormalMap(shared_ptr<Texture> normalMap) { _normalMap = normalMap; }
	void SetSpecularMap(shared_ptr<Texture> specularMap) { _specularMap = specularMap; }

	void Update();

	shared_ptr<Material> Clone();

private:
	friend class MeshRenderer;

	MaterialDesc _desc;

	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _diffuseMap;
	shared_ptr<Texture> _normalMap;
	shared_ptr<Texture> _specularMap;

	ComPtr<ID3DX11EffectShaderResourceVariable> _diffuseEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _normalEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _specularEffectBuffer;
};

```

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/5)

## Normal Mapping

* **tangent space** - 정점마다 접하는 평면의 좌표계

```cpp
void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv)
{
	// [0,255] 범위에서 [0,1]로 변환
	float4 map = NormalMap.Sample(LinearSampler, uv);
	if (any(map.rgb) == false)
		return;

	float3 N = normalize(normal); // z
	float3 T = normalize(tangent); // x
	float3 B = normalize(cross(N, T)); // y
	float3x3 TBN = float3x3(T, B, N); // TS -> WS

	// [0,1] 범위에서 [-1,1] 범위로 변환
	float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
	float3 worldNormal = mul(tangentSpaceNormal, TBN);

	normal = worldNormal;
}
```
