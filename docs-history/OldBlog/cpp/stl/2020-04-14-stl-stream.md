---
title: "STL : (추가) C++ 표준 stream"
permalink: cppl/stream/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-14 00:00:00 -0000
last_modified_at: 2020-04-14 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/5hbOMy)

```cpp
#include <iostream>
#include <fstream>
#include <sstream>    // string 입출
#include <string>
using namespace std;

int main()
{
  string s;
  cin >> s;         // 표준 입력에서 한 단어 입력

  ifstream fin("stream.cpp");
  fin >> s;         // 파일에서 한 단어 입력

  istringstream iss("I am a boy");
  iss >> s;         // I

  while(iss >> s)   // 끝을 만날때까지 계속 출력
    cout << s << endl;
}
```

* [Run This Code](https://ideone.com/Ot0Adc)

```cpp
#include <iostream>
#include <fstream>
#include <sstream>    // string 입출
#include <string>
using namespace std;

int main()
{
    cout << "hello";        // 화면 출력

    ofstream f("a.txt")     // 파일 출력
    f << "hello";

    ostringstream oss;
    oss << "hello";         // oss 버퍼에 출력

    string s = oss.str();
    cout << s << endl;
}
```

* [Run This Code](https://ideone.com/HsOPoU)

```cpp
int n = 10;

ostringstream oss;

oss << " n = " << n;

string s = oss.str();

cout << s << endl;
```


