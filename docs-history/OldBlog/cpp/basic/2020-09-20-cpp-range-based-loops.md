---
title: "(C++) Range-Based Loops"
permalink: cpp/range-based-loops/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-20 00:00:00 -0000
last_modified_at: 2020-09-20 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - Range-Based Loops
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
char texts[] = {"one", "two", "three"};
// auto를 써도 상관은 없다

for(auto text : texts) {
    // texts의 char가 하나씩 불러와진다
    cout << text << endl;
}

```cpp
vector<int> numbers;
numbers.push_back(5);
numbers.push_back(7);
numbers.push_back(9);
numbers.push_back(10);

// vector도 처리가 가능
for(auto number : numbers) {
    cout << number << endl;
}
```

```cpp
string hello = "Hello";

for(auto c : hello) {
    // char가 하나씩 출력됨.
    cout << c << endl;
}
```