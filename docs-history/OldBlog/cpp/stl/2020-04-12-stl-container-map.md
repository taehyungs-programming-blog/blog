---
title: "STL : map"
permalink: cpp/stl/associative/map/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2020-04-12 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <string>
#include <map>
using namespace std;

int main()
{
    map<string, string> m;      // <key, data>

    pair<string, string> p1("월요일", "mon");
    m.insert(p1);

    m.insert(make_pair("화요일", "tue"));

    m["수요일"] = "wed";

    cout << m["월요일"] << endl;        // "mon"
    cout << m["토요일"] << endl;        // 새로운 pair가 생성되어 버린다.!!

    auto ret = m.find("월요일");
    if(ret == m.end())
        cout << "fail" << endl; vb
}
```

```cpp
#include <iostream>
#include <string>
#include <map>
using namespace std;

int main()
{
    map<int, string> m;

    m[1] = "ONE";
    m[0] = "ZERO";
    m[2] = "TWO";

    auto p = begin(m);

    cout << p->first << endl;       // 0
    cout << p->second << endl;      // "ZERO"
}
```

---

**Example : 파일에서 단어를 분석해보자.**

```cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
using namespace std;

int main()
{
  map<string, list<int>> index;

  ifstream f("text.txt");

  int lineno = 0;
  string s;

  while( getline(f, s) )    // 파일에서 한 줄 읽기
  {
    lineno++;
    istringstream iss(s);
    string word;
    while(iss >> word)
    {
      index[word].push_back(lineno);
    }
  }

  auto p = bein(index);

  while( p != end(index) )
  {
    cout << p->first << " : ";
    for(auto n : p->second)
      cout << n << ", ";
    cout << endl;
    ++p;
  }
}
```

이게 필요한가 싶겠지만 이걸 C로 구현해야한다면?? 말이 또 달라진다.

---