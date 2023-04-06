---
layout: default
title: "26. Multiple Render Target"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/20)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-26-4.png"/>
</p>

* 잠깐 복습하자면 ...
    * Rendering의 시작과 끝만 표현하자면
    * Vertex와 Index정보를 Input Assembler에게 넘기면 그 데이터를 기반으로 Pixel, Vertex Shader를 거치고 마지막으로 Output merger단계에서 Render Target View에 이미지를 그려준다

* 여기서 집중할 부분은 Output merger인데

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect) 
{            
        // ...
	// Specify the buffers we are going to render to.
        // 스왑체인의 Renter Target View를 받아와서
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();

        // 한번 비워주고
	_cmdList->ClearRenderTargetView(backBufferView, Colors::Black, 0, nullptr); 

	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->GetDSVCpuHandle();

        // depth stencil view와 render target view를 output merger에 넣어준다
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView); 
	_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

// 간단히 설명하자면 shader에게 후면버퍼를 전달했다고 생각하면 된다.
```

## 이론 (Forward Shader 문제점 + Defered Shader 이론)

* 현재 사용중인 Forward Shader에서 랜더링 방식을 간단하게 보자면

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	//...

    // 그려질 후면 버퍼를 가져오고
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
    // D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }

    // 후면 버퍼를 비워주고
	_cmdList->ClearRenderTargetView(backBufferView, Colors::Black, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->GetDSVCpuHandle();
    // D3D12_CPU_DESCRIPTOR_HANDLE	GetDSVCpuHandle() { return _dsvHandle; }

    // OM(Output merger)단계에서 후면 버퍼에 최종결과물을 그려달라고 명령한다.
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);
	_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
```

```cpp
void CommandQueue::RenderEnd()
{
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),   // 백버퍼를 그려달라
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT); 

	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	WaitSync();

	_swapChain->SwapIndex();
}
```

* 중요한 단계를 정리하자면
    * 준비된 BackBuffer를 비워주고
    * 비워진 BackBuffer에 `OMSetRenderTargets`를 통하여 최종 랜더링 결과를 넣어달라고 한다.

* 그런데 이 과정에서 좀 아쉬운 점은 Rendering과정에서 만들어진 각종변수를 재활용할 수 없다는 점이다.

```cpp
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    // viewPos, viewNormal, viewTangent, viewBinormal 변수모두
    // 엄청난 연산을 통해 나온 변수들인데 재사용이 불가능하다
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}
```

```cpp
float4 PS_Main(VS_OUT input) : SV_Target
{
    // ...

    LightColor totalColor = (LightColor)0.f;

    for (int i = 0; i < g_lightCount; ++i)
    {
        // 특히 빛 계산의 경우 함수내부를 보면알지만 연산량이 많은데 이걸 매번 해주는게 아쉽다
            // 가령 Spot light인데 범위가 해당객체까지 못온다면? Light계산이 필요없어지는데 
            // Forward Shader에서는 구분이 불가능
         LightColor color = CalculateLightColor(i, viewNormal, input.viewPos);
         totalColor.diffuse += color.diffuse;
         totalColor.ambient += color.ambient;
         totalColor.specular += color.specular;
    }

        color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;

     return color;
}

// 아직은 정확히 몰라도 된다.
// 그냥 Texture를 이용해 Shader의 결과를 C++의 변수로 보관할수 있다 
// 정도로 이해하자.
```

---

## Deferred Rendering

* 이런 단점의 보완을 위해서 **Deferred Rendering**이 나타났다
* 아래가 Forward Rendering의 과정인데 **VS(Vertex Shader) -> GS(Geometry Shader) -> FS(Pixel Shader)**를 거치고 마지막 Render Target에 넘긴다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-26-1.png"/>
</p>

* **Deferred Rendering**의 경우 쉐이더에서 연산한결과를 Depth, Normal, Color 등 ... 여러 Render Target 에 두고 마지막에 합쳐 Final Render Target을 생성한다.(합치는 과정에서 Light연산을 하게 됨)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-26-2.png"/>
</p>

* 이게 뭔... 사실 이걸 글로만 보고 이해하면 천재이다. 이렇게만 이해해도 100점이다. 
* **지금까진 최종 결과를 Render Target에 넣었는데, 결과의 중간과정 까지 나눠서 여러 Render Target에 넣을 수 있게 하겠다.**

---

* Deferred Rendering은 다음 강좌에서 살펴보고 우선 여기서 할 내용은 Multiple Render Target을 만들고 그리는 방법을 설명하고자 한다.

## 코드로 보자

```cpp
// deferred.fx

// PS_Main에서 당장 출력될 정보를 주는게 아니라 내가 필요한 정보를 넘긴다가 핵심
struct PS_OUT
{
    // 쉐이더 연산의 결과를 받게할 변수들
    // 하나만 받는게 아니라 3개를 받음을 주목.!
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

	// (핵심) Texture 타겟에 데이터를 넣는다
    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(viewNormal.xyz, 0.f);
    output.color = color;

	// 빛 연산은 모두 삭제함

    return output;
}
```

* 여기까지하면 Pixel Shader의 Output에 각종 Rendering 결과가 들어가 있음
* C++에서 쉐이더 연산의 결과를 받아야하니 코드 수정이 필요하다

```cpp
// RenderTargetGroup.h

enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN,     // 화면을 그릴 (BACK_BUFFER, FRONT_BUFFER)
	G_BUFFER,       // Geometry Buffer : 연산의 결과를 저장할 (POSITION, NORMAL, COLOR)
	END,
};

enum
{
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};

struct RenderTarget
{
	shared_ptr<Texture> target;
	float clearColor[4];
};

class RenderTargetGroup
{
public:
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);

	void OMSetRenderTargets(uint32 count, uint32 offset);
	void OMSetRenderTargets();

	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetView();

	shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
	shared_ptr<Texture> GetDSTexture() { return _dsTexture; }

private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	uint32							_rtCount;
	shared_ptr<Texture>				_dsTexture;     
    // Depth-Stencil은 별도 관리(Depth-Stencil도 Texture이기에 여기서 관리)
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;

private:
	uint32							_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHeapBegin;
	D3D12_CPU_DESCRIPTOR_HANDLE		_dsvHeapBegin;
};
```

```cpp
// RenderTargetGroup.cpp

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType,  // 생성할 Type(SwapChain, G_BUFFER)
                                vector<RenderTarget>& rtVec,        // RenderTarget의 주소 Vector
                                shared_ptr<Texture> dsTexture)      // DepthStencil
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	_dsTexture = dsTexture;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = _rtCount;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

	_rtvHeapSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	_dsvHeapBegin = _dsTexture->GetDSV()->GetCPUDescriptorHandleForHeapStart();

	for (uint32 i = 0; i < _rtCount; i++)
	{
		uint32 destSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE destHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeapBegin, i * _rtvHeapSize);

		uint32 srcSize = 1;
		ComPtr<ID3D12DescriptorHeap> srcRtvHeapBegin = _rtVec[i].target->GetRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = srcRtvHeapBegin->GetCPUDescriptorHandleForHeapStart();

		DEVICE->CopyDescriptors(1, &destHandle, &destSize, 1, &srcHandle, &srcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}
```

```cpp
// 각 Render Target의 생성은 이렇게 한다.

void Engine::CreateRenderTargetGroups()
{
	// DepthStencil
	shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
	}

	// Deferred Group
	{
/*
    struct RenderTarget
    {
        shared_ptr<Texture> target;
        float clearColor[4];
    };
*/
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

/*
    shared_ptr<Texture> Resources::CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
        const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
    {
        shared_ptr<Texture> texture = make_shared<Texture>();
        texture->Create(format, width, height, heapProperty, heapFlags, resFlags, clearColor);
        Add(name, texture);

        return texture;
    }
*/

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}
}
```

```cpp
#pragma once
#include "Object.h"

class Texture : public Object
{
public:
	Texture();
	virtual ~Texture();

	// 이제 이미지 파일을 불러서 Texture를 만들때만사용됨
	virtual void Load(const wstring& path) override;

public:
    // Texture를 처음부터 만들어야 하는 경우
	void Create(DXGI_FORMAT format, uint32 width, uint32 height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // 이미 Resource는 있는데 Texture를 만들어야 하는경우(SwapChain)
	void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

    /*
    	// SwapChain Group
        {
            vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

            for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
            {
                wstring name = L"SwapChainTarget_" + std::to_wstring(i);

                ComPtr<ID3D12Resource> resource;
                _swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));

                // SwapChain의 경우 Buffer를 받아온 후 CreateTextureFromResource를 호출해서 Texture를 만든다
                rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
            }

    */

public:
	ComPtr<ID3D12Resource> GetTex2D() { return _tex2D; }
	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }

private:
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_tex2D;

    // srv(shader resource view) : 이미지 파일(.png, .jpg ...) 출력용
    // rtv(render target view) : Render Target 용(버퍼용)
    // dsv(depth stencil view) : Depth Stencil 용
    // 이제 texture(render target)이 단순 그림출력용이 아니라 여려 용도로 쓰이기에 할당을 어려개를 함.
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	ComPtr<ID3D12DescriptorHeap>	_dsvHeap;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHeapBegin = {};
};
```

```cpp
void Texture::Create(DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
	desc.Flags = resFlags;

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		// Depth Sencil인지
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
		optimizedClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
	}
	else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		// Render Target인지
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
		float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
		optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
	}

	// Create Texture2D
	HRESULT hr = DEVICE->CreateCommittedResource(
		&heapProperty,
		heapFlags,
		&desc,
		resourceStates,
		&optimizedClearValue,
		IID_PPV_ARGS(&_tex2D));

	assert(SUCCEEDED(hr));

    // ComPtr<ID3D12Resource>			_tex2D;
	CreateFromResource(_tex2D);
}

void Texture::CreateFromResource(ComPtr<ID3D12Resource> tex2D)
{
	_tex2D = tex2D;

	D3D12_RESOURCE_DESC desc = tex2D->GetDesc();

	// 주요 조합
	// - DSV 단독 (조합X)	-> Depth Stencil
	// - SRV				-> 이미지 파일 출력용도
	// - RTV + SRV          -> Render Target + 이미지(쉐이더의 결과물 출력용)
	if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		// DSV
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;
		DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_dsvHeap));

		D3D12_CPU_DESCRIPTOR_HANDLE hDSVHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
		DEVICE->CreateDepthStencilView(_tex2D.Get(), nullptr, hDSVHandle);
	}
	else
	{
		if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			// RTV
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = 1;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapDesc.NodeMask = 0;
			DEVICE->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_rtvHeap));

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
			DEVICE->CreateRenderTargetView(_tex2D.Get(), nullptr, rtvHeapBegin);
		}

		// SRV
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DEVICE->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap));

		_srvHeapBegin = _srvHeap->GetCPUDescriptorHandleForHeapStart();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = _image.GetMetadata().format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srvHeapBegin);
	}
}
```

---

### Render Taget을 만들어 보자

```cpp
void Engine::Init(const WindowInfo& info)
{
	// ...

	CreateRenderTargetGroups();
```

```cpp
void Engine::CreateRenderTargetGroups()
{
	// DepthStencil
	shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil",
		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
	}

	// Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget",
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget",
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}
}
```

```cpp
void Shader::Init(const wstring& path, ShaderInfo info)
{
	// ...

	switch (info.shaderType)
	{
	case SHADER_TYPE::DEFERRED:
		_pipelineDesc.NumRenderTargets = RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // POSITION
		_pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT; // NORMAL
		_pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // COLOR
		break;
	case SHADER_TYPE::FORWARD:
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

    // ...
```

```cpp
void Scene::Render()
{
	PushLightData();

	// SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	// Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	for (auto& gameObject : _gameObjects)
	{
		if (gameObject->GetCamera() == nullptr)
			continue;

		// 내가 그려야할 오브젝트 정리
		gameObject->GetCamera()->SortGameObject();

		// Deferred OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();
		gameObject->GetCamera()->Render_Deferred();
/*
    void Camera::Render_Deferred()
    {
        S_MatView = _matView;
        S_MatProjection = _matProjection;

        for (auto& gameObject : _vecDeferred)
        {
            gameObject->GetMeshRenderer()->Render();
        }
    }
*/

		// Light OMSet

		// Swapchain OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);
		gameObject->GetCamera()->Render_Forward();
/*
    void Camera::Render_Forward()
    {
        S_MatView = _matView;
        S_MatProjection = _matProjection;

        for (auto& gameObject : _vecForward)
        {
            gameObject->GetMeshRenderer()->Render();
        }
    }
*/
	}
}
```

---

### 흠... 잘 이해가... 그려지는 부분 부터 다시 보자면

😺 아래 그림처럼 총 세개의 UI(쉐이더의 중간 결과물)을 그려보려한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-26-3.png"/>
</p>

```cpp
// 텍스쳐의 생성

void Engine::CreateRenderTargetGroups() 
{ 
	// DepthStencil 
	shared_ptr<Texture> dsTexture = GET_SINGLE(Resources)->CreateTexture(L"DepthStencil", 
		DXGI_FORMAT_D32_FLOAT, _window.width, _window.height, 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);



	// SwapChain Group 
	{ 
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT); 
		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) 
		{ 
			wstring name = L"SwapChainTarget_" + std::to_wstring(i); 
			ComPtr<ID3D12Resource> resource; 
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource)); 
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource); 
		} 
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>(); 
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture); 
	}




	// Deferred Group 
	{ 
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT); 
		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(L"PositionTarget", 
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, 
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET); 
		rtVec[1].target = GET_SINGLE(Resources)->CreateTexture(L"NormalTarget", 
			DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width, _window.height, 
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET); 
		rtVec[2].target = GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget", 
			DXGI_FORMAT_R8G8B8A8_UNORM, _window.width, _window.height, 
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET); 
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>(); 
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture); 
	} 
}
```

```cpp
shared_ptr<Scene> SceneManager::LoadTestScene() 
{ 

        // ...
#pragma region UI_Test 
	for (int32 i = 0; i < 3; i++) 
	{ 
		shared_ptr<GameObject> sphere = make_shared<GameObject>(); 
		sphere->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI 
		sphere->AddComponent(make_shared<Transform>()); 
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f)); 
		sphere->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 160), 250.f, 500.f)); 
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(); 
		{ 
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh(); 
			meshRenderer->SetMesh(mesh); 
		} 
		{ 
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Forward"); 

                        // 만들어둔 Texture를 넣는다
			shared_ptr<Texture> texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);

			shared_ptr<Material> material = make_shared<Material>(); 
			material->SetShader(shader); 
			material->SetTexture(0, texture); 
			meshRenderer->SetMaterial(material); 
		} 
		sphere->AddComponent(meshRenderer); 
		scene->AddGameObject(sphere); 
	} 
#pragma endregion
```

```cpp
void Scene::Render() 
{ 
	PushLightData(); 
	// SwapChain Group 초기화 
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex(); 
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex); 
	// Deferred Group 초기화 
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView(); 
	for (auto& gameObject : _gameObjects) 
	{ 
		if (gameObject->GetCamera() == nullptr) 
			continue; 
		gameObject->GetCamera()->SortGameObject(); 
		// Deferred OMSet 
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets(); 
		gameObject->GetCamera()->Render_Deferred(); 
		// Light OMSet 
		// Swapchain OMSet 
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex); 
		gameObject->GetCamera()->Render_Forward(); 
	} 
}
```