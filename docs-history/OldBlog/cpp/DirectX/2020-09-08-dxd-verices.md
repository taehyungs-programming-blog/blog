---
title: "Direct3D : Verices"
permalink: dxd/verices/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-08 00:00:00 -0000
last_modified_at: 2020-09-08 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - C++
  - DirectX
category:
  - verices
excerpt: ""
classes: wide
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [강의](https://www.youtube.com/watch?v=ExFyJAJMuKo&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=8)

---

## 이론에 대한 설명

* 쉽기에 중요하다 생각되는 내용만 정리
* Direct3D는 왼손좌표계를 사용한다.
* 벡터에 대한 설명

---

## 벡터의 합

* [Get Code](https://github.com/EasyCoding-7/Direct3DExample/tree/master/Dxd-3)

```cpp
#include <d3dx9math.h>

// ...

D3DXVECTOR3 v1(0.f, 3.0f, 0.0f);
D3DXVECTOR3 v2(3.0f, 0.0f, 0.0f);
D3DXVECTOR3 v3;
D3DXVECTOR3 v4(3.0f, 3.0f, 0.0f);
float fLength;

// 벡터의 덧셈
v3 = v1 + v2;               // 3.0f, 3.0f, 0.0f
D3DXVec3Add(&v3, &v1, &v2); // 3.0f, 3.0f, 0.0f

// 벡터의 뺄셈
v3 = v1 - v2;
D3DXVec3Subtract(&v3, &v1, &v2);
```

---

## 벡터의 크기

* [강의](https://www.youtube.com/watch?v=ZFOnOHiwprI&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=9)

```cpp
// 벡터의 크기 구하기
fLength = D3DXVec3Length(&v4);  // 4.24264050

// 벡터의 크기 변환
float fScale = 2.0f;
D3DXVECTOR3 v5(2.0f, 1.0f, 0.0f);

D3DXVec3Scale(&v5, &v5, fScale); // 4.0f, 2.0f, 0.0f

// 단위 벡터(normalized vector)
D3DXVECTOR3 v6(2.0f, 2.0f, 3.0f);
D3DXVECTOR3 vResult;
float fNormalize;

D3DXVec3Normalize(&vResult, &v6);
fNormalize = D3DXVec3Length(&vResult);
```

---

## 벡터의 외적과 내적

* [강의](https://www.youtube.com/watch?v=xYDzdQAUF8E&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=10)

* 내적 : 투영된 길이를 구할때 사용

```cpp
D3DXVECTOR3 v7(3.0f, 0.0f, 0.0f);
D3DXVECTOR3 v8(-3.0f, 0.0f, 0.0f);
float fCos, fDot, fScale;

fDot = D3DXVec3Dot(&v7, &v8);
fScale = D3DXVec3Length(&v7) * D3DXVec3Length(&v8);
fCos = fDot / fScale;
```

* 외적 : 두 벡터가 이루는 평면의 수직 벡터(두 벡터로 이뤄진 한 평면이 앞인지 뒤인지 확인할때 사용)

```cpp
D3DXVECTOR3 v9(3.0f, 0.0f, 0.0f);
D3DXVECTOR3 v10(0.0f, 3.0f, 0.0f);
D3DXVECTOR3 vResult;

D3DXVec3Cross(&vResult, &v1, &v2);
D3DXVec3Normalize(&vResult, &vResult);
```

---

## 정점의 구조

* [강의](https://www.youtube.com/watch?v=cPFikK8_GFs&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=11)

vertice는 아래와 같이 구성해야한다.

![](/file/image/dxd-vertices-1.png)

흠... 뭔소린지... 예시를 통해서 설명한다.

* 정점을 넣어주는 방향은 시계방향임을 기억하자.

```cpp
struct CUSTOMVERTEX
{
  FLOAT x, y, z, rhw; // rhw : 2d 좌표계를 사용하겠다
  DWORD color;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)  // diffuse : color를 의미

CUSTOMVERTEX vertices[] = {
  {150.0f, 50.0f, 0.5f, 1.0f ,0xffff0000, },
  {250.0f, 250.0f, 0.5f, 1.0f ,0xff00ff00, },
  {50.0f, 250.0f, 0.5f, 1.0f ,0xff00ffff, },
};
```

---

## 정점 버퍼 생성

* [강의](https://www.youtube.com/watch?v=kSTE0_q6p9M&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=12)

```cpp
LPDIRECT3DVERTEXBUFFER9 pVB;
if(FAILED(pd3dDevice->CreateVertexBuffer(
  3*sizeof(CUSTOMVERTEX), 
  0,
  D3DFVF_CUSTOMVERTEX,
  D3DPOOL_DEFAULT,
  &pVB,
  NULL
)));
```

vertices 버퍼 복사하기

```cpp
VOID* pVertices;
if(FAILED(pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
  return E_FAIL;

memcpy(pVertices, vertices, sizeof(vertices));
pVB->Unlock();
```

여기까지 하면 정점을 그릴 준비가 된 것이다.

렌더링하기 위한 정점 버퍼의 설정

```cpp
pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));
```

정점 구조를 설정

```cpp
pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
```

정점출력

```cpp
d3dD3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, 6); // 점 단위로 출력
d3dD3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 3); // 선 단위로 출력
d3dD3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 5); // 선 스트림 으로 출력
d3dD3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2); // 삼각형 으로 출력
d3dD3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 4); // 삼각형 스트림 으로 출력
d3dD3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 4); // 삼각형 팬 으로 출력
```

---

## 직접만들어보자

* [강좌](https://www.youtube.com/watch?v=RZLiP3t_D48&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=13)
* [Get Code](https://github.com/EasyCoding-7/Direct3DExample/tree/master/Dxd-3-1)

우선 첫 번째 방법은 그냥 CGameEdu01에 그리는 것이다.

```cpp
#pragma once
#include <d3d9.h>
class CD3DApp
{
protected:
	LPDIRECT3D9			m_pD3D;
	LPDIRECT3DDEVICE9	m_pd3dDevice;
	HWND				m_hWnd;

	virtual void OnInit() = 0;
	virtual void OnRender() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRelease() = 0;

public:
	HRESULT InitD3D(HWND hWnd);
	void Render();
	void Update();
	void Cleanup();

public:
	CD3DApp(void);
	~CD3DApp(void);
};
```

```cpp
#include "CD3DApp.h"

CD3DApp::CD3DApp(void)
{

}

CD3DApp::~CD3DApp(void)
{

}

HRESULT CD3DApp::InitD3D(HWND hWnd) 
{
	// Direct 3D 변수 초기화
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Direct 3D 파라미터 선언
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// back, front buffer를 번갈아 써달라
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// D3D format을 윈도우와 동일하게 해달라.

	// Direct 3D를 이용해 Direct 3D Device를 생성해 달라
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pd3dDevice)))
	{
		return E_FAIL;
	}

	OnInit();

	return S_OK;
}

void CD3DApp::Render() 
{
	if (NULL == m_pd3dDevice)
		return;

	// 백버퍼 지우기
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// 렌더링 준비
	m_pd3dDevice->BeginScene();

	// Render here
	OnRender();

	// 렌더링 종료
	m_pd3dDevice->EndScene();

	// 버퍼 교체(페이지 플리핑)
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void CD3DApp::Update()
{
	OnUpdate();
}

void CD3DApp::Cleanup()
{
	OnRelease();

	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Release();
	if (m_pD3D != NULL)
		m_pD3D->Release();
}
```

* [Get Code](https://github.com/EasyCoding-7/Direct3DExample/tree/master/Dxd-3-2)

두 번째 방법은 Triangle 클래스를 새로 만드는 것이다.

```cpp
#pragma once
#include <d3d9.h>

struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

class CTriangle
{
	LPDIRECT3DVERTEXBUFFER9 pVB;			// Vertex Buffer
	LPDIRECT3DDEVICE9 m_pd3dDevice;			// Direct3D device

public:

	void OnInit(LPDIRECT3DDEVICE9 pd3dDevice);
	void OnRender();
	void OnUpdate();
	void OnRelease();

	CTriangle();
	~CTriangle();
};
```

```cpp
#include "stdafx.h"
#include "CTriangle.h"


CTriangle::CTriangle()
{
}


CTriangle::~CTriangle()
{
}

void CTriangle::OnInit(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	// Vertex info
	CUSTOMVERTEX vertices[3] =
	{
	{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, },
	{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
	{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};

	// CreateVertexBuffer for draw Primitive
	m_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &pVB, NULL);

	// Memory Lock for Vertex Buffer
	VOID* pVertices;
	pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	pVB->Unlock();
}

void CTriangle::OnRender()
{
	// Device stream setting for rendering
	m_pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(CUSTOMVERTEX));
	m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

void CTriangle::OnUpdate()
{
	// Changing of vertex position
}

void CTriangle::OnRelease()
{
	// Release Vertex Buffer
	if (pVB != NULL)
		pVB->Release();
}
```

```cpp
#pragma once
#include "CD3DApp.h"
#include "CTriangle.h"

class CGameEdu01 : public CD3DApp
{
private:
	CTriangle m_Triangle;

protected:
	virtual void OnInit();
	virtual void OnRender();
	virtual void OnUpdate();
	virtual void OnRelease();

public:
	CGameEdu01(void);
	~CGameEdu01(void);
};
```

```cpp
#include "CGameEdu01.h"

CGameEdu01::CGameEdu01(void)
{

}

CGameEdu01::~CGameEdu01(void)
{

}

void CGameEdu01::OnInit()
{
	m_Triangle.OnInit(m_pd3dDevice);
}

void CGameEdu01::OnRender()
{
	m_Triangle.OnRender();
}

void CGameEdu01::OnUpdate()
{

}

void CGameEdu01::OnRelease()
{
	m_Triangle.OnRelease();
}
```