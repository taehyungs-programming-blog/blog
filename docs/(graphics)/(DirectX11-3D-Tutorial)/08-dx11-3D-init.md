---
layout: default
title: "08. Instancing"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/8)

## Instancing

* 이런 처리는 어떻게 할까?
* 아래와 같이 네모가 여러개 있다고 가정하자, 이 경우 모든 네모는 렌더링 파이프라인을 통과할까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-1.png"/>
</p>

* 아무런 옵션을 넣지 않았다면 그렇다! 모든 네모는 렌더링 파이프라인을 거친다.
    * 이게 무슨 비효율인가!!
* 아래 그림의 Batch가 렌더링 파이프라인을 거친지를 의미한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-2.png"/>
</p>

* 그럼 옵션에서 Dynamic batching을 켜보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-4.png"/>
</p>

* 확실히 Batch가 줄었다.
* 이번 장에선 이 인스턴싱을 공부해볼 예정이다.

---

## Instancing을 적용해 보자

* 결론부터 말하면 `DrawIndexedInstanced`를 사용하게 된다.

```cpp
void InstancingDemo::Update()
{
	_camera->Update();
	RENDER->Update();

	{
		LightDesc lightDesc;
		lightDesc.ambient = Vec4(0.4f);
		lightDesc.diffuse = Vec4(1.f);
		lightDesc.specular = Vec4(0.1f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		RENDER->PushLightData(lightDesc);
	}

	_material->Update();

	_mesh->GetVertexBuffer()->PushData();
	_instanceBuffer->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_shader->DrawIndexedInstanced(0, 0, _mesh->GetIndexBuffer()->GetCount(), _objs.size());
}
```

* 궁금한건 각 Instance의 World Position을 어떻게 넘기느냐다.

```cpp
void InstancingDemo::Init()
{
	// ...

	// instance buffer라는 vertex buffer를 만들고
	_instanceBuffer = make_shared<VertexBuffer>();

	for (auto& obj : _objs)
	{
		Matrix world = obj->GetTransform()->GetWorldMatrix();
		_worlds.push_back(world);
	}

  // 기존 vertex buffer와 차이를 두기위해 slot을 1로 정의한다
	_instanceBuffer->Create(_worlds, /*slot*/1);
}
```

```cpp
void InstancingDemo::Update()
{
	// ...

	_mesh->GetVertexBuffer()->PushData();

  // instance buffer를 push한다
	_instanceBuffer->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_shader->DrawIndexedInstanced(0, 0, _mesh->GetIndexBuffer()->GetCount(), _objs.size());
}
```

```cpp
#include "00. Global.fx"
#include "00. Light.fx"

struct VS_IN
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	// INST일 경우 slot 1을 사용하게됨(effect 코드 참조)
	matrix world : INST;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION1;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

VS_OUT VS(VS_IN input)
{
	VS_OUT output;

	output.position = mul(input.position, input.world); // W
	output.worldPosition = output.position;
	output.position = mul(output.position, VP);
	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
	float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
	return color;
}

technique11 T0
{
	PASS_VP(P0, VS, PS)
};

```

---

## MeshInstancing

```cpp
void MeshInstancingDemo::Update()
{
	// ...

	// 생성한 object를 instancing manager를 통해 render한다
	INSTANCING->Render(_objs);
}
```

```cpp
void InstancingManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	for (shared_ptr<GameObject>& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue;

		const InstanceID instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject);
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		{
			const InstanceID instanceId = pair.first;

			for (int32 i = 0; i < vec.size(); i++)
			{
				const shared_ptr<GameObject>& gameObject = vec[i];
				InstancingData data;
				data.world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceId, data);
			}

			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			vec[0]->GetMeshRenderer()->RenderInstancing(buffer);
		}
	}
}
```

* 핵심은 Instancing Manager로 Instance를 관리한다.
* InstancingBuffer를 이용해 Instance Data를 밀어넣는다.

---