---
layout: default
title: "01. 디버그 환경 설정"
parent: "(Windows Debugging)"
grand_parent: "(C++)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Windows API, Symbol cache store 지정하기

* 시스템 환경변수 `_NT_SYMBOL_PATH` : `srv*c:\{원하는위치}*http://msdl.microsoft.com/download/symbols`
* 재시작
* 이렇게 하면 Windows API의 Symbol을 다운로드한다.
* 이제 VS에서 Windows API의 Symbol을 읽을수 있다!

### 혹시 VS에서 안보인다면?

* VS -> 도구 -> 옵션
* 디버깅 -> 기호 에서 내가 설졍한 환경변수가 활성화 되어있는지 확인.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-01-windbg.png"/>
</p>

---

## AeDebug 설정

```cpp
int* p = nullptr;
p = 100;
```

* 이런코드가 있다고 할때, 빌드 후 실행시 크래쉬가 나지만 vs와 같은도구로 확인할수 없다.
* 이걸 vs에 바로 연결해 보고싶다면? -> AeDebug를 사용해 보자.
* 근데 이걸 어디쓰나? -> **멀티프로세스 디버깅에 아주 유용하다**

```cpp
__debugbreak();
```

* 를 걸면 AeDebug에 걸리게 된다.

### 자! 그럼 AeDebug를 어떻게 설정하나

* 레지스트리 편집기 -> `컴퓨터\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug`
* Debugger(REG_SZ) : `"C:\Windows\system32\vsjitdebugger.exe" -p %ld -e %ld -j 0x%p`
* Auto (RED_DWORD) : 1

