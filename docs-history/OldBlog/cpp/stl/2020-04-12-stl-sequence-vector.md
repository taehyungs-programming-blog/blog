---
title: "STL : sequence container1 - vector"
permalink: cpp/stl/sequence/vector/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2021-03-05 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
tag:
  - C++
category:
  - vector
excerpt: ""
classes: wide
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## iterator 활용

```cpp
#include <iostream>
#include <vector>

using namespace std;

class MyClass
{
public:
	int a = 0;
	int b = 0;
};

int main()
{
	vector<MyClass*> vec;

	MyClass* mc = new MyClass;
	vec.push_back(mc);

	MyClass* mc2 = new MyClass;
	vec.push_back(mc2);

	auto it = vec.begin();

	int aa = (*it)->a;
	int bb = (*it)->b;
}
```

---

## 초기화

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> v1;
    vector<int> v2(10);
    vector<int> v3(10, 3);
    vector<int> v4(v3);

    vector<int> v5 = {1,2,3,4};
    vector<int> v6{1,2,3,4};

    vector<int> v7(10,0);       // 10개를 0으로 초기화
    vector<int> v8{10,0};       // 2개를 10, 0으로 초기화

    /* *************************** */

    // v.push_front(10);       // error
    v.push_back(10);
    v.insert(begin(v)+1, 30);   // insert(위치, 값)

    /* *************************** */

    int n = v.front();
    int n1 = v[0];

    /* *************************** */

    int x[5] = {1,2,3,4,5};
    v.assing(x, x+5);

    /* *************************** */

    v[100] = 10;        // 예외 없이 runtime error
    v.at(100) = 10;     // error!

    for(int i = 0; i < v.size(); i++)
        v[i] = 10;      // ok
        v.at(i) = 10;   // 성능이 나쁘다.
}
```

---

## 맴버삽입

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> v(10, 0);
    v.resize(7);        // 메모리를 재할당할 것인가? or 메모리 사용량을 줄일 것인가?
    // 메모리 사용량을 줄이게 된다.

    cout << v.size() << endl;           // 7
    cout << v.capacity() << endl;       // 10

    v.push_back();

    cout << v.size() << endl;           // 8
    cout << v.capacity() << endl;       // 10

    v.pop_back()

    cout << v.size() << endl;           // 7
    cout << v.capacity() << endl;       // 10

    v.shrink_to_fit();
    cout << v.size() << endl;           // 7
    cout << v.capacity() << endl;       // 7

    v.push_back(10);
    cout << v.size() << endl;           // 8
    cout << v.capacity() << endl;       // 10 ~ 11 : 1.5배로 할당하게 된다.
}
```

---

## 초기화2

* vector와 c스타일 호환

```cpp
#include <iostream>
#include <vector>
using namespace std;

void print(int* arr, int sz)
{
    for(int i = 0; i < sz; i++)
        cout << arr[i] << endl;
}

int main()
{
    int arr[10] = {1,2,3,4,5,6,7,8,9,10};
    print(arr, 10);

    vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    // print(v,v.size());        // error!
    print(v.data(), v.size());   // 메모리 주소값을 넘기게 된다.
}
```

---

## Example

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

struct FindChar
{
    string data;
    FindChar(string s) : data(s) {}

    bool operator()(char c) const
    {
        auto p = find(begin(data), end(data), c);
        return p != end(data);
    }
};

// FindChar fc("0123456789");

int main()
{
    ifstream f("vector4.cpp");
    string s;
    f >> s;         // 단어를 받는다.
    getline(f, s);  // 한 문장을 받는다.
    while(getline(f,s))
        cout << s << endl;      // 이런식으로 많이 받음.

    cout << s << endl;      // #include 출력

    while(getline(f,s))
        v.push_back(s);

    reverse(begin(v), end(v));      // 파일의 내용을 뒤집는다.
    for(int i = 0; i < v.size(); i++)
        reverse(begin(v[0], end(v[0])));// 각 줄을 뒤집는다.
        replace(begin(v[i]], end(v[i]), 'i', ' ')); // i를 모두 공백을 변경한다.
        replace_if(begin(v[i]], end(v[i]), [](char c){return c == 'a';}], ' '));
        FindChar fc("aieouAIEOU");
        replace_if(begin(v[i]), end(v[i]), fc, ' ');

    for(auto str : v)
        cout << str << endl;    // vector의 모든 내용 출력
}
```

```cpp
// 아래와 같은 모양으로 헤쉬테이블을 만들수 있음.
vector<list<int>> h(10);
h[0].push_back(10);
h[0].push_back(20);
```

---

## foreach 활용

* [참고사이트](https://hycszero.tistory.com/81)

```cpp
// 과거 스타일 for문
for (int i = 0; i < 10; i++)
{
    std::cout << i << std::endl;
}

// C++11 스타일 for문
for(int i : {0, 1, 2, 3, 4, 5})
{
    std::cout << i << std::endl;
}

std::vector<std::string> name_vector;       // 아마 보통이 방법을 많이 쓸 듯!
for(const auto& element : name_vector)
{
    std::cout << element << std::endl;
}

// std::for_each 활용
std::vector<std::string> name_vector{ "test1", "test2", "test3" };
std::for_each(name_vector.begin(), name_vector.end(), [](auto& input) {std::cout << input << std::endl; });

void print(std::string& input)
{
    std::cout << input << std::endl;
}
std::vector<std::string> name_vector{ "test1", "test2", "test3" };
std::for_each(name_vector.begin(), name_vector.end(), print);
```