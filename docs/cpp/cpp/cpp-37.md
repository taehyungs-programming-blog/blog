---
layout: default
title: "37. 문자집합에 대해서 💥"
parent: (C++)
grand_parent: (C++)
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고 페이지 🌎](https://taehyungs-programming-blog.github.io/blog/docs/cpp/win32api/2022-01-13-win32-7/#unicode%EC%97%90-%EA%B4%80%ED%95%98%EC%97%AC)

💥 쓸때마다 헷갈리는 문자집합 이제 좀 정리하자.

## Visual Studio에서 사용되는 유니코드 / 멀티바이트 문자집합

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-1.png"/>
</p>

💥 이걸 설정하면 뭐가 달라지나???

```cpp
// 유니코드 / 멀티바이트(ANSI) 다를것 없이 아래데로 나옴.
char c[] = "abc안녕";
wchar_t w[] = L"abc안녕";

std::cout << sizeof(c) << std::endl;
// 8 (영어 1byte / 한글 2byte / 마지막 null 1바이트)
std::cout << sizeof(w) << std::endl;
// 12 (영어 / 한글 / null할 것 없이 2byte)
```

💥 결과가 다를게 없는데 그럼 뭐가 다를까??<br>
💥 우선 전처리기를 보면(아래) `_MBCS`가 설정된 것을 알 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-2.png"/>
</p>

```cpp
// 멀티바이트(ANSI)
TCHAR str[] = _T("abc안녕");
std::cout << sizeof(str) << std::endl;  // 8

// 유니코드
TCHAR str[] = _T("abc안녕");
std::cout << sizeof(str) << std::endl;  // 12
```

💥 매크로의 선언으로 메크로사용시 영향을 줌을 알 수 있다.

---

## 헷갈리는 용어 총 정리

* **ASCII(American Standard Code for Information Interchange)**
    * 언어를 **7bits로 표현하는 인코딩** 방법이다.
    * 2^7 = 128, 7bits로는 128개의 문자만 표현이 가능하기에 **세상의 모든 언어를 담지는 못한다.** (해결책은 없을까 -> ANSI의 탄생)
* **ANSI**
    * `[ ASCII(7bits) + CodePage(1bit) ]` 8bits로 표현하는 인코딩 방법이다.
    * CodePage로 어느정도(몇몇국가) 언어를 표기가능하긴하나 치명적 단점이 있다.
    * (단점1) 디코딩을 위해서 어느국가의 **CodePage인지 미리 알아야** 한다.
        * 예를 들어 일본 윈도우에서 작성된 ANSI인코딩의 파일을 한글 윈도우에서 열면 ANSI로 열어도 다 깨져서 나오게 된다.
    * (단점2) 언어말고 이모티콘등 새로운 **표현의 추가**가 힘들다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-3.png"/>
</p>

* **Unicode**
    * ANSI의 단점을 보완하기 위해 나왔다. ANSI는 치명적으로 **CodePage를 알아야한다는 단점이 있다.**
    * Unicode의 SubSet으로 UTF8, UTF16 ... 이 존재하며 어떤 인코딩방법으로 인코딩 되었는지는 알아야한다.*(단점일 수도??)*
    * **Q.** 그럼 그냥 UTF8만 사용하면 되지 않나??
    * **A.** 한글의 경우 UTF8로 표현시 3bytes이다. UTF16으로 표현시 2bytes인데 글자당 1byte의 손해를 보는 것!
    * Win32에선 `L""`표 표기 `wchar`로 받을시 유니코드임을 명시적으로 표현한다.
* **EUC-KR**
    * EUC-KR은 한글 지원을 위해 유닉스 계열에서 나온 완성형 코드 조합이다.
    * EUC-KR은 ANSI를 한국에서 확장한 것으로 외국에서는 지원이 안 될 가능성이 높다.
        * **완성형 코드**란 완성 된 문자 하나하나마다 코드 번호를 부여한 것이다.
        * 반대되는 개념으로 **조합형 코드**가 있는데, 이는 한글의 자음과 모음 각각에 코드 번호를 부여한 후 초성, 중성, 종성을 조합하여 하나의 문자를 나타내는 방식을 말한다.
* **CP949(Code Page 949)**
    * EUC-KR은 2bytes의 완성형 코드로 2bytes 내에서는 표현할 수 있는 완성된 문자의 수는 한계가 있었다.
    * CP949는 한글 지원을 위해 윈도우즈 계열에서 나온 확장 완성형 코드 조합이다. 그래서 마이크로소프트에서 EUC-KR을 개선, 확장하여 만든 것이 CP949 이다. 여기서 949는 페이지 번호를 의미하며 한국을 의미한다. (참고로 일본어는 CP932, 중국어 간체는 CP936이다.)
    * 기본적으로 EUC-KR과 호환이 되며, EUC-KR에서 표현이 되지 않는 문자는 조합을 하여 표현한다.
    * 마이크로소프트가 만들었다고 하여 MS949라고 부르기도 한다.

---

💥 그렇다면 VisualStudio C++에서는 문자열이 어떻게 저장되어 있을까?

```cpp
int main()
{
    // c, w를 브레이크 포인트 찍어보자.
    char c[] = "abc안녕";       // 멀티바이트(ANSI)
    wchar_t w[] = L"abc안녕";   // 유니코드
    return 0;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-5.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/37-6.png"/>
</p>

💥 `char`의 경우 **멀티바이트(ANSI, 정확히는 EUC-KR)**로 저장되고<br>
💥 `wchar_t`는 **유니코드 정확히는 UTF-16**으로 저장된다.

* [참고 UTF8 Decoder🌎](https://mothereff.in/)
* [참고 UTF16 Decoder🌎](https://convertcodes.com/utf16-encode-decode-convert-string/)

* **정리** 
    * `char`은 윈도우에서 사용하는 언어 인코딩 방법인 ANSI로 데이터를 저장.
    * `wchar_t`는 유니코드(UTF-16)으로 데이터를 저장한다.

{% endraw %}