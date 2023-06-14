---
title: "(c++20) not_null"
permalink: cpp/not_null/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-26 00:00:00 -0000
last_modified_at: 2021-01-26 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - not_null
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://blog.seulgi.kim/2018/02/cppcoreguidelines-not-null.html)

Null pointer dereferencing은 C++을 사용하다 보면 자주 발생하는 문제다. <br>
값이 없을 수 있는 객체를 지칭할 때 포인터를 사용하고 값이 없는 상태를 null로 표현하는 C++에서 이를 근본적으로 회피할 방법은 없다.  <br>
따라서 null일 수 있는 포인터는 사용하기 전에 항상 체크하고 사용해야 한다.

```cpp
Animation::Animation(
	not_null<Player*> player,
	const QByteArray &content,
	const FrameRequest &request,
	Quality quality,
	const ColorReplacements *replacements)
: _player(player) {
    // ...
```