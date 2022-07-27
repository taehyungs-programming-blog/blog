---
layout: default
title: "QString은 어떻게 인코딩되어 있나?"
parent: (QT)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://winplz.tistory.com/entry/QTQString%EA%B4%80%EB%A0%A8-%EC%A0%95%EB%A6%AC)

## `.toStdString()`은 어떻게 인코딩 할까?

💩 정답부터 말하자면 **UTF-8**이다.<br>
💩 **주의사항** - `toStdString()`로 모든언어가 UTF-8로 정상적 인코딩이 되진않으니 언어별로 확인이 꼭 필요하다(안되면 `.toUtf8()` 사용해서 Bytes로 받아야함.)<br>
💩 자세한 테스트는 아래를 참고<br>

```cpp
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 방법 1.
    QString qstr = "test";
    std::string str = qstr.toStdString();
    //const char* c_str = qstr.toStdString().c_str();

    // 방법 2.
    QByteArray bArray = qstr.toUtf8();
    char* c_str_utf8 = bArray.data();

    return a.exec();
}
```

* [참고사이트(UTF-8 Encode/Decode) 🌎](https://mothereff.in/utf-8)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-2.png"/>
</p>

💩 영어 기준 모두 utf-8로 인코딩 된걸 볼 수 있다<br>

```cpp
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 방법 1.
    QString qstr = QString::fromLocal8Bit("한글");
    std::string str = qstr.toStdString();

    // 방법 2.
    QByteArray bArray = qstr.toUtf8();
    char* c_str_utf8 = bArray.data();

    return a.exec();
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-4.png"/>
</p>

💩 역시 utf-8로 인코딩이 잘 된다.<br>
💩 **(추가)** - `QString::fromLocal8Bit("한글");` fromLocal8Bit는 왜하나?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-5.png"/>
</p>

💩 윈도우 기준 텍스트 파일생성시 기본으로 **ANSI**를 적용받게 된다.

* [참고사이트 🌎](https://taehyungs-programming-blog.github.io/blog/docs/cpp/win32api/2022-01-13-win32-7/#%EC%B6%94%EA%B0%80-%EB%AC%B8%EC%9E%90%EC%97%B4-%EA%B4%80%EB%A0%A8-%ED%95%AD%EC%83%81-%ED%97%B7%EA%B0%88%EB%A6%AC%EB%8A%94-%EC%8B%9C%EB%A6%AC%EC%A6%88)

* **ANSI(American National Standard Institute)**
    * 8비트로 언어를 표현
    * 정확히 구조는 `[ ASCII(7bits) + CodePage(1bit) ]` : 영어이외의 다른 언어를 표현하고 싶어 탄생
    * **단점**은 CodePage정보(어느국가에서 사용하는지)를 미리 알고있어야 한다.(단순 CodePage만으로 어느국가인지 표현이 불가능)
    * 예를 들어 일본 윈도우에서 작성된 ANSI인코딩의 파일을 한글 윈도우에서 열면 ANSI로 열어도 다 깨져서 나오게 된다.

💩 QT의 경우 UTF-16을 기반으로 텍스트를 읽기에 **Local8Bit(ANSI)**라는 것을 알려줘야 한다.<br>

---

## `.toLocal8Bit()` Vs `.toUtf8()` 비교

```cpp
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString qstr = QString::fromLocal8Bit("한글");
    std::string str = qstr.toStdString();

    QByteArray bArray_utf8 = qstr.toUtf8();
    char* c_str_utf8 = bArray_utf8.data();

    QByteArray bArray_local8 = qstr.toLocal8Bit();
    char* c_str_local8 = bArray_local8.data();

    return a.exec();
}
```

* [참고사이트 (text encoder)🌎](https://www.rapidtables.com/convert/number/hex-to-ascii.html)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qstring-7.png"/>
</p>

---

* **toAscii()** - returns an 8-bit string encoded using the codec specified by QTextCodec::codecForCStrings (by default, that is Latin 1).
* **toLatin1()** - returns a Latin-1 (ISO 8859-1) encoded 8-bit string.
* **toUtf8()** - returns a UTF-8 encoded 8-bit string. UTF-8 is a superset of US-ASCII (ANSI X3.4-1986) that supports the entire Unicode character set through multibyte sequences.
* **toLocal8Bit()** - returns an 8-bit string using the system's local encoding.

