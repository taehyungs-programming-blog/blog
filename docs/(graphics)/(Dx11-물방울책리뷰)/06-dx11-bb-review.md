---
layout: default
title: "6. 파도 시뮬레이션"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/10)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-6-1.png"/>
</p>

* 특이 한 부분은 Vertex를 CPU에서 수정하기에 아래와 같이 Flag를 수정해야하며

```cpp
void WavesDemo::BuildWavesGeometryBuffers()
{
	D3D11_BUFFER_DESC vbd;
    // CPU에서 Vertex수정이 필요한 경우 이렇게 설정 1
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * _waves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // CPU에서 Vertex수정이 필요한 경우 이렇게 설정 2
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(_device->CreateBuffer(&vbd, 0, _wavesVB.GetAddressOf()));

    // ...
```

* Vertex를 수정하는 부분은 아래와 같고 사실 큰 의민없다.
    * 위에 메모리 엑세스 부분이 더 중요함.

```cpp
void WavesDemo::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = _radius * sinf(_phi) * cosf(_theta);
	float z = _radius * sinf(_phi) * sinf(_theta);
	float y = _radius * cosf(_phi);

	// Build the view matrix.
	XMVECTOR pos = ::XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = ::XMVectorZero();
	XMVECTOR up = ::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = ::XMMatrixLookAtLH(pos, target, up);
	::XMStoreFloat4x4(&_view, V);

	//
	// Every quarter second, generate a random wave.
	//
	static float t_base = 0.0f;
	if ((_timer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		DWORD i = 5 + rand() % 190;
		DWORD j = 5 + rand() % 190;

		float r = MathHelper::RandF(1.0f, 2.0f);

		_waves.Disturb(i, j, r);
	}

	_waves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(_deviceContext->Map(_wavesVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	for (UINT i = 0; i < _waves.VertexCount(); ++i)
	{
		v[i].pos = _waves[i];
		v[i].color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	_deviceContext->Unmap(_wavesVB.Get(), 0);
}
```