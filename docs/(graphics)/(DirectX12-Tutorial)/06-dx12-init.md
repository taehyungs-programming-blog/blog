---
layout: default
title: "06. Index Buffer"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/6)

## 네모를 그려보자

* 네모는 결국 삼각형 두 개를 붙여둔 것이라 할수 있으니 ...
    * 정점 6개를 만들면 되겠구나? 난 천재야 히히

```cpp
void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	vector<Vertex> vec(6);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	vec[3].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[4].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[4].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[5].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[5].color = Vec4(1.f, 0.f, 0.f, 1.f);

    // ...
```

* 이렇게 해도 되기는한데... 이거 너무 무식하지 않나?
    * 중복되는 정점을 없애고 싶다!
* **Index buffer**를 사용해보자

```cpp
void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	// 4개의 정점을 넘기고
	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);

	/*

		[0]-------------[1]
		 |               |
		 |               |
		 |               |
		 |               |
		[3]-------------[2]

	*/

    // 삼각형이 어떻게 세트인지 index buffer를 통해 넘긴다.
	vector<uint32> indexVec;
	{
		// 우/상단을 index로 넣음
		// 정점의 순서는 삼각형이 어떠한 순서로 구성되어있는지 알림.
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		// 좌/하단을 index로 넣음
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	mesh->Init(vec, indexVec);

	shader->Init(L"..\\Resources\\Shader\\default.hlsli");

	GEngine->GetCmdQueue()->WaitSync();
}
```

```cpp
void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}
```

```cpp
void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	_indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr);

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;
}
```

```cpp
void Mesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
    // Index Buffer로 쓸꺼에요!
    CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GEngine->GetCB()->PushData(0, &_transform, sizeof(_transform));
		GEngine->GetTableDescHeap()->SetCBV(handle, CBV_REGISTER::b0);
	}

	GEngine->GetTableDescHeap()->CommitTable();

	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}
```