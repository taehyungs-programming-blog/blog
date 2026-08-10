---
layout: default
title: "34. std::initializer_list"
parent: "(C++) 문법"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고 사이트 🌍](http://egloos.zum.com/sweeper/v/3058167)

👾 C++11부터는 아래와 같은 초기화가 가능해 졌다.

```cpp
struct Object
{
    float first;
    int second;
};
 
// 구조체 하나 초기화
Object scalar = { 0.43f, 10 };
 
// 중첩 브레이스를 이용한 구조체 배열 초기화
Object anArray[] = { { 13.4f, 3 }, { 43.28f, 29 }, { 5.934f, 17 } };
```

👾 흠... 엄청 좋구만 그런데 아래와 같은 struct의 초기화는??

```cpp
template <class T>
struct Sample
{
    // ????
 
    vector<T> v;
};

// 이렇게 초기화 하고 싶다면?
Sample<int> s = { 1, 2, 3, 4, 5 };
```

👾 그래서 등장했다. `std::initializer_list`

```cpp
template <class T>
struct Sample
{
    Sample(initializer_list<T> l) : v(l)
    {
        cout << "constructed with a " << l.size() << "-element list\n";
    }
 
    void append(initializer_list<T> l)
    {
        // initializer_list는 begin()/end() 메써드를 제공한다
        v.insert(v.end(), l.begin(), l.end());
    }
 
    pair<const T*, std::size_t> c_arr() const
    {
        // 반환값으로...
        return { &v[0], v.size() };
    }
 
    vector<T> v;
};
```

```cpp
int _tmain(int argc, _TCHAR* argv[])
{
    // intializer-list를 인자로 가지는 생성자 함수 호출
    Sample<int> s = { 1, 2, 3, 4, 5 };
    // append 함수의 인자로써의 initializer_list
    s.append({ 6, 7, 8 });
    cout << "The vector size is now " << s.c_arr().second << " ints:\n";
 
    // ...
```

{% endraw %}