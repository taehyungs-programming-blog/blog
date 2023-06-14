---
title: "STL : stream iterator"
permalink: cpp/stl/stream-iterator/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-07 00:00:00 -0000
last_modified_at: 2020-04-07 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Note

* [Lecture Note : ostream_iterator 써보기](https://ideone.com/v3L2rg)
* [Lecture Note : ostream_iterator 써보기 - 2](https://ideone.com/LLkexp)
* [Lecture Note : ostreambuf_iterator 써보기](https://ideone.com/LN7loU)
* [Lecture Note : cout에 대해서](https://ideone.com/rQ17e8) : 빌드안됨.
* [Lecture Note : istreambuf_iterator 써보기](https://ideone.com/TKkB9W) : 빌드안됨.

---

* 입/출력 스트림에서 요소를 읽거나 쓰기 위한 반복자.

## ostream_iterator 써보기 - 1

```cpp
#include <iostream>
#include <iterator>
#include <list>
using namespace std;

int main()
{
    int n = 10;
    cout << n << endl;

    ostream_iterator<int> p(cout, ", ");        // cout에 출력을 해달라
    *p = 20;        // cout << 20 << ", "
    *p = 30;        // cout << 30 << ", "
}
```

## ostream_iterator 써보기 - 2

```cpp
list<int> s = { 1, 2, 3 };
copy(begin(s), end(s), p);

fill_n(p, 3, 0);        // 0, 0, 0
```

---

## 파일에 쓰는 것도 가능

* 다양한 출력스트림에 출력이 가능하다 정도로 기억하자.

```cpp
#include <iostream>
#include <iterator>
#include <list>
#include <fstream>
using namespace std;

int main()
{
    ofstream f("a.txt");

    ostream_iterator<int> p(f, ", ");

    *p = 20;
    ++p;
    *p = 30;        // ((p.operator*()).operator=(30)
}
```

---

## ostreambuf_iterator 써보기

```cpp
#include <iostream>
#include <iterator>
using namespace std;

int main()
{
    // ostream_iterator<int> p(cout, ", ");
    ostreambuf_iterator<char> p(cout);      // ostreambuf_iterator : 오직 캐릭터만 출력(wchar_t:유니코드)

    *p = 65; // 'A'
}
```

---

## cout에 대해서

```cpp
#include <iostream>
#include <iterator>
using namespace std;

int main()
{
    cout << 65;     // cout는 65를 버퍼에 일단 넣는다
    // ostream이 숫자를 '6', '5'로 저장
    // straembuf 버퍼를 관리

    steambuf* buf = cout.rdbuf();       // cout의 streambuf주소를 출력
    buf->sputc(65); // A를 화면 출력 -> 이런일을 cout이 대신해준다.
}
```

---

## ostreambuf_iterator 써보기

* 문자만 쓸꺼라면 ostreambuf_iterator가 성능상 유리하다

```cpp
#include <iostream>
#include <iterator>
using namespace std;

int main()
{
    ostreambuf_iterator<char> p2(cout.rdbug());
    *p2 = 'A';      // streambuf->sputc('A');

    ostreambuf_iterator<wchar_t> p3(wcout.rdbug());
    *p3 = L'A';
```

---

## ostreambuf_iterator만들어 보기

* 중요성을 느끼지 못해서 정리 안함.

---

## istreambuf_iterator 써보기

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
    ifstream f("stream11.cpp");

    istream_iterator<int> p1(cin);

    int n = *p1;

    cout << n << endl;

}
```

```cpp
int main()
{
    char a = 0, b = 0, c = 0;

    // x \t y \t z
    cin >> a;   // x
    cin >> b;   // \t이 아니라 y가 들어간다.
    cin >> c;   // 역시 \t가 아니라 z가 들어간다.

    a = cin.rdbuf()->sgetc();       // x
    b = cin.rdbuf()->sgetc();       // x
    c = cin.rdbuf()->sgetc();       // x
    // 버퍼를 이동하지 않아 모두 같은걸 읽거한다.

    a = cin.rdbuf()->sgetc();       // x
    b = cin.rdbuf()->snextc();       // \t
    c = cin.rdbuf()->snextc();       // y

    cout << a << ", " << b << ", " << c << endl;
}
```


```cpp
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
using namespace std;

int main()
{
    ifstream f("stream11.cpp");
    istreambuf_iterator<char> p1(f), p2;        // p2는 디폴트 생성자로 생성이 되고 디폴트는 end of stream이다.
    ostream_iterator<char> p3(cout);

    *p3 = *p1;      // 파일을 입력받아 화면(cout)에 바로 출력

    copy(p1, p2, p3);       // f의 시작부터 p2(end of stream)까지 p3에 저장(cout) 해달라
    // 모든 공백등이 무시된다.
}
```

```cpp
int main()
{
    ifstream f("stream11.cpp");
    // istreambuf_iterator<char> p1(f), p2;
    istreambuf_iterator<char> p1(f), p2;
    ostream_iterator<char> p3(cout);

    *p3 = *p1;

    copy(p1, p2, p3);
    // 모든 공백등이 포함된다.
}
```