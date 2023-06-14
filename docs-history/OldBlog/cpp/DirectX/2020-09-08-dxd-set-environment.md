---
title: "Direct3D : Direct3D 개발 환경 조성"
permalink: dxd/environment/                # link 직접 지정
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
  - 개발환경
excerpt: ""
classes: wide
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Direct3DExample/tree/master/Dxd-1)

![](/file/image/dxd-1-1.png)

단축키는 필요없기에 코드 삭제

```cpp
HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXD));
```

```cpp
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DXD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXD));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        //{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        //}
    }

    return (int) msg.wParam;
}
```

핸들은 전역으로 쓸 수 있게 전역화

```cpp
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   
   HWND hWnd;
   g_hWnd = hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

// ...
```

`GetMessage()` 말고 `PeekMessage()`를 활용해보자

```cpp
/*
while (GetMessage(&msg, nullptr, 0, 0))
{
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
*/

while (true)
{
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else {
        // 업데이트와 렌더링
    }
}
```

---

## Direct 3D 구조

![](/file/image/dxd-1-2.png)

---

## Direct 3D 초기화 및 생성

* 추가 포함 디렉터리 : `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include`
* 추가 라이브러리 디렉터리 : `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64`
* 포함 라이브러리 : dxguid.lib, d3d9.lib, d3dx9.lib

```cpp
#include <d3d9.h>
```

---

## Direct3D Device 생성

* Direct3D Device : 모든 Direct3D SDK를 쓰기위한 장비? 라고 생각하자

```cpp
// Direct 3D 및 Direct 3D Deive 변수 선언
LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
```

```cpp
HRESULT InitD3D(HWND hWnd)
{
	// Direct 3D 변수 초기화
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Direct 3D 파라미터 선언
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// back, front buffer를 번갈아 써달라
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// D3D format을 윈도우와 동일하게 해달라.

	// Direct 3D를 이용해 Direct 3D Device를 생성해 달라
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pd3dDevice)))
	{
		return E_FAIL;
	}
}
```

---

## 초기화 후 출력

* 이중 버퍼링 : 메모리를 front, back으로 나누어 관리한다.
* 페이지 플리핑(page flipping) : 이중 버퍼의 메모리 front, back을 한번씩 번갈아 가면서 front, back을 쓰는 방법 (렌더링 속도를 늘린다)

```cpp
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;

	// 백버퍼 지우기
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// 렌더링 준비
	g_pd3dDevice->BeginScene();

	// Render here

	// 렌더링 종료
	g_pd3dDevice->EndScene();

	// 버퍼 교체(페이지 플리핑)
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
```

---

## Direct 3D Deive 및 Direct 3D 의 메모리 해제

```cpp
VOID Cleanup()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();
	if (g_pD3D != NULL)
		g_pD3D->Release();
}
```

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
		Render();
        break;
    case WM_DESTROY:
		Cleanup();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
```