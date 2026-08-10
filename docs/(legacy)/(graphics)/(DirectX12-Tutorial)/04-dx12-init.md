---
layout: default
title: "04. Constant Buffer"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/4)

## 뭘 하고자 하나?

* 기존에 그린 삼각형을 이동, 색상을 해보고자 한다.
* 이동, 색상변경을 위해서 쉐이더 단계에서 사용할 상수를 넘겨 내가 원하는 이동, 색상변경을 하고자하는데
* 쉐이더는 GPU에서 수행하는 명령이기에 상수(변수)자체를 넘기는 일이 그리 쉽지는 않다

```cpp
// ...

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1.f);
    output.pos += ???;		// 여기를 변경할 수 있다면??
    output.color = input.color;
    output.color += ???;

    return output;
}

// ...
```

---

## ConstantBuffer

* 우선 지난 장에서 쉐이딩 파이프라인을 RootSignature에서 관리하고 RootSignature를 통해서 쉐이딩 파이프라인 중 특정단계에서 상수를 넣을 수 있다.
* 뭐 대략 사용은 아래와 같이한다.

```cpp
// cbuffer(constant buffer)를 사용할꺼고 register b0를 사용하겠습니다
// 이름은 offset0로 해주세요
cbuffer TEST_B0 : register(b0)
{
    float4 offset0;
};

cbuffer TEST_B1 : register(b1)
{
    float4 offset1;
};

// ...

// offset0, 1은 이렇게 쓰겠다.
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos, 1.f);
    output.pos += offset0;
    output.color = input.color;
    output.color += offset1;

    return output;
}
```

```cpp
// 대략 해석해 보자면 ...

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
    // constantbuffer를 2개 쓸 예정입니다.
	CD3DX12_ROOT_PARAMETER param[2];
	param[0].InitAsConstantBufferView(0); // 0번 -> b0 -> CBV 
	param[1].InitAsConstantBufferView(1); // 1번 -> b1 -> CBV

	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(2, param);
	// constantbuffer는 IA(입력 조립기 단계)에 들어갔으면 합니다.
    sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; 

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
```

* 참고로 위 방식은 RootSignature에 constant(상수)를 넘기는 방식이고 Buffer View(일종의 포인터)로 넘길수 있고 Descriptor tables 테이블(배열) 형태로 넘길수도 있다. 
    * [참고 🌍](https://docs.microsoft.com/en-us/windows/win32/direct3d12/example-root-signatures)

```cpp
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect) 
{ 
	// ... 
	// RootSignature가 사용되게 해달라고 전달 
	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
```

* 여기까지하면 RootSignature를 통해 변수를 넘기고 쉐이더에서 그 변수를 사용하는데 까진 됐는데
* 가장중요한 변수를 어떤 값으로 할당하는지를 어떻게 할지가 나오지 않았다.
* 대략 이렇게 진행된다.

1. 데이터를 담을 GPU 메모리를 할당한다.
2. CPU의 데이터를 GPU 메모리에 데이터를 담는다
3. GPU 레지스터가 GPU 메모리에서 읽어온다
4. 읽어온 데이터를 쉐이딩 단계에서 사용한다.

---

### 데이터를 담을 GPU 메모리를 할당해 보자

```cpp
// ConstantBuffer라는 클래스에서 담당
    // Q. GPU메모리를 그냥 size만큼 할당하면 되지 왜 count를 둬서 더 많이 할당하나?
        // 아래 답변이 나옴

void ConstantBuffer::Init(uint32 size, uint32 count) 
{ 
	// 상수 버퍼는 256 바이트 배수로 만들어야 한다 
	// 0 256 512 768 
	_elementSize = (size + 255) & ~255; 
	// & ~255 하위 8bits 정보를 모두 0으로 만들기에 256의 배수가 된다.
    // 255 = 1111 1111 / ~255 = 0000 0000
	_elementCount = count; 
	CreateBuffer(); 
}
```

```cpp
void ConstantBuffer::CreateBuffer() 
{ 
	uint32 bufferSize = _elementSize * _elementCount; 
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);


    // GPU 메모리를 할당해 주세요 
	DEVICE->CreateCommittedResource( 
		&heapProperty, 
		D3D12_HEAP_FLAG_NONE, 
		&desc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(&_cbvBuffer)); 

    // 할당받은 GPU 메모리를 Map을 통해 열어둔다.
	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer)); 
        	// 여기까지하면 _mappedBuffer를 통해 GPU 메모리에 데이터를 넣을 수 있다
	// We do not need to unmap until we are done with the resource.  However, we must not write to 
	// the resource while it is in use by the GPU (so we must use synchronization techniques). 

	// 여기서 질문? CPU가 데이터를 GPU에 쓸때
	// GPU가 데이터를 읽어가 버리면???
	// 절대 그럴일이없음 -> fence로 현재 GPU작업이 다 끝나면 다음작업을 CPU에서 하도록 강제하게 만들어둠.
}
```

### GPU 메모리에 데이터를 담는다

### GPU 레지스터가 GPU 메모리에서 읽어와 쉐이딩 단계에서 사용한다.

```cpp
void Mesh::Render() 
{ 
	// ... 

	// 1) Buffer에다가 데이터 세팅 
	// 2) Buffer의 주소를 register에다가 전송 
	GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform)); 
	GEngine->GetCB()->PushData(1, &_transform, sizeof(_transform)); 

        // ...
}
```

```cpp
void ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size) 
{ 
	assert(_currentIndex < _elementSize);

	// 2. GPU 메모리에 데이터를 담는다
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size); 
            // 메모리 복사를 하고
        
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);

/*
    D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
    {
        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress();
        objCBAddress += index * _elementSize;
        return objCBAddress;
    }
*/

	// 3. GPU 레지스터가 GPU 메모리에서 읽어와 쉐이딩 단계에서 사용한다. 
	// 커멘드 리스트에 등록해 GPU Register가 읽어가도록 명령
            // 커멘드 리스트에게 메모리가 써졌다고 알린다고 생각하자
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address); 
	_currentIndex++; 
}
```

---

## (TIPS) GPU메모리를 그냥 size만큼 할당하면 되지 왜 count를 둬서 더 많이 할당하나?

* 구현중 이해가 되지 않는 부분이 있을 것이다.
* 메모리 공간할당의 부분인데 사용될 메모리보다 GPU공간을 더 많이 할당한다. 
* 이런 할당을 하는 이유는 CPU->GPU 메모리 복사는 즉시 일어나고 레지스터가 GPU메모리를 읽어가는 부분은 큐에 의해 동작하기에 즉시 동작하지 않는다
* 꼭 레지스터에 데이터를 쓸때의 문제는 아니고, 커멘드리스트(cmdlist)를 쓸때 공통적으로 발생하는 주의사항이다.

```cpp
void ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size) 
{ 
	assert(_currentIndex < _elementSize);
	// 메모리의 복사는 즉시 일어나는 부분이고 
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size); 
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);


	// 레지스터에 주소값을 전달하는건 queue에 의해 나중에 일어나는 부분
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address); 
	_currentIndex++; 
}
```

* 그럼 복사는 일어났는데 queue에 명령이 쌓여 내가 원하는 시점의 데이터를 못 가져오는 경우가 시차때문에 발생할수 있지 않을까?
* 버퍼를 여러개만들어서 이 문제를 해결해야한다.

```cpp
void ConstantBuffer::CreateBuffer()  
{  
	uint32 bufferSize = _elementSize * _elementCount; 
	// 버퍼사이즈를 사용하는 레지스터 count만큼 잡음 
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);  
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
```

---

## 결과출력

```cpp
void Game::Update()
{
	GEngine->RenderBegin();

	shader->Update();

	{
		Transform t;
		t.offset = Vec4(0.75f, 0.f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}

	{
		Transform t;
		t.offset = Vec4(0.f, 0.75f, 0.f, 0.f);
		mesh->SetTransform(t);

		mesh->Render();
	}

	GEngine->RenderEnd();
}
```

```cpp
void Mesh::Render()
{
    CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
    // TODO
    // 1) Buffer에다가 데이터 세팅
    // 2) Buffer의 주소를 register에다가 전송
    GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
    GEngine->GetCB()->PushData(1, &_transform, sizeof(_transform));
    CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-4-1.png"/>
</p>