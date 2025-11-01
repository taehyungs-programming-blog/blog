---
layout: default
title: "02. vs debug"
parent: "(Windows Debugging)"
grand_parent: "(C++)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 중단점 설정 (Break Point Condition)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-01-windbg.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-02-windbg.png"/>
</p>

* 조건과 그 조건에 대한 액션을 넣을수 있음
* 액션은 로그를 찍는 것

### 디버깅 Tips

```cpp
CMagic* pMagig = GetObject()->GetPlayer()->GetMagic();
// 이런 코드는 피하자

/*
GetObject()
GetPlayer()
GetMagic()

어디서 크래쉬인지 알길이 없다.
C#은 알수있음!
*/

CObject* object = GetObject();
CPlayer* player = object->GetPlayer();
CMagic* magic = player->GetMagic();
// 어떤 객체를 사용할지 목적성도 분명하고, 크래쉬가 난다면 포인트 찾기도 수월
```

---

## Heap profile

* Heap이 어디서 얼마나 할당되었나 확인가능(성능이슈~)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-03-windbg.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-04-windbg.png"/>
</p>

---

## CRT Memory Debug

* CRT Memory Debug는 Microsoft Visual C++ 컴파일러(cl.exe)의 기능으로, IDE 기능이 아닌 런타임 라이브러리에서 제공하는 디버깅 도구
* 메모리 누수와 힙 손상을 감지하고 추적

```cpp
_CrtMemState state1, state2, stateDiff;

// 검사 시작 지점
_CrtMemCheckpoint(&state1);

// ... 테스트할 코드 ...

// 검사 종료 지점
_CrtMemCheckpoint(&state2);

// 메모리 변화량 계산
_CrtMemDifference(&stateDiff, &state1, &state2);

// 결과 출력
_CrtMemDumpStatistics(&stateDiff);

// 프로그램 종료 시 자동 누수 체크
_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
```

```cpp
// 매크로
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
```

### Example

```cpp
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <iostream>
using namespace std;

int main()
{
    // 프로그램 종료 시 누수 자동 검사
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

    // 메모리 누수 발생
    int* pMemory = new int[100];
    // pMemory를 해제하지 않음

    // 힙 손상 확인
    if (!_CrtCheckMemory())
    {
        cout << "Heap corruption detected!" << endl;
    }

    return 0;
    // 프로그램 종료 시 누수 보고서 출력
}
```

* VS에서 더블클릭시 해당 코드라인으로 감.

```
Detected memory leaks!
Dumping objects ->
{1234} normal block at 0x00A2F448, 100 bytes long.
 Data: <test            > 74 65 73 74 20 20 20 20 20 20 20 20 20 20 20 20
Object dump complete.
```

---

## Remote Debugging

* 디버깅하고자 하는 장비(QA팀?) 완전한 개발환경 구축이 불가능할 경우 사용해 보자
* 최소한의 개발환경만 주입한다고 생각하면 된다

* [준비물](https://visualstudio.microsoft.com/ko/downloads/?cid=learn-onpage-download-cta)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-05-windbg.png"/>
</p>

* 설치과정이 있는데 거의 Next나 Ok를 누르면 됨 (별도세팅이 필요없음)

* 빌드할 로컬머신에선, 
    * 빌드한 .exe혹은 .dll을 전달
    * 프로젝트 -> 속성 -> 디버깅 -> 시작할 디버거 -> 원격 Windows 디버거 로 수정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/windbg/01-02-06-windbg.png"/>
</p>

* 원격 명령을 원격 PC의 .exe위치를 지정한다 (exe까지 포함되어야 함.)
* 원격 서버 이름을 원격 PC의 이름을 지정 (`ipconfig /all`)

```
Windows IP 구성

   호스트 이름 . . . . . . . . : DESKTOP-L605CQ3
```

