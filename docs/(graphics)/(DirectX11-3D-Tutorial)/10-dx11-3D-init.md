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

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/9)

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

## System Value - 로그에 찍힌 값이 뭔지 알아보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-10-1.png"/>
</p>

* 위 그림에 나온 대로 

```cpp
// thread group을 x, y, z (1, 1, 1)로 선언합니다.
_shader->Dispatch(0, 0, 1, 1, 1);
```

```cpp
// 각 thread group에는 x, y, z (10, 8, 3) = 240개의 thread가 존재합니다.
[numthreads(10, 8, 3)]
```

* 가령 아래와 같이 설정한다면

```cpp
// 2 * 2 * 2 = 8
_shader->Dispatch(0, 0, 2, 2, 2);

// 240 
[numthreads(10, 8, 3)]

// Total = 8 * 240 = 1920개의 thread
```

* 그럼 각 System Value의 의미는 뭘까?
* `SV_GroupID`, `SV_GroupThreadID`, `SV_DispatchThreadID`, `SV_GroupIndex`
    * `SV_GroupID` - 말 그대로 Group id
    * `SV_GroupThreadID` - 말 그대로 Group Thread id
    * `SV_DispatchThreadID` - group 까지 포함해 unique한 id
    * `SV_GroupIndex` - 현재 group에서 해당 thread의 index

---

## input에 뭔가 데이터를 넣어보자

```cpp
void GroupDemo::Init()
{
	_shader = make_shared<Shader>(L"25. GroupDemo.fx");

	// 하나의 쓰레드 그룹 내에서, 운영할 쓰레드 개수
	uint32 threadCount = 10 * 8 * 3;
	uint32 groupCount = 2 * 1 * 1;
	uint32 count = groupCount * threadCount;

	vector<Input> inputs(count);
	for (int32 i = 0; i < count; i++)
		inputs[i].value = rand() % 10000;

	shared_ptr<RawBuffer> rawBuffer = make_shared<RawBuffer>(inputs.data(), sizeof(Input) * count, sizeof(Output) * count);
	
	_shader->GetSRV("Input")->SetResource(rawBuffer->GetSRV().Get());
	_shader->GetUAV("Output")->SetUnorderedAccessView(rawBuffer->GetUAV().Get());

	// x, y, z 쓰레드 그룹
	_shader->Dispatch(0, 0, 2, 1, 1);

	vector<Output> outputs(count);
	rawBuffer->CopyFromOutput(outputs.data());
```

```cpp
ByteAddressBuffer Input; // SRV
RWByteAddressBuffer Output; // UAV

struct ComputeInput
{
	uint3 groupID : SV_GroupID;
	uint3 groupThreadID : SV_GroupThreadID;
	uint3 dispatchThreadID : SV_DispatchThreadID;
	uint groupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(ComputeInput input)
{
	uint index = input.groupID.x * (10 * 8 * 3) + input.groupIndex;
	uint outAddress = index * 11 * 4;

	uint inAddress = index * 4;
	float value = (float)Input.Load(inAddress);

	Output.Store3(outAddress + 0, input.groupID);
	Output.Store3(outAddress + 12, input.groupThreadID);
	Output.Store3(outAddress + 24, input.dispatchThreadID);
	Output.Store(outAddress + 36, input.groupIndex);
	Output.Store(outAddress + 40, (uint)value);
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

---

## Texture Buffer

* Buffer를 Texture로 보내려한다

```cpp
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureBufferDemo::MakeComputeShaderTexture()
{
	auto shader = make_shared<Shader>(L"26. TextureBufferDemo.fx");
	
	auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
	shared_ptr<TextureBuffer> textureBuffer = make_shared<TextureBuffer>(texture->GetTexture2D());

	shader->GetSRV("Input")->SetResource(textureBuffer->GetSRV().Get());
	shader->GetUAV("Output")->SetUnorderedAccessView(textureBuffer->GetUAV().Get());

	uint32 width = textureBuffer->GetWidth();
	uint32 height = textureBuffer->GetHeight();
	uint32 arraySize = textureBuffer->GetArraySize();

	uint32 x = max(1, (width + 31) / 32);
	uint32 y = max(1, (height + 31) / 32);
	shader->Dispatch(0, 0, x, y, arraySize);

	return textureBuffer->GetOutputSRV();
}
```

```cpp
Texture2DArray<float4> Input;
RWTexture2DArray<float4> Output;

[numthreads(32, 32, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
	float4 color = Input.Load(int4(id, 0));

	//Output[id] = color;
	Output[id] = 1.0f - color;
	//Output[id] = (color.r + color.g + color.b) / 3.0f;
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

---

## Structure Buffer

* 우리가 원하는 구조로 Buffer를 만들어보자
* 복잡하진 않아 설명은 생략, 자세한건 코드를 보자.

```cpp

struct InputDesc
{
	matrix input;
};

struct OutputDesc
{
	matrix result;
};

StructuredBuffer<InputDesc> Input;
RWStructuredBuffer<OutputDesc> Output;

[numthreads(500, 1, 1)]
void CS(uint id : SV_GroupIndex)
{
	matrix result = Input[id].input * 2;

	Output[id].result = result;
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
