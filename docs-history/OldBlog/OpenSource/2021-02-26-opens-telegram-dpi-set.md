---
title: "(OpenSource - Telegram Qt UI) DPI Setting"
permalink: opens/telegram/dpiset/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-26 00:00:00 -0000
last_modified_at: 2021-02-26 00:00:00 -0000
tag:
  - OpenSource
  - Telegram
  - Qt
  - UI
category:
  - DPI
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

telegram은 *.style 파일에서 파싱한 pixel을 기반으로 dpi를 받아서 그린다.

## dpi를 구하는 방법

```cpp
void Sandbox::setupScreenScale() {
	const auto dpi = Sandbox::primaryScreen()->logicalDotsPerInch();
    // static QScreen *primaryScreen(); -> qapplication 함수
    // logicalDotsPerInch() -> qscreen 함수

	// ...
```

## 추가(글로벌 세팅값 세팅하기)

```cpp
cSetScreenScale(300); // 300%: 288 DPI (264-inf)
// 이런식으로 세팅값을 저장하는데 ... 좀 더 자세히 보자면
```

```cpp
// setting.h

// ...
DeclareSetting(int, ScreenScale);
// ...

// 이런식으로 세팅값을 저장하는데
```

```cpp
// setting.h

#define DeclareSetting(Type, Name) DeclareReadSetting(Type, Name) \
inline void cSet##Name(const Type &Name) { \
	g##Name = Name; \
}
```

```cpp
// setting.h

#define DeclareReadSetting(Type, Name) extern Type g##Name; \
inline const Type &c##Name() { \
	return g##Name; \
}
```

```cpp
// gScreenScale를 불러올수 있다
int gScreenScale = style::kScaleAuto;
```