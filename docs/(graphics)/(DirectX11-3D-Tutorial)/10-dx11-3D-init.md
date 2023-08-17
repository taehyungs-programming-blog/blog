---
layout: default
title: "10. Compute Shader"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/8)

## RawBuffer

* 우선 Compute Shader에게 데이터를 넘겨줄 Buffer를 먼저 만들어 보자

```cpp
#pragma once


class RawBuffer
{
public:
	RawBuffer(void* inputData, uint32 inputByte, uint32 outputByte);
	~RawBuffer();

public:
	void CreateBuffer();
	void CopyToInput(void* data);
	void CopyFromOutput(void* data);

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return _srv; }
	ComPtr<ID3D11UnorderedAccessView> GetUAV() { return _uav; }

private:
	void CreateInput();
	void CreateSRV();
	void CreateOutput();
	void CreateUAV();
	void CreateResult();

private:
    // GPU에게 념겨줄 buffer
	ComPtr<ID3D11Buffer> _input;
	ComPtr<ID3D11ShaderResourceView> _srv;

    // GPU의 계산결과를 받을 buffer
	ComPtr<ID3D11Buffer> _output;
	ComPtr<ID3D11UnorderedAccessView> _uav;
	ComPtr<ID3D11Buffer> _result;

private:
	void* _inputData;
	uint32 _inputByte = 0;
	uint32 _outputByte = 0;
};
```

```cpp
void RawBuffer::CopyToInput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	DC->Map(_input.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, _inputByte);
	}
	DC->Unmap(_input.Get(), 0);
}

void RawBuffer::CopyFromOutput(void* data)
{
	// 출력 데이터 -> result에 복사
	DC->CopyResource(_result.Get(), _output.Get());

	D3D11_MAPPED_SUBRESOURCE subResource;
	DC->Map(_result.Get(), 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, _outputByte);
	}
	DC->Unmap(_result.Get(), 0);
}
```

---

### 이제 계산을 시켜보자

```cpp
// 우선 shader 부터

RWByteAddressBuffer Output; // UAV

struct ComputeInput
{
	uint3 groupID : SV_GroupID;
	uint3 groupThreadID : SV_GroupThreadID;
	uint3 dispatchThreadID : SV_DispatchThreadID;
	uint groupIndex : SV_GroupIndex;
};

// thread의 개수를 의미한다.
    // 10 * 8 * 3 = 240개의 thread를 쓰게 해주세요!
[numthreads(10, 8, 3)]
void CS(ComputeInput input)
{
	uint index = input.groupIndex;
	uint outAddress = index * 10/*ComputeInput는 4바이트짜리 변수가 10개 있는것과 동일(uint3 == int*3)*/ * 4;

	Output.Store3(outAddress + 0, input.groupID);
	Output.Store3(outAddress + 12, input.groupThreadID);
	Output.Store3(outAddress + 24, input.dispatchThreadID);
	Output.Store(outAddress + 36, input.groupIndex);
}

technique11 T0
{
	pass P0
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, CS()));
	}
};
```

```cpp
void RawBufferDemo::Init()
{
	_shader = make_shared<Shader>(L"24. RawBufferDemo.fx");

	// 하나의 쓰레드 그룹 내에서, 운영할 쓰레드 개수
	uint32 count = 10 * 8 * 3;

	shared_ptr<RawBuffer> rawBuffer = make_shared<RawBuffer>(nullptr,       // input data
                                                            0,              // input byte
                                                            sizeof(Output) * count);// output byte

	_shader->GetUAV("Output")->SetUnorderedAccessView(rawBuffer->GetUAV().Get());

	// x, y, z 쓰레드 그룹
	_shader->Dispatch(0, 0, 1, 1, 1);   // Compute Shader 동작해 주세요!


    // *** 결과 저장 ***
	vector<Output> outputs(count);
	rawBuffer->CopyFromOutput(outputs.data());

	FILE* file;
	::fopen_s(&file, "../RawBuffer.csv", "w");

	::fprintf
	(
		file,
		"GroupID(X),GroupID(Y),GroupID(Z),GroupThreadID(X),GroupThreadID(Y),GroupThreadID(Z),DispatchThreadID(X),DispatchThreadID(Y),DispatchThreadID(Z),GroupIndex\n"
	);

	for (uint32 i = 0; i < count; i++)
	{
		const Output& temp = outputs[i];

		::fprintf
		(
			file,
			"%d,%d,%d,	%d,%d,%d,	%d,%d,%d,	%d\n",
			temp.groupID[0], temp.groupID[1], temp.groupID[2],
			temp.groupThreadID[0], temp.groupThreadID[1], temp.groupThreadID[2],
			temp.dispatchThreadID[0], temp.dispatchThreadID[1], temp.dispatchThreadID[2],
			temp.groupIndex
		);
	}

	::fclose(file);
}
```

* 아직까진 어떻게 동작하는진 모르겠지만 어쨋든 결과가 나왔다.
* 이후에 동작을 어떻게 하는지 설명할 예정!

---

