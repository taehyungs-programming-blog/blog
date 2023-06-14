---
title: "(Win32, MFC) Dll 함수사용"
permalink: cpp/win32/use-dll-func/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-22 00:00:00 -0000
last_modified_at: 2020-09-22 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - use-dll-func
  - GetProcAddress
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://3dmpengines.tistory.com/590)

---

```cpp
// 뭐 대략 이런식으로 쓴다
HMODULE pTestDXLib;
FARPROC pFunctionality;

// Dll을 로드
pTestDXLib=LoadLibrary("DSOUND");

// 함수를 가져온다
pFunctionality = GetProcAddress(pTestDXLib, (char*) 7);
// Ordinary Number를 이용해서 원하는 함수를 호출할수도 있다
```

```cpp
typedef UINT (CALLBACK* LPFNDLLFUNC1)(DWORD,UINT);

HINSTANCE hDLL;               // Handle to DLL
LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer


hDLL = LoadLibrary("MyDLL");
if (hDLL != NULL)
{
   lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL, "DLLFunc1");
   // 함수의 이름을 통해서도 가져온다

    FreeLibrary(hDLL);
    return SOME_ERROR_CODE;
   }
   else
   {
      // call the function
      uReturnVal = lpfnDllFunc1(dwParam1, uParam2);
   }
}
```