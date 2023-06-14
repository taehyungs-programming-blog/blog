---
title: "(C++) foreach"
permalink: cpp/foreach/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-06 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - foreach
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
// C++ 03
for(vector<int>::iterator itr = v.begin(); itr != v.end(); ++itr)
    cout << (*itr);

// C++ 11
for(int i: v)
    cout << i;
```