---
layout: default
title: "02. Global Shader"
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

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/2)

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

* ??? 헷갈리면 예시로 보자!

## 예시로 보자

```cpp
class GlobalTestDemo : public IExecute
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	shared_ptr<Shader> _shader;

	shared_ptr<GameObject> _obj;
	shared_ptr<GameObject> _camera;
};
```

```cpp

void GlobalTestDemo::Init()
{
	_shader = make_shared<Shader>(L"08. GlobalTest.fx");

	// 카메라를 붙이고
	_camera = make_shared<GameObject>();
	_camera->GetOrAddTransform();
	_camera->AddComponent(make_shared<Camera>());
	_camera->AddComponent(make_shared<CameraScript>());
	
	// GameObject를 하나 만들어 MeshRenderer를 붙이자
	_obj = make_shared<GameObject>();
	_obj->GetOrAddTransform();
	_obj->AddComponent(make_shared<MeshRenderer>());
	{
		_obj->GetMeshRenderer()->SetShader(_shader);
	}
	{
		RESOURCES->Init();
		auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
		_obj->GetMeshRenderer()->SetMesh(mesh);
	}
	{
		auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
		_obj->GetMeshRenderer()->SetTexture(texture);
	}

	RENDER->Init(_shader);
}

void GlobalTestDemo::Update()
{
	_camera->Update();
	RENDER->Update();

	_obj->Update();
}

void GlobalTestDemo::Render()
{
	
}
```

