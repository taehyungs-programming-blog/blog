---
title: "(Qt) Menubar 위치 조정"
permalink: qt/menubargeo/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-12 00:00:00 -0000
last_modified_at: 2020-03-14 00:00:00 -0000
---

> * [Github](https://github.com/8bitscoding/VS_Frameless_Widget)

```cpp
Frameless::Frameless(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.newTitlebarLayout->setMenuBar(ui.menuBar);       // 상당히 쉽다
}
```

Layout은 setMenuBar함수를 지원하기에 간단하게 SetMenuBar함수를 호출하면 된다.