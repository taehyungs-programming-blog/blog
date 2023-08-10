---
layout: default
title: "06. Animation"
parent: "(DirectX11 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 애니메이션 파일을 파싱하는 부분은 생략함. 앞 부분이랑 겹치는듯

```cpp
// 참고로 말하자면 이제 Vertex를 아래와 같이 받는데
// blendIndices, blendWeights가 추가되었다.
    // blendIndices - 현제 Vertex에 영향을 주는 Bone Index
    // blendWeights - 얼마나 영향을 주는지 영향도를 의미
struct VertexTextureNormalTangentBlend
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 blendIndices : BLEND_INDICES;
	float4 blendWeights : BLEND_WEIGHTS;
};
```

---

## 에니메이션 적용

```cpp
void AnimationDemo::CreateKachujin()
{
	shared_ptr<class Model> m1 = make_shared<Model>();
	m1->ReadModel(L"Kachujin/Kachujin");
	m1->ReadMaterial(L"Kachujin/Kachujin");
	m1->ReadAnimation(L"Kachujin/Idle");
	m1->ReadAnimation(L"Kachujin/Run");
	m1->ReadAnimation(L"Kachujin/Slash");

	_obj = make_shared<GameObject>();
	_obj->GetOrAddTransform()->SetPosition(Vec3(0, 0, 1));
	_obj->GetOrAddTransform()->SetScale(Vec3(0.01f));

	_obj->AddComponent(make_shared<ModelAnimator>(_shader));
	{
		_obj->GetModelAnimator()->SetModel(m1);
		//_obj->GetModelAnimator()->SetPass(1);
	}
}
```
