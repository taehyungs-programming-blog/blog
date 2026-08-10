---
layout: default
title: "29. Particle System"
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

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/23)

## 이론

### 1. Particle은 하나하나 만들어야 할까?

* 아래그림과 같이 생성된 Sphere주의에 Particle을 넣으려한다. 어떻게 구현하면 될까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-29-1.png"/>
</p>

* Material을 하나하나 만들어서 넣으면 될까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-29-2.png"/>
</p>

* 매번 이 많은 Pipeline을 돌면서?? 이건 너무 비효율적이다 
    * 👉 DirectX에서 효율적인 방법을 준비했다.

```cpp
// Instancing이라는 기법으로 여러 Particle을 표현할 예정이다.
// Instancing이란 뭐냐하면 기존에는 하나의 Object(Material)를 
// Pipeline으로 보냈다고 한다면

// Instancing으로 Pipeline으로 하나의 Object를 보내고
// 출력은 여러개를 뽑는 방법이다.

void Mesh::Render()
{
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

    // DrawIndexedInstanced에서 몇개의 Instance를 그릴지 지정
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, 1/* 현재는 무조건 한 개로 그린다 */, 0, 0, 0);

    // 아니 이것도 결국 Rendering해달라는거 아닌가?
        // 약간 다른게 정점정보(IA)를 한 번만 넘겨줘도 된다는 점에서 다르다.
        // 정점이 간단하면 모르지만 복잡해진다면 연산에 큰 차이를 보인다.
}
```

* `DrawIndexedInstanced`를 사용시 DirectX에서 IA(Input Assembler Stage)를 제외하고 나머지 Pipeline을 돌아준다

---

### 2. 각 Particle의 위치, 속도, 크기 등은 어디서 지정하나?

```cpp
// params.fx

// Particle을 Constant Buffer로 넘기면될까?
    // Constant Buffer로 넘기는 가장 대표적인 Light를 보자면

cbuffer GLOBAL_PARAMS : register(b0)
{
    int         g_lightCount;
    float3      g_lightPadding;
    LightInfo   g_light[50];        
    // 딱 50개만 넘길수 있게 만들었는데 
    // 사실 Particle이 몇개 일거란 보장이 없다.
        // 이걸 1000개로 바꿔서 만들어 볼까?
        // 나중에 2000개가 필요하면?? 3000개면??? -> 다른 방법을 고려해보자
}
```

* 또 다른 문제1) Constant Buffer이용시 매번 CPU메모리를 GPU로 옮겨야 하는데 그 속도 또한 매우 느리다. Particle이 커지면 커질수록 부담도 같이 커지게 된다.
* GPU로 Particle의 위치, 속도, 크기등을 CPU에서 넘기는게 아니라 필요한 정보를 GPU자체에서 연산(Compute Shader)을 해서 쓰게 만들어 줘야한다.
    * (추가 설명) Constant Buffer를 GPU에서 쓸 수 없음(이름그대로 상수) 따라서 Constant Buffer에 값에 연산이 필요할 경우 또 새로운 메모리를 잡아야 한다.
* 또 다른 문제2) 생성된 Particle을 매번 갖고 있어야 할까 메모리 부담이 상당할꺼 같은데?? 
    * 그려지면 안된다의 판별기준이 필요해진다. (Geometry Shader를 이용하게 되는데, 자세한건 아래서 참조)

- 결론은 Particle System의 경우 어디 그려질지, 어떤크기로 그려질지, 언제 그려질지 등은 GPU내에서 별도로 결정하게 해야한다는 것!
- 이렇게 생각하면 편할듯 Compute Shader에서 Particle이 어디,크기,언제 그려질지 결정 후 Pipeline으로 보내게 된다. Pipeline을 돌며 그려지며 특히 Geometry Shader에서 그려질지 말지 결정됨을 주의해서 보자

---

## 구현

### 쉐이더 분석

```cpp
// particle.fx

struct Particle
{
    float3  worldPos;
    float   curTime;
    float3  worldDir;
    float   lifeTime;
    int     alive;
    float3  padding;
};

// ...

struct ComputeShared
{
    int addCount;
    float3 padding;
};

// RWStructuredBuffer : 아직은 이해가 잘 되지 않겠지만 배열이라 생각하고
// u0에 배열의 주소를 넣어놨다고 생각하면된다.
RWStructuredBuffer<Particle> g_particle : register(u0);

// GPU Thread에서 공용으로 사용할 메모리(변수)
RWStructuredBuffer<ComputeShared> g_shared : register(u1);
```

```cpp
// Compute Shader가 먼저 사용이 되기에 먼저 분석해 보자면

// CS_Main
// g_vec2_1 : DeltaTime / AccTime
// g_int_0  : Particle Max Count
// g_int_1  : AddCount
// g_vec4_0 : MinLifeTime / MaxLifeTime / MinSpeed / MaxSpeed
[numthreads(1024, 1, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    /*
        // Compute Shader 사용시 절대 주의사항!
        g_particle[threadIndex.x - 1] = XXX;
        * 나 말고 다른 Index의 Thread를 건드리면 절대 안된다.
        * 병렬로 처리되고 있기에 연산이 끝났는지 안끝났는지 알 수없음.

        * 공유자원(g_shared)를 사용할시에 주의해야한다. *
    */

    if (threadIndex.x >= g_int_0)
        return;

    int maxCount = g_int_0;
    int addCount = g_int_1;
    int frameNumber = g_int_2;
    float deltaTime = g_vec2_1.x;
    float accTime = g_vec2_1.y;
    float minLifeTime = g_vec4_0.x;
    float maxLifeTime = g_vec4_0.y;
    float minSpeed = g_vec4_0.z;
    float maxSpeed = g_vec4_0.w;

    // 이거 너무 위험하지 않나?
    // g_shared는 모든 thread에서 접근하는 변수인데 이렇게 값을 세팅??
    // 아래서 보완코드를 넣음.
    g_shared[0].addCount = addCount;

    // GroupMemoryBarrierWithGroupSync();가 신호등이 된다.
    // 나머지 Thread는 기다려!
    GroupMemoryBarrierWithGroupSync();

    // 기존에 그리지 않고 있었다면 ( alive == 0 )
    if (g_particle[threadIndex.x].alive == 0)
    {
        // 공유자원 접근을 위한 코드
        while (true)
        {
           // 부활이 가능한지 체크한다. remaining이 0이면 부활불가능.
            int remaining = g_shared[0].addCount;
            if (remaining <= 0)
                break;

            int expected = remaining;
            int desired = remaining - 1;
            int originalValue;
            InterlockedCompareExchange(g_shared[0].addCount, expected, desired, originalValue);

            if (originalValue == expected)
            {
                g_particle[threadIndex.x].alive = 1;
                break;
            }
        }

        // 아래 연산은 Particle을 대략적 계산(크게 중요하진 않음.)

        if (g_particle[threadIndex.x].alive == 1)
        {
            float x = ((float)threadIndex.x / (float)maxCount) + accTime;

            float r1 = Rand(float2(x, accTime));
            float r2 = Rand(float2(x * accTime, accTime));
            float r3 = Rand(float2(x * accTime * accTime, accTime * accTime));

            // [0.5~1] -> [0~1]
            float3 noise =
            {
                2 * r1 - 1,
                2 * r2 - 1,
                2 * r3 - 1
            };

            // [0~1] -> [-1~1]
            float3 dir = (noise - 0.5f) * 2.f;

            g_particle[threadIndex.x].worldDir = normalize(dir);
            g_particle[threadIndex.x].worldPos = (noise.xyz - 0.5f) * 25;
            g_particle[threadIndex.x].lifeTime = ((maxLifeTime - minLifeTime) * noise.x) + minLifeTime;
            g_particle[threadIndex.x].curTime = 0.f;
        }
    }
    else
    {
        g_particle[threadIndex.x].curTime += deltaTime;
        if (g_particle[threadIndex.x].lifeTime < g_particle[threadIndex.x].curTime)
        {
            g_particle[threadIndex.x].alive = 0;
            return;
        }

        float ratio = g_particle[threadIndex.x].curTime / g_particle[threadIndex.x].lifeTime;
        float speed = (maxSpeed - minSpeed) * ratio + minSpeed;
        g_particle[threadIndex.x].worldPos += g_particle[threadIndex.x].worldDir * speed * deltaTime;
    }
}
```

```cpp
// 랜덤을 만들기 위한 함수
// 일반적으로 이렇게 쓰니 너무 고민 말고 그냥 쓰자.
	// 혹은 노이즈 텍스쳐로 랜덤함수를 만드는데 그냥 알아만 두자.
float Rand(float2 co)
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}
```

* Compute Shader의 연산을 마친 후

```cpp
// Compute Shader에 의해 연산된 결과가 
// Vertex Shader에 들어가게 된다.
    // (t9인걸로 봐선 Texture로 들어감)
StructuredBuffer<Particle> g_data : register(t9);

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float id : ID;
};

// VS_MAIN
// g_float_0    : Start Scale
// g_float_1    : End Scale
// g_tex_0      : Particle Texture

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;

    // index로 particle을 찾아서 위치를 변환해 준다
    worldPos += g_data[input.id].worldPos;

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.id = input.id;

    return output;
}
```

* 마지막으로 Geometry Shader에서 그려줄지 말지 결정

```cpp
struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
};

[maxvertexcount(6)]
void GS_Main(point/*점 하나만 넘어갑니다.*/ VS_OUT input[1], 
inout TriangleStream<GS_OUT> outputStream)
{
    // Vertex Shader의 결과(input)가 Geometry Shader로 들어가게 되고
    // 

    GS_OUT output[4] =
    {
        (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
    };

    VS_OUT vtx = input[0];
    uint id = (uint)vtx.id;
    if (0 == g_data[id].alive)
        // 그릴 필요가 없는경우 그냥 return 해버리자.
        return;

    float ratio = g_data[id].curTime / g_data[id].lifeTime;
    float scale = ((g_float_1 - g_float_0) * ratio + g_float_0) / 2.f;

    // View Space
    output[0].position = vtx.viewPos + float4(-scale, scale, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale, scale, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale, -scale, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale, -scale, 0.f, 0.f);

    // Projection Space
    output[0].position = mul(output[0].position, g_matProjection);
    output[1].position = mul(output[1].position, g_matProjection);
    output[2].position = mul(output[2].position, g_matProjection);
    output[3].position = mul(output[3].position, g_matProjection);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    output[0].id = id;
    output[1].id = id;
    output[2].id = id;
    output[3].id = id;

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}
```

---

### C++ 분석

```cpp
class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	void Init(uint32 elementSize, uint32 elementCount);

	void PushGraphicsData(SRV_REGISTER reg);
	void PushComputeSRVData(SRV_REGISTER reg);
	void PushComputeUAVData(UAV_REGISTER reg);

	ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
	ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

	void SetResourceState(D3D12_RESOURCE_STATES state) { _resourceState = state; }
	D3D12_RESOURCE_STATES GetResourceState() { return _resourceState; }
	ComPtr<ID3D12Resource> GetBuffer() { return _buffer; }

private:
	ComPtr<ID3D12Resource>			_buffer;
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;
	ComPtr<ID3D12DescriptorHeap>	_uavHeap;

	uint32						_elementSize = 0;
	uint32						_elementCount = 0;
	D3D12_RESOURCE_STATES		_resourceState = {};

private:
	D3D12_CPU_DESCRIPTOR_HANDLE _srvHeapBegin = {};
	D3D12_CPU_DESCRIPTOR_HANDLE _uavHeapBegin = {};
};
```

```cpp
struct ParticleInfo
{
	Vec3	worldPos;
	float	curTime;
	Vec3	worldDir;
	float	lifeTime;
	int32	alive;
	int32	padding[3];
};

struct ComputeSharedInfo
{
	int32 addCount;
	int32 padding[3];
};

class ParticleSystem : public Component
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

public:
	virtual void FinalUpdate();
	void Render();

public:
	virtual void Load(const wstring& path) override { }
	virtual void Save(const wstring& path) override { }

private:
	shared_ptr<StructuredBuffer>	_particleBuffer;
	shared_ptr<StructuredBuffer>	_computeSharedBuffer;
	uint32							_maxParticle = 1000;

	shared_ptr<Material>		_computeMaterial;
	shared_ptr<Material>		_material;
	shared_ptr<Mesh>			_mesh;

	float				_createInterval = 0.005f;
	float				_accTime = 0.f;

	float				_minLifeTime = 0.5f;
	float				_maxLifeTime = 1.f;
	float				_minSpeed = 100;
	float				_maxSpeed = 50;
	float				_startScale = 10.f;
	float				_endScale = 5.f;
};
```

```cpp
ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM)
{
	_particleBuffer = make_shared<StructuredBuffer>();
	_particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

	_computeSharedBuffer = make_shared<StructuredBuffer>();
	_computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

	_mesh = GET_SINGLE(Resources)->LoadPointMesh();
	_material = GET_SINGLE(Resources)->Get<Material>(L"Particle");
	shared_ptr<Texture> tex = GET_SINGLE(Resources)->Load<Texture>(
		L"Bubbles", L"..\\Resources\\Texture\\Particle\\bubble.png");

	_material->SetTexture(0, tex);

	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeParticle");
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::FinalUpdate()
{
	_accTime += DELTA_TIME;

	int32 add = 0;
	if (_createInterval < _accTime)
	{
		_accTime = _accTime - _createInterval;
		add = 1;
	}

	_particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
	_computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

	_computeMaterial->SetInt(0, _maxParticle);
	_computeMaterial->SetInt(1, add);

	_computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
	_computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

	_computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render()
{
	GetTransform()->PushData();

	_particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
	_material->SetFloat(0, _startScale);
	_material->SetFloat(1, _endScale);
	_material->PushGraphicsData();

	_mesh->Render(_maxParticle);
}
```