---
title: "(Qt) Dock 특정 Layout에 넣기"
permalink: qt/dockadd/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-14 00:00:00 -0000
last_modified_at: 2020-03-14 00:00:00 -0000
---

```cpp
ui->Layout->addWidget(ui->Dock->widget());
```

Dock은 widget함수를 지원 widget을 부르면 widget으로 동작하게 된다.