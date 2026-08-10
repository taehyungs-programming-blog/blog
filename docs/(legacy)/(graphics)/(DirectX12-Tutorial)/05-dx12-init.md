---
layout: default
title: "05. Descriptor Table"
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

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/5)

## Constant Buffer의 한계점

* 64 DWORD (64 * 4 bytes = 256 bytes) 까지만 지원
* 메모리 용량의 문제로 **Descriptor Table**을 써야한다.

---

## (복습) 기존 Constant Buffer의 동작방식

```cpp
void ConstantBuffer::CreateBuffer()
{
    // ***************************
    // 1. GPU Register에 메모리 할당
    // ***************************
	uint32 bufferSize = _elementSize * _elementCount;
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

    // ***************************
    // 2. GPU RAM에 메모리 할당
    // ***************************
	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
```

```cpp
void ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	assert(_currentIndex < _elementSize);

    // ***************************
    // 3. CPU 메모리를 GPU로 복사
    // ***************************
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

    // ***************************
    // 4. GPU에게 할당된 메모리를 사용해 달라고 명령
    // ***************************
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);
	_currentIndex++;
}
```

---

## Descriptor Table 사용하기

* 주의할 점은 Descriptor Table이라고 해도 Constant buffer를 안쓰지 않는다
* Constant buffer에 CPU에서 온 데이터를 잠시 담아놨다 GPU가 가져가라고 명령하는 형태이다.

```cpp
void Engine::Init(const WindowInfo& info)
{
	// Engine의 Init 시점에 Transfrom 사이즈로 256개의 constant buffer를 생성
	_cb->Init(sizeof(Transform), 256);
	_tableDescHeap->Init(256);
}
```

```cpp
void ConstantBuffer::Init(uint32 size, uint32 count)
{
	// 상수 버퍼는 256 바이트 배수로 만들어야 한다
	// 0 256 512 768
	_elementSize = (size + 255) & ~255;
	_elementCount = count;

	CreateBuffer();
	CreateView();
}
```

```cpp
void ConstantBuffer::CreateBuffer()
{
    // GPU에 _elementSize * _elementCount 사이즈 만큼 메모리할당 후
    // _mappedBuffer를 통해 접근하겠다고 선언
	uint32 bufferSize = _elementSize * _elementCount;
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
}
```

```cpp
void ConstantBuffer::CreateView()
{
    // ** 여기가 어려움 !!주의!! **
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DEVICE->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

    // CPU Descriptor Heap의 시작주로를 준비 (일단은 받아들이자)
	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
    // Descriptor의 Size를 준비 (일단은 받아들이자)
	_handleIncrementSize = DEVICE->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);

/*
D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 index)
{
    // 위에 준비한 두 주소지, Size는 handle의 주소를 받아오기 위해 필요하다
        // 매번 이렇게 받아오는 이유는 GPU사양마다 주소가 다를수 있기때문!
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);
}
*/

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        // GPU에 할당된 constant buffer에 주소지를 받아오고
		cbvDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
		cbvDesc.SizeInBytes = _elementSize;   // CB size is required to be 256-byte aligned.

        // GPU에 할당된 constnat buffer주소를 CPU주소에 매핑한다
		DEVICE->CreateConstantBufferView(&cbvDesc, cbvHandle);
	}
}
```

* 이렇게 해서 어떻게 쓰냐?

```cpp
D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	assert(_currentIndex < _elementSize);

    // GPU에 메모리를 복사 후
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

    // 매핑된 CPU주소를 넘겨주면
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCpuHandle(_currentIndex);
	_currentIndex++;

	return cpuHandle;
}
```

```cpp
void Mesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)

	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
		GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);
```

```cpp
void TableDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	uint32 destRange = 1;
	uint32 srcRange = 1;
    // GPU에게 데이터를 읽어가라 명령을 내릴 수 있다.
	DEVICE->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
```

---

## Update Game 전체순서

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
class Mesh
{
    // ...

	void SetTransform(const Transform& t) { _transform = t; }
```

```cpp
void Mesh::Render()
{
	// ...
    
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
		GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);
	}
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
		GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b1);
	}
```