---
layout: default
title: "36. std::vector와 std::array의 std::move"
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

* [참고 사이트 🌍](https://jwvg0425.tistory.com/46) : 여기 정말 좋음

❤ `std::vector`에 `std::move`를 써보자.

```cpp
std::vector<Widget> vw1;

//vw1을 vw2로 move하는 건 상수 시간에 가능.
auto vw2 = std::move(vw1);
```

❤ 메모리 주소만 옮겨준다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/36-1.png"/>
</p>

---

❤ 이번엔 `std::array`에 `std::move`를 써보자.

```cpp
std::array<Widget, 10000> aw1;

//aw1을 aw2로 move하는 건 선형 시간이 걸린다.
//aw1의 모든 원소를 aw2로 move시켜야 함.
auto aw2 = std::move(aw1);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/cpp/36-2.png"/>
</p>

❤ 값의 복사가 일어난다.

---

🤷‍♀️ 궁금한 것은 실제로 메모리 주소만 옮기는지, 값 복사가 일어나는지 눈으로 보고싶은데

```cpp
// 생각해 볼거리

#include <iostream>
#include <vector>
#include <array>

class movetesterclass {
public:
    movetesterclass() : value(-1) {
        std::cout << "movetesterclass()" << std::endl;
    }
    movetesterclass(const movetesterclass& m) {
        std::cout << "movetesterclass(movetesterclass&)" << std::endl;
        value = m.value;
    }
    movetesterclass(movetesterclass&& m) {
        std::cout << "movetesterclass(movetesterclass&&)" << std::endl;
        value = m.value;
    }
    ~movetesterclass() {
        std::cout << "~movetesterclass()" << std::endl;
    }

    void SetValue(int v) { value = v; }

    int value{ -1 };
};

int main()
{
    std::vector<movetesterclass> vv;
    movetesterclass a;
    a.SetValue(100);
    vv.push_back(std::move(a));
    /*
        // 객체가 2개 생성되며
        movetesterclass()
        movetesterclass(movetesterclass&&)

        // 2개 생성됐기에 해제도 2개가 된다.
        ~movetesterclass()
        ~movetesterclass()

        vv안에 들어간 객체는 movetesterclass a;와는 다른 객체임을 알아야한다.
    */
}
```

```cpp
int main()
{
    std::vector<movetesterclass> vv;
    movetesterclass a;
    a.SetValue(100);
    vv.push_back(a);

    /*
        movetesterclass()
        movetesterclass(movetesterclass&)
        ~movetesterclass()
        ~movetesterclass()

        // 역시 객체가 2개 생성됨. &, && 생성자는 생성자임을 기억하자(별다른 기능을 안한다.)
    */
}
```

```cpp
int main()
{
    std::vector<movetesterclass> vv;
    movetesterclass a;
    a.SetValue(100);
    vv.push_back(std::move(a));
    movetesterclass b;
    b.SetValue(200);
    vv.push_back(b);

    /*
        결과를 예측해보자 객체 2개 생성, 객체 2개 l-value 생성?
        
        땡!
        movetesterclass()
        movetesterclass(movetesterclass&&)
        movetesterclass()

        // 갑자기 복사?? -> capacity가 부족해 다른공간에 복사가 일어남
        movetesterclass(movetesterclass&)
        movetesterclass(movetesterclass&)
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
    */
```

```cpp
int main()
{
    std::vector<movetesterclass> vv;
    vv.reserve(5);
    movetesterclass a;
    a.SetValue(100);
    vv.push_back(std::move(a));
    movetesterclass b;
    b.SetValue(200);
    vv.push_back(b);

    /*
        // 예상처럼된다.
        movetesterclass()
        movetesterclass(movetesterclass&&)
        movetesterclass()
        movetesterclass(movetesterclass&)
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()

        // 갑자기 이 말을 왜?? 아래 예시를 보면 이해됨.
    */
```

```cpp
int main()
{
    std::array<movetesterclass, 2> vv;
    /*
        // 예상했던 결과이다.
        movetesterclass()
        movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
    */
```

```cpp
int main()
{
    std::array<movetesterclass, 2> vv;
    std::array<movetesterclass, 2> vv2 = std::move(vv);

    /*
        movetesterclass()
        movetesterclass()
        // 복사가 일어난다. 우리가 원하던 결과가 아님
        movetesterclass(movetesterclass&&)
        movetesterclass(movetesterclass&&)
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
    */
}
```

```cpp
int main()
{
    std::vector<movetesterclass> vv;
    vv.reserve(5);
    movetesterclass a;
    a.SetValue(100);
    vv.push_back(std::move(a));
    movetesterclass b;
    b.SetValue(200);
    vv.push_back(b);

    std::vector<movetesterclass> vvv = std::move(vv);
    /*
        // 복사가 일어나지 않음(원하던 결과)
        
        movetesterclass()
        movetesterclass(movetesterclass&&)
        movetesterclass()
        movetesterclass(movetesterclass&)
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
    */
```

```cpp
// 마지막 참고 아래도 move가 먹는다!!

std::vector<movetesterclass> fn()
{
    std::vector<movetesterclass> vv;
    vv.reserve(5);

    movetesterclass a;
    vv.push_back(a);
    movetesterclass b;
    vv.push_back(b);

    return std::move(vv);
}

int main()
{
    std::vector<movetesterclass> vv = fn();

    /*
        movetesterclass()
        movetesterclass(movetesterclass&)
        movetesterclass()
        movetesterclass(movetesterclass&)
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
        ~movetesterclass()
    */
}
```

{% endraw %}