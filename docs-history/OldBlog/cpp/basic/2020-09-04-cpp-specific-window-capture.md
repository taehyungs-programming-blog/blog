---
title: "C++ : 특정 프로그램 Capture"
permalink: cpp/specific-window-capture/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-04 00:00:00 -0000
last_modified_at: 2020-09-04 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - capture
  - printwindow
  - bitblt
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트1](https://stackoverflow.com/questions/19705797/find-the-window-handle-for-a-chrome-browser)
* [참고사이트2](https://stackoverflow.com/questions/30965343/printwindow-could-not-print-google-chrome-window-chrome-widgetwin-1)
* [참고사이트3](https://forum.powerbasic.com/forum/user-to-user-discussions/powerbasic-for-windows/784149-get-dc-of-chrome-window)
* [참고사이트4](https://sonny777.tistory.com/3)

bitblt을 이용하면 가장 좋지만... <br>
요즘 개발되고 있는 cef기반의 프로그램은 hardware accelation을 지원하기에 단순히 bitblt을 이용해 capture가 불가능하다.<br>
`::printwindow`를 사용하자 자세한건 나중에 정리...

---

## Capture한 데이터 BITMAP으로 저장하기

* [참고사이트](https://stackoverflow.com/questions/55034881/printwindow-prints-with-empty-space)

```cpp
bool result = true;

HWND appHWnd = ::FindWindow(nullptr, TEXT("Melon"));

RECT appWindowRect; ::GetWindowRect(appHWnd, &appWindowRect);
HDC appDC = ::GetWindowDC(appHWnd);
//    HDC appDC = ::GetDC(appHWnd); // same issue occured either
//    HDC appDC = ::GetDC(nullptr);
HDC memoryDC = ::CreateCompatibleDC(appDC);

HBITMAP capturedScreenBitmap = ::CreateCompatibleBitmap(
    appDC,
    appWindowRect.right - appWindowRect.left,
    appWindowRect.bottom - appWindowRect.top
);

HBITMAP memoryBitmap = static_cast<HBITMAP>(::SelectObject(memoryDC, capturedScreenBitmap));

result = ::PrintWindow(appHWnd, memoryDC, 0);

//copy to clipboard
OpenClipboard(nullptr);
EmptyClipboard();
SetClipboardData(CF_BITMAP, capturedScreenBitmap);
CloseClipboard();

::SelectObject(memoryDC, memoryBitmap);
::DeleteObject(capturedScreenBitmap);
::DeleteDC(memoryDC);
::ReleaseDC(appHWnd, appDC);
```

---

## 추가

capture가 제대로 되지 않는 App들이 있는데 DPI Awareness 문제임.