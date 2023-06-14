---
title: "(Win32, MFC) DC에 연결된 BITMAP 정보 처리"
permalink: cpp/win32/dc-bitmap/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-24 00:00:00 -0000
last_modified_at: 2020-09-24 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - win32
category:
  - dc
  - bitmap
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://m.blog.naver.com/PostView.nhn?blogId=tipsware&logNo=220983425334&proxyReferer=https:%2F%2Fwww.google.com%2F)

---

## DC에 연결된 비트맵의 핸들 받기

```cpp
HDC h_dc = ::GetDC(m_hWnd);  // DC를 얻는다. m_hWnd는 윈도우의 핸들 값
// DC에 연결된 GDI Object 중에서 비트맵 객체의 핸들 값을 얻는다.
HGDIOBJ h_bitmap = ::GetCurrentObject(h_dc, OBJ_BITMAP);

// 요렇게 써도 된다
HBITMAP h_bitmap = (HBITMAP)::GetCurrentObject(h_dc, OBJ_BITMAP);
```

## 비트맵 핸들에서 비트맵 정보얻기

```cpp
HDC h_dc = ::GetDC(m_hWnd);
HGDIOBJ h_bitmap = ::GetCurrentObject(h_dc, OBJ_BITMAP);
BITMAP bmp_info;
::GetObject(h_bitmap, sizeof(BITMAP), &bmp_info);

// ... 작업 생략 ...

// h_bitmap은 단순히 참조하기 위해 핸들 값만 받은 것이기 때문에 해제할 필요가 없습니다.
::ReleaseDC(m_hWnd, h_dc);  // 사용하던 DC를 반환한다.
```