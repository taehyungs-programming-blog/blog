---
title: "Direct3D : Direct3D Frameworkd"
permalink: dxd/d3d-framework/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-24 00:00:00 -0000
last_modified_at: 2020-10-24 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - C++
  - DirectX
category:
  - direct3d framework
excerpt: ""
classes: wide
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [강의](https://www.youtube.com/watch?v=H2rlAm8oC_Y&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=54)

---

## DirectX Sample Browser 사용하기

![](/file/image/dxd-d3d-framework-1.png)

DirectX Sample Browser의 분석을 해보는 것을 추천한다.

![](/file/image/dxd-d3d-framework-2.png)

---

## DXUT

* [Get Code](https://github.com/EasyCoding-7/Direct3DExample/tree/master/EmptyProject)

우선 Empty Project를 받아보자

![](/file/image/dxd-d3d-framework-3.png)

DXUT에 대해 확인해 보자

![](/file/image/dxd-d3d-framework-4.png)

* DXUT : Direct3D Framework 관련 대부분의 전역함수와 콜백함수들이 있다
* DXUTcamera : 카메라 클래스가 있어서 툴과 게임에서 쉽게 카메라를 사용할 수 있게 해준다.(주로 툴을 만들때 사용된다.)
* DXUTenum : Direct3D 어뎁터와 디바이스와 모드에 대한 설정함수가 있다
* DXUTgui : Direct3D 다이얼로그, 리소스 관리자, 컨트롤을 구현한 클래스
* DXUTmisc : 타이머, 화면등에 관한 함수와 클래스
* DXUTres : Direct3D에서 제공되는 리소스 아이디
* DXUTsettingsdlg : Direct3D를 셋업하기 위한 Dialog GUI 클래스
* SDKmesh : 메쉬를 다루기 위한 클래스
* SDKmisc : 메쉬에 사용되는 텍스쳐와 테스트 출력등에 관한 클래스

---

## Direct3D Framework의 구조

* [강의](https://www.youtube.com/watch?v=DnoyhQ6OhMY&list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1&index=55)

초기화 -> Data 갱신 -> 렌더링 -> (종료 요청 시) -> 해제

---

## Direct3D 프레임워크에서 실행되는 콜백함수

### OnD3D9CreateDevice()

  * 특정한 d3d device에 대한 설정
  * 백버퍼에 대한 설정
  * 카메라와 프로젝션 설정
  * d3d device에 한 번만 설정하는 코드
  * `DXUTSetCallbackDeviceCreated()`에서 호출

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // ...
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    // ...
```

```cpp
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    return S_OK;
}
```

### OnD3D9ResetDevice

  * device가 소실되었을 경우
  * device에 연관된 객체를 다시 셋업
  * `DXUTSetCallbackDeviceReset()` 에서 호출

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // ...
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
```

```cpp
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
    return S_OK;
}
```

### OnFrameMove

  * 데이터 갱신 함수
  * `DXUTSetCallbackFrameMove()` 에서 호출

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // ...
    DXUTSetCallbackFrameMove( OnFrameMove );
```

```cpp
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}
```

### OnD3D9FrameRender

  * 렌더링 함수
  * `DXUTSetCallbackFrameRender()` 에서 호출

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // ...
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
```

```cpp
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        V( pd3dDevice->EndScene() );
    }
}
```

### MsgProg

  * 응용프로그램 윈도우 메시지 처리
  * WndProc와 같은 역할
  * `DXUTSetCallbackMsgProg` 에서 호출

```cpp
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}
```

### OnD3D9LostDevice

  * device에 연관된 객체를 해제
  * `DXUTSetCallbackDeviceLost()` 에서 호출

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // ...
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
```

```cpp
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
}
```

### wWinMain

  * 게임프로그래밍 전체 구조

```cpp
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
  // ...
```
