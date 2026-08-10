---
layout: default
title: "28. Compute Shader"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/22)

## 이론

* **GPGPU** : GPU에게 CPU 처럼 일반적 연산을 시켜보자(주로 병렬적 연산)
    * Ex) 소수찾기는 모든 숫자마다 연관성이 없기에 병렬적으로 연산가능 등등..
    * Ex2) 가상화폐 암호연산

```cpp
// compute.fx

#ifndef _COMPUTE_FX_
#define _COMPUTE_FX_

#include "params.fx"

// u0 레지스터 : Compute Shader 전용 레지스터
// RWTexture2D : Read, Write가 되는 Texture
    // 읽고 쓸수 있는 1차원 배열임을 기억하자.
RWTexture2D<float4> g_rwtex_0 : register(u0);

/* GPU에서 쓰레드를 실행의 단위라 생각하면 안됨. 일감의 단위라 생각해야한다. */
/* Thread가 1024개 만들어 졌다고 CPU에서 처럼 1024개의 별도의 Thread가 아니라 */
/* 독립적인 일처리할 분량이 1024개 만들어 졌다는 말. */

// 쓰레드 그룹당 쓰레드 개수
// CS 5.0에서 최대 1024개 까지 선언가능 -> max : 1024 (CS_5.0)
// - 하나의 쓰레드 그룹은 하나의 다중처리기에서 실행


// 조금 설명이 필요한데
// Thread Group을 Dispatch하며 material->Dispatch(1, 1024, 1);
// y축으로 1024개 선언예정
// 각 thread group내 thread box는 x축으로 1024개 선언 예정
// g_rwtex_0을 1024 x 1024로 만들예정

[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    if (threadIndex.y % 2 == 0)
        // 짝수는 빨강
        g_rwtex_0[threadIndex.xy] = float4(1.f, 0.f, 0.f, 1.f);
    else
        // 홀수는 녹색
        g_rwtex_0[threadIndex.xy] = float4(0.f, 1.f, 0.f, 1.f);
}

#endif
```

* `[numthreads(1024, 1, 1)]` 코드 이해를 위해선 GPGPU Thread동작방식에 대한 이해가 필요한데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-28-1.png"/>
</p>

* 위 그림과 같이 GPU내에서는 Thread가 2차원 배열로 관리되고 그 2차원 배열은 **Block(Thread Group)**이라는 단위로 관리된다.

* `[numthreads(1024, 1, 1)]` : Block(Thread Group)당 쓰레드의 개수를 정의한다. 총 Thread개수를 `1024 * 1 * 1` 개로 해달라

* [numthreads MSDN 🌍](https://docs.microsoft.com/ko-kr/windows/win32/direct3dhlsl/sm5-attributes-numthreads)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-28-2.png"/>
</p>

* 위의 예를 들면
    * `Dispatch(5, 3, 2)`는 Thread Group을 x = 5, y = 3, z = 2
    * `numthread(10, 8, 3`은 Thread를 x = 10, y = 8, z = 3개로 만들어 달라

* 따라서 `[numthreads(1024, 1, 1)]`는 2차원 배열이 된다.

---

## 구현

```cpp
// CommandQueue.h

// Compute Shader용 Queue를 만든다

/*
class GraphicsCommandQueue 
-> 는 Graphics용 CommandQueue
-> 참고로 CommandQueue는 명령을 모아서 보내는 용도.
*/

class ComputeCommandQueue
{
public:
	~ComputeCommandQueue();

	void Init(ComPtr<ID3D12Device> device);
	void WaitSync();
	void FlushComputeCommandQueue();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetComputeCmdList() { return _cmdList; }

private:
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;

	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;
};
```

```cpp
// Compute용 RootSignature도 생성

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return _computeRootSignature; }

private:
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	D3D12_STATIC_SAMPLER_DESC	_samplerDesc; 
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;	
	ComPtr<ID3D12RootSignature>	_computeRootSignature;
};

```

```cpp
// DescriptorHeap도 Compute용으로 생성

class ComputeDescriptorHeap
{
public:
	void Init();

	void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void SetSRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

	void CommitTable();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UAV_REGISTER reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint8 reg);

private:

	ComPtr<ID3D12DescriptorHeap> _descHeap;
	uint64						_handleSize = 0;
};
```

* Compute Shader 추가

```cpp
void Resources::CreateDefaultShader()
{
    // ...

	// Compute Shader
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateComputeShader(L"..\\Resources\\Shader\\compute.fx", "CS_Main", "cs_5_0");
        /*
            void Shader::CreateComputeShader(const wstring& path, const string& name, const string& version)
            {
                _info.shaderType = SHADER_TYPE::COMPUTE;

                CreateShader(path, name, version, _csBlob, _computePipelineDesc.CS);
                _computePipelineDesc.pRootSignature = COMPUTE_ROOT_SIGNATURE.Get();

                HRESULT hr = DEVICE->CreateComputePipelineState(&_computePipelineDesc, IID_PPV_ARGS(&_pipelineState));
                assert(SUCCEEDED(hr));
            }
        */
		Add<Shader>(L"ComputeShader", shader);
	}
```

* Compute Shader를 사용하는 Material 생성

```cpp
void Resources::CreateDefaultMaterial()
{
    // ...

	// Compute Shader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		Add<Material>(L"ComputeShader", material);
	}
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV(Unordered Access View) 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

    // ...

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
                // UI도 UAV용 Texture를 받게한다
				texture = GET_SINGLE(Resources)->Get<Texture>(L"UAVTexture");
```