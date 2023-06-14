---
title: "STL string"
permalink: cpp/stl/string/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-01 00:00:00 -0000
last_modified_at: 2020-04-01 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## using std::string

```cpp
#include <iostream>

int main()
{
    char s1[10] = "hello";
    char s2[10];

    s2 = s1;        // error - strcpy(s2, s1)

    if (s1 == s2)   // error strcmp(s1, s2)
    {

    }
}
```

하지만 string을 쓰면 간단하게 해결된다.

```cpp
#include <iostream>
#include <string.h>
using namespace std;

int main()
{
    string s1 = "hello";
    string s2;

    s2 = s1;                    // ok

    if (s1 == s2)               // ok
    {

    }

    string s3 = s1 + s2;        // ok
}
```

### string to const char *

```cpp
#include <iostream>
#include <string.h>
using namespace std;

int main()
{
    string s1 = "hello";
    char s2[10];

    strcpy(s2, s1);             // error - string to const char* 변환필요
    strcpy(s2, s1.c_str());     // ok
```

### string to double

```cpp
#include <iostream>
#include <string.h>
using namespace std;

int main()
{
    // string to double
    string s3 = "3.4";
    double d = stod(s3);

    // double to string
    string s4 = to_string(5.4);
```

### 접미사를 통한 자료형 선택

```cpp
#include <iostream>
#include <string.h>
using namespace std;
using namespace std::string_literals;

void foo(string s) { cout << "string" << endl; }
void foo(const char * s) { cout << "char" << endl; }

int main()
{
    foo("hello");   // char
    foo("hello"s);  // string -> 접미사로 자료형 선택이 가능
}
```

* "string"s - string
* 0s - seconds
* 0i - complex<double>

```cpp
using namespace std::literals;

std::complex<double> c = 3i; // ok
```
