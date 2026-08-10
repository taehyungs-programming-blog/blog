---
layout: default
title: "32. Tessellation"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/26)

## 이론

* **Tesslator**란?? 👉 정점을 추가/삭제하는 Stage이다.

* 그럼 **Geometry Stage**와 차이점이 있나? -> Geometry Stage가 먼저나왔고 Tesslator는 이후에 나온 개념으로 Geometry보다 기능이 방대하기에 더 많은 상황에서 사용이 가능하다(일단 이정도만 알고있자.)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-29-2.png"/>
</p>

* Tesslator는 다음 세 가지 Stage로 구성이 되어있다.
    * Hull Shader
    * Tessellator
    * Domain Shader
* 참고로 Hull, Domain Shader는 개발자가 작업하고 
* Tessellator는 DirectX에서 알아서 해준다

* **LOD(Level Of Detail)** - 같은 오브젝트라도 폴리곳 갯수별로 다르게 그려주는 기술
    * Ex) 카메라에서 멀어질수록 낮은 레벨의 LOD를 사용한다 든지
* 그럼 LOD는 어떻게 적용할까?
    * Mesh를 매번 바꿔 끼우나?
    * 그럴수도 있지만, DirectX에서 알아서 잘(Tesslator) 해주면 좋겠지? 👉 동적 LOD

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-2.jpg"/>
</p>

* **Control Point** & **Patch** 
    * **Control Point** - 정점 하나하나를 Control Point라한다.
    * **Patch** - Control Point의 묶음(Group)을 Patch라 한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-3.png"/>
</p>

* 아니 그냥 Vertex라 하지 왜 어렵게 Control Point라 하나?
    * 약간 개념이 다르다 Control Point는 딱 그 점대로 그려준다기 보단, 아래 그림처럼 Control Point에 따라 그려질 선을 정의한다는 개념이 강하다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-4.png"/>
</p>

---

## 구현

* 대략 Shader를 보면서 어떻게 처리될지 예상해 보자.

```cpp
// Tessellation.fx

/* --------------
// Vertex Shader
* 그냥 Vertex정보만 넘긴다.
 -------------- */

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = input;

    return output;
}
```

* Hull Shader와 Control Point HS는 병렬적으로 돌아가며
* input으로 Vertex의 결과물(`InputPatch<VS_OUT, 3> input`)이 들어감을 주목하자.

```cpp
// --------------
// Hull Shader
// --------------

struct PatchTess
{
    float edgeTess[3] : SV_TessFactor;
    float insideTess : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

// Constant HS
PatchTess ConstantHS(InputPatch<VS_OUT, 3> input, int patchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess)0.f;

    output.edgeTess[0] = 1;
    // 0번 변에 대해서 몇개로 쪼갤 것인가? -> 1이기에 쪼개지 않음
    output.edgeTess[1] = 2;
    // 1번 변에 대해 몇개로 쪼갤 것인가 -> 2이기에 두 개로 쪼갬
    output.edgeTess[2] = 3;
    // 2번 변에 대해 몇개로 쪼갤 것인가 -> 3이기에 세 개로 쪼갬
    output.insideTess = 1;
    // 몇개의 점으로 모을 것인가?

    return output;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-5.png"/>
</p>

* 삼각형이 아래와 같이 쪼개진다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-6.png"/>
</p>

```cpp
// Control Point HS
[domain("tri")]             // 패치의 종류 (tri, quad, isoline)
[partitioning("integer")]   // subdivision mode (integer 소수점 무시, 
                            // fractional_even, fractional_odd)
[outputtopology("triangle_cw")] // (triangle_cw, triangle_ccw, line)
[outputcontrolpoints(3)]     // 하나의 입력 패치에 대해, HS가 출력할 제어점 개수
[patchconstantfunc("ConstantHS")] // ConstantHS 함수 이름

// Control Point별로 들어온다
    // Control Point(정점)가 3개라면 세 번 들어옴
HS_OUT HS_Main(InputPatch<VS_OUT, 3> input,  // 컨트롤 포인트 정보 어떤 컨트롤 포인트인지는 아래 index로 알수 있음.
                int vertexIdx : SV_OutputControlPointID,    // Control Point index
                int patchID : SV_PrimitiveID)               // Patch index
{
    // 현재는 특별한 동작없이 들어온 그대로 출력으로 넘김
    HS_OUT output = (HS_OUT)0.f;

    output.pos = input[vertexIdx].pos;
    output.uv = input[vertexIdx].uv;

    return output;
}
```

### Domain Shader

* Hull, Tesselator를 거치고 들어온다.
* Hull, Tesselator를 거쳤다면 Control Point가 늘어난 상태일 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-32-7.png"/>
</p>

```cpp
// --------------
// Domain Shader
// --------------

struct DS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> input,  // Control Point 정보
                float3 location : SV_DomainLocation, // Control Point 대비 어디쯤 위치했나 정보(위에 그림 참고)
                PatchTess patch)
{
    DS_OUT output = (DS_OUT)0.f;

    // location 정보를 찾고싶다면 비율로 찾으면 된다.
    float3 localPos = input[0].pos * location[0] + input[1].pos * location[1] + input[2].pos * location[2];
    float2 uv = input[0].uv * location[0] + input[1].uv * location[1] + input[2].uv * location[2];

    // View를 변경하여 넘기자
    output.pos = mul(float4(localPos, 1.f), g_matWVP);
    output.uv = uv;

    return output;
}
```

---

## C++ 코드 추가된 부분

```cpp
	// Tessellation
	{
		ShaderInfo info =
		{
			SHADER_TYPE::FORWARD,
            // Frame이 보이게 WireFrame으로 선언
			RASTERIZER_TYPE::WIREFRAME,
			DEPTH_STENCIL_TYPE::LESS,
			BLEND_TYPE::DEFAULT,
            // Control Point Patch로 생성
			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST
		};

        // 넘겨야 할 값이 많아 Arg를 별도로 만듦
		ShaderArg arg =
		{
			"VS_Main",
			"HS_Main",
			"DS_Main",
			"",
			"PS_Main",
		};

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->CreateGraphicsShader(L"..\\Resources\\Shader\\tessellation.fx", info, arg);
		Add<Shader>(L"Tessellation", shader);
	}
}
```