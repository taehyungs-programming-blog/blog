---
layout: default
title: "03-02. 삼각형을 그려보자 (Rootsignature)"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## RootSignature 클래스

* RootSignature는 랜더링 파이프라인에 변수를 넣을때 사용된다
    * 아직 RootSignature에 대해 완벽히 이해할순 없고 대략 RootSignature라는 애가 있고 현재는 기본적 생성만 한다고 받아들이자.

```cpp
class RootSignature
{
public:
    void Init(ComPtr<ID3D12Device> device);
    ComPtr<ID3D12RootSignature> GetSignature() { return _signature; }
private:
    ComPtr<ID3D12RootSignature> _signature;
};
```

```cpp
void RootSignature::Init(ComPtr<ID3D12Device> device)
{
    D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
    sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립 단계

    ComPtr<ID3DBlob> blobSignature;
    ComPtr<ID3DBlob> blobError;
    ::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);

    device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}
```

```cpp
void Shader::Init(const wstring& path)
{
	CreateVertexShader(path, "VS_Main", "vs_5_0");
	CreatePixelShader(path, "PS_Main", "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	_pipelineDesc.InputLayout = { desc, _countof(desc) };

    // Shader 생성시 RootSignature를 넣는다 기억하자
	_pipelineDesc.pRootSignature = ROOT_SIGNATURE.Get();
```
