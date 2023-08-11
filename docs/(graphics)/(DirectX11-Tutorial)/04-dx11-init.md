---
layout: default
title: "04. Lighting, Material"
parent: "(DirectX11 2D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Global Shader

* 아직은 Lighting이나 Material에 대한 부분은 아니다
* Shader에 공통된 부분을 Global.fx로 묶어보자

```cpp
// Global.fx

#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_

/////////////////
// ConstBuffer //
/////////////////

cbuffer GlobalBuffer
{
	matrix V;
	matrix P;
	matrix VP;
};

cbuffer TransformBuffer
{
	matrix W;
};

// ...
```

```cpp
// 실제 Global.fx를 사용
#include "Global.fx"

VertexOutput VS(VertexTextureNormal input)
{
	VertexOutput output;
	output.position = mul(input.position, W);
	output.position = mul(output.position, VP);
	output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)W);

	return output;
}

Texture2D Texture0;

float4 PS(VertexOutput input) : SV_TARGET
{
	return Texture0.Sample(LinearSampler, input.uv);
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};
```

* 이제 남은 부분은 constant buffer로 어떻게 넘기냐
* `VertexTextureNormal`을 Vertex Shader로 어떻게 넘기냐
* `VertexOutput`를 Shader에서 사용할 것을 어떻게 알리냐 이다.
* 이를 위해서 RenderManager를 만들었다.

```cpp
class Shader;

struct GlobalDesc
{
	Matrix V = Matrix::Identity;
	Matrix P = Matrix::Identity;
	Matrix VP = Matrix::Identity;
};

struct TransformDesc
{
	Matrix W = Matrix::Identity;
};


class RenderManager
{
	DECLARE_SINGLE(RenderManager);

public:
	void Init(shared_ptr<Shader> shader);
	void Update();

    // Global, Transform이 Constant Buffer이다
	void PushGlobalData(const Matrix& view, const Matrix& projection);
	void PushTransformData(const TransformDesc& desc);
	
private:
	shared_ptr<Shader> _shader;

	GlobalDesc _globalDesc;
	shared_ptr<ConstantBuffer<GlobalDesc>> _globalBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _globalEffectBuffer;

	TransformDesc _transformDesc;
	shared_ptr<ConstantBuffer<TransformDesc>> _transformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _transformEffectBuffer;
};
```

```cpp
// Constant로 넘기는 부분은 해결!

void RenderManager::PushGlobalData(const Matrix& view, const Matrix& projection)
{
	_globalDesc.V = view; 
	_globalDesc.P = projection;
	_globalDesc.VP = view * projection;
	_globalBuffer->CopyData(_globalDesc);
	_globalEffectBuffer->SetConstantBuffer(_globalBuffer->GetComPtr().Get());
}

void RenderManager::PushTransformData(const TransformDesc& desc)
{
	_transformDesc = desc;
	_transformBuffer->CopyData(_transformDesc);
	_transformEffectBuffer->SetConstantBuffer(_transformBuffer->GetComPtr().Get());
}
```

---

## Depth Stencil View

* 현재 두 물체가 겹치게 표현이 된다면, 과연 원근감 표현이 될까?
* 딱히 구현을 안해뒀으니 안되는게 정상이다.

```cpp
// DepthStencil관리는 Graphics에서 한다
class Graphics
{
	DECLARE_SINGLE(Graphics);
    // ...

	// DSV
	ComPtr<ID3D11Texture2D> _depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
```

---

## Ambient

* 환경광, 전반적으로 산란된 빛을 의미한다
* 두 가지를 알아야 한다.
    * 빛의 색상
    * 반사하는 물체의 반사 색상

```cpp
float4 LightAmbient;        // 빛 색상
float4 MaterialAmbient;     // 물체의 반사색상

// ...

Texture2D Texture0;

// Ambient (주변광/환경광)
// 수많은 반사를 거쳐서 광원이 불분명한 빛
// 일정한 밝기와 색으로 표현
float4 PS(VertexOutput input) : SV_TARGET
{
	float4 color = LightAmbient * MaterialAmbient;
	//return color;
	return Texture0.Sample(LinearSampler, input.uv) * color;
}
```

```cpp
void AmbientDemo::Update()
{
	_camera->Update();
	RENDER->Update();

	// 
	Vec4 lightAmbient{0.f, 1.f, 0.f, 0.f};
	_shader->GetVector("LightAmbient")->SetFloatVector((float*)&lightAmbient);

	{
		Vec4 materialAmbient(1.f);
		_shader->GetVector("MaterialAmbient")->SetFloatVector((float*)&materialAmbient);
		_obj->Update();
	}

	{
		Vec4 materialAmbient(1.f);
		_shader->GetVector("MaterialAmbient")->SetFloatVector((float*)&materialAmbient);
		_obj2->Update();
	}	
}
```

---

## Diffuse

* 분산광, 물체 표면에 반사되어 카메라로 들어오는 빛의 영향
* normal vector(표면에 수직)일 경우 가장 강하게 반사된다.
    * 어떻게 Diffuse를 구할지 힌트가 된다.

```cpp
#include "00. Global.fx"

float3 LightDir;            // 빛의 방향
float4 LightDiffuse;        // 빛의 색상
float4 MaterialDiffuse;     // 물체가 반응하는 색상
Texture2D DiffuseMap;       // 물체의 Texture이다

// ...

// Diffuse (분산광)
// 물체의 표면에서 분산되어 눈으로 바로 들어오는 빛
// 각도에 따라 밝기가 다르다 (Lambert 공식)
float4 PS(VertexOutput input) : SV_TARGET
{
	float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
	
    // 내적시 빛의 각도가 나온다
	float value = dot(-LightDir, normalize(input.normal));
    // 빛의 각도를 색에 반영
	color = color * value * LightDiffuse * MaterialDiffuse;

	return color;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};

```

---

## Specular

* 정반사광, 들어온 반대방향으로 반사되는 광의 영향을 의미한다.

```cpp
#include "00. Global.fx"

float3 LightDir;
float4 LightSpecular;
float4 MaterialSpecular;

Texture2D DiffuseMap;

// ...

// Specular (반사광)
// 한방향으로 완전히 반사되는 빛 (Phong)
float4 PS(MeshOutput input) : SV_TARGET
{
	//float3 R = reflect(LightDir, input.normal);   // 아래와 동일한 식이다.
	float3 R = LightDir - (2 * input.normal * dot(LightDir, input.normal));
    // input.normal을 곱하는 이유는 dot을 하면 Scalar값이 나온다 vector로 연산을 위해 곱함.
	R = normalize(R);

	float3 cameraPosition = -V._41_42_43;       // (TIPS) View에 행렬중 마지막 행이 위치이다.
	float3 E = normalize(cameraPosition - input.worldPosition);

	float value = saturate(dot(R, E)); // clamp(0~1)

    // pow를 하는이유는 그냥 specular를쓰면 너무 작은 구역만 영향을 받게된다
	float specular = pow(value, 10);

	float4 color = LightSpecular * MaterialSpecular * specular;

	return color;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};

```

---

## Emissive

* 외곽선 그리기
* normal vector와 eye vector가 수직에 가까울수록 1을 넣으면 되겠지?

```cpp
#include "00. Global.fx"

float4 MaterialEmissive;

// ...

// Emissive
// 외각선 구할 때 사용
// (림라이트)
float4 PS(MeshOutput input) : SV_TARGET
{
	float3 cameraPosition = -V._41_42_43;
	float3 E = normalize(cameraPosition - input.worldPosition);

	float value = saturate(dot(E, input.normal));
	float emissive = 1.0f - value;

	// min, max, x
	emissive = smoothstep(0.0f, 1.0f, emissive);
	emissive = pow(emissive, 2);

	float4 color = MaterialEmissive * emissive;

	return color;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};

```

---

## Light 통합

```cpp
float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
	float4 ambientColor = 0;
	float4 diffuseColor = 0;
	float4 specularColor = 0;
	float4 emissiveColor = 0;

	// Ambient
	{
		float4 color = GlobalLight.ambient * Material.ambient;
		ambientColor = DiffuseMap.Sample(LinearSampler, uv) * color;
	}

	// Diffuse
	{
		float4 color = DiffuseMap.Sample(LinearSampler, uv);
		float value = dot(-GlobalLight.direction, normalize(normal));
		diffuseColor = color * value * GlobalLight.diffuse * Material.diffuse;
	}

	// Specular
	{
		//float3 R = reflect(GlobalLight.direction, normal);
		float3 R = GlobalLight.direction - (2 * normal * dot(GlobalLight.direction, normal));
		R = normalize(R);

		float3 cameraPosition = CameraPosition();
		float3 E = normalize(cameraPosition - worldPosition);

		float value = saturate(dot(R, E)); // clamp(0~1)
		float specular = pow(value, 10);

		specularColor = GlobalLight.specular * Material.specular * specular;
	}

	// Emissive
	{
		float3 cameraPosition = CameraPosition();
		float3 E = normalize(cameraPosition - worldPosition);

		float value = saturate(dot(E, normal));
		float emissive = 1.0f - value;

		// min, max, x
		emissive = smoothstep(0.0f, 1.0f, emissive);
		emissive = pow(emissive, 2);

		emissiveColor = GlobalLight.emissive * Material.emissive * emissive;
	}

	return ambientColor + diffuseColor + specularColor + emissiveColor;
}
```

---

## Material

