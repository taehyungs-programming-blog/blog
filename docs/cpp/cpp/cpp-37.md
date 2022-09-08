---
layout: default
title: "37. 문자집합에 대해서 💥"
parent: (C++)
grand_parent: C++
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

💥 쓸때마다 헷갈리는 문자집합 이제 좀 정리하자.

## Visual Studio에서 사용되는 유니코드 / 멀티바이트 문자집합

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-1.png"/>
</p>

💥 이걸 설정하면 뭐가 달라지나???

```cpp
// 유니코드 / 멀티바이트 다를것 없이 아래데로 나옴.
char c[] = "abc안녕";
wchar_t w[] = L"abc안녕";

std::cout << sizeof(c) << std::endl;
// 8 (영어 1byte / 한글 2byte / 마지막 null 1바이트)
std::cout << sizeof(w) << std::endl;
// 12 (영어/한글/null할것 없이 2byte)
```

💥 결과가 다를게 없는데 그럼 뭐가 다를까??<br>
💥 우선 전처리기를 보면(아래) `_MBCS`가 설정된 것을 알 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-2.png"/>
</p>

```cpp
// 멀티바이트
TCHAR str[] = _T("abc안녕");
std::cout << sizeof(str) << std::endl;  // 8

// 유니코드
TCHAR str[] = _T("abc안녕");
std::cout << sizeof(str) << std::endl;  // 12
```

💥 메크로의 선언으로 메크로사용시 영향을 줌을 알 수 있다.

---

## 그렇다면 유니코드 / 멀티바이트는 뭔가???



{% endraw %}