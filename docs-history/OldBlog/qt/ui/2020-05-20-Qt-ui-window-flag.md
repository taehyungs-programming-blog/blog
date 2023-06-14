---
title: "Qt : WindowFlag 지우기/설정하기"
permalink: qt/ui/windowflag/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-20 00:00:00 -0000
last_modified_at: 2020-05-20 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - UI
  - windowflags
excerpt: ""
---

```cpp
// 기존의 Flag에 추가하고 싶을때
setWindowFlag(windowFlags() | Qt::Tool);

// 기존의 Flag에서 빼고싶을때
setWindowFlag(windowFlags() ^ Qt::Tool);       

// 아에 새로 넣고 싶을때
setWindowFlag(Qt::Tool);        

// 가장중요한건 새로 show해줘야 한다는 점.
show();
```

* [참고사이트](https://forum.qt.io/topic/87289/removing-always-on-top-flag-doesn-t-work-windows)