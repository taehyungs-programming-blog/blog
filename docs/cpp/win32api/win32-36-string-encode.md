---
layout: default
title: "36. 이젠 마지막 이길... string encode!"
parent: (Windows API)
grand_parent: (C++)
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

> Windows C++ 개발 시 자주 혼동되는 문자열 인코딩, 문자집합 설정, std::string/wstring의 관계를 정리합니다.

---

## 유니코드/멀티바이트 문자집합?

* 유니코드 - UTF8/16/32
    * OS마다 달라지는데 WinOS기준 UTF-16
* 멀티바이트 문자집합 - CP949(한국기준)

* 이라고 보면된다.

---

## 오호라? 그럼 유니코드 설정 후 std::string을 쓰면 UTF-16을 쓰는거겠군

* 땡! `std::wstring`을 써야함
* 호엥? 그럼 `std::string`은?
    * 여전히 CP949이다.
* 엥? 그럼 유니코드 설정을 왜 하는데?
    * `TCHAR, TEXT, _T` 매크로에는 먹힌다

---

## C++17이상은 `u8`옵션으로 std::string에서 utf가 사용된다

```cpp
// C++17
std::string utf8_str = u8"안녕";
// utf8_str에는 UTF-8 바이트가 저장됨
// 바이트: 0xEC 0x95 0x88 0xEB 0x85 0x95

// C++20 (타입이 변경됨)
std::u8string utf8_str = u8"안녕";  // char8_t 기반
```

| 타입 | 인코딩 | 리터럴 prefix | 사용 예시 |
|------|--------|--------------|----------|
| `char*`, `std::string` | 지정 안 됨 (보통 시스템 CP) | `"문자열"` | `std::string str = "안녕";` |
| `wchar_t*`, `std::wstring` | UTF-16 | `L"문자열"` | `std::wstring str = L"안녕";` |
| `char8_t*`, `std::u8string` | UTF-8 (C++20) | `u8"문자열"` | `std::u8string str = u8"안녕";` |
| `TCHAR*` | 설정에 따라 변경 | `TEXT("문자열")`, `_T("문자열")` | `TCHAR str[] = TEXT("안녕");` |
