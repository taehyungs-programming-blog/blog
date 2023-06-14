---
title: "Direct3D : index buffer"
permalink: dxd/d3d-index-buffer/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-25 00:00:00 -0000
last_modified_at: 2020-10-25 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - C++
  - DirectX
category:
  - index buffer
excerpt: ""
classes: wide
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [강의](https://www.youtube.com/watch?v=rCwXyIp77Xs&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=21)

---

## 인덱스 버퍼

* 정점 버퍼에 들어간 순서인 인덱스를 이용하여 삼각형을 구성하는 버퍼
* 인덱스 버퍼가 필요한이유? 정점 버퍼의 경우 중복되는 영역이 발생하게 되는데 그 중복되는 정점 버퍼를 인덱스로 대신처리해보자는데서 아이디어가 나옴.
    * 예를들어 정육면체를 그냥 정점버퍼로 그린다면 3*2*6 = 36개의 정점버퍼가 필요하나 인덱스 버퍼로 처리하면 8개의 인덱스 버퍼만으로 정육면체를 표현할 수 있다

---

## 인덱스 버퍼를 이용한 정점 렌더링 순서

1. 정점과 인덱스 선언
2. 정점 버퍼 생성 및 정점 입력
3. 인덱스 버퍼 생성 및 인덱스 입력
4. 인덱스를 이용한 출력(`DrawIndexedPrimitive`)

---

## Example

```cpp
// 정점과 인덱스 선언
#pragma once
#include <d3d9.h>
class CCube
{
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;  
	LPDIRECT3DINDEXBUFFER9 m_pIB;
    // ...
```

```cpp
void CCube::OnInit( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

    // 정점 선언
	CUSTOMVERTEX Vertices[] = {
		{ D3DXVECTOR3(-1.0f, 1.0f,  1.0f) ,  D3DCOLOR_RGBA( 255, 0, 255, 255) },
		{ D3DXVECTOR3( 1.0f, 1.0f,  1.0f)  , D3DCOLOR_RGBA( 0, 255, 0, 255) },
		{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f)  , D3DCOLOR_RGBA( 0, 0, 255, 255) },
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f)  , D3DCOLOR_RGBA( 255, 255, 0, 255) },

		{ D3DXVECTOR3(-1.0f, -1.0f,  1.0f) , D3DCOLOR_RGBA( 255, 0, 0, 255) },
		{ D3DXVECTOR3( 1.0f, -1.0f,  1.0f) , D3DCOLOR_RGBA( 0, 255, 10, 255) },
		{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f) , D3DCOLOR_RGBA( 0, 0, 255, 255) },
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f) , D3DCOLOR_RGBA( 255, 0, 0, 255) }
	};	

	// 인덱스 선언
	WORD Indices[] = 
	{
			0, 1, 2,
			0, 2, 3,
			4, 6, 5,
			4, 7, 6,
			0, 3, 7,
			0, 7, 4,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			0, 4, 5,
			0, 5, 1
	};
```

```cpp
void CCube::OnInit( LPDIRECT3DDEVICE9 pd3dDevice )
    //...


	// 정점 버퍼 생성 및 설정
	
	m_pd3dDevice->CreateVertexBuffer( sizeof(Vertices),  0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pVB, NULL );
	
	void* pVertices;   
	m_pVB->Lock( 0, sizeof(Vertices), &pVertices, 0 );	
	memcpy( pVertices, Vertices, sizeof(Vertices) );
	m_pVB->Unlock();

	// 인덱스 버퍼 생성 및 설정	
	m_pd3dDevice->CreateIndexBuffer( sizeof(Indices), 
		     0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL );	
	
	void* pIndices;
	m_pIB->Lock( 0, sizeof(Indices), &pIndices,  0 );
	memcpy( pIndices, Indices, sizeof(Indices) );
	m_pIB->Unlock();
}
```

```cpp
// 렌더링 처리
void CCube::OnRender()
{
    // SetRenderState : 렌더링 상태 옵션 변경
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE);
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );

    // 인덱스 버퍼를 디바이스에 세팅
	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );	

    // 인덱스 버퍼를 출력
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 6*2 ); // sizeof( Indices ) / sizeof(WORD) / 3
	
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW);
}
```