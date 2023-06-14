---
title: "(C++) 가변인자(Variadic Template)"
date: 2020-01-22 00:00:00 -0000
---

> 사실 그리 중요하지 않음 ...

> 가변 인자? 가변 매개변수와 같은 말인데 매개변수의 개수를 가변적으로 조정가능<br>
> 대표적 예는 `printf`

```cpp
int main() {
    const char* champion = "Lulu";

    printf("My most champion is %s", champion);
    // 매개변수(인자)가 2개

    printf("%d, %d, %s", 10, 20, champion);
    // 흐잉?? 몇개 까지 늘어나는겨??
}
```

---

> 기본적 구조는 다음과 같다.

```cpp
tempate<typename... values> class tuple;
```

> 간단한 예제를 살펴보자

```cpp
#include <iostream>
using namespace std;

class dog {
public:
  void speak() const { cout << "bow wow" << endl; }
};
class cat {
public:
  void speak() const { cout << "meow" << endl; }
};

template<typename T>
void speak(const T& p)
{
  p.speak();
}

template<typename T, typename... ARGS>
void speak(const T& p, ARGS... args)
{
  p.speak();
  speak(args...);
  // speak로 매개변수를 넘긴다.
}

int main()
{
  dog a, b, c, d;
  cat e, f, g, h;
  speak(a, b, c, c, d, e, f, g);
}
```

> 참고로 매개변수를 순서대로 넣게 만들고 싶다면

```cpp
template <class Dog, class Cat>
void speak(const Dog& d, const Cat& c)
{
    d.speak();
}

template <class Dog, class Cat, class ... ARGS>
void speak(const Dog& d, const Cat& c, ARGS ... args)
{
    d.speak();
    speak(args...);
}

int main()
{
    dog a, b, c, d;
    cat e, f, g, h;
    speak(a, e, b, f, c, g, d, h);
    //speak(e, b, f, c, g, d, h); //Compile Error!
    return 0;
}
```

> 이외에 다양한 표현이 있음… 참고사이트 볼 것!

---

* [참고사이트(대표)](https://bunhere.tistory.com/408)
* [참고사이트2](http://en.wikipedia.org/wiki/Variadic_templates)
* [참고사이트3](http://en.cppreference.com/w/cpp/language/parameter_pack)
* [참고사이트4](http://pic.dhe.ibm.com/infocenter/lnxpcomp/v111v131/index.jsp?topic=%2Fcom.ibm.xlcpp111.linux.doc%2Flanguage_ref%2Fvariadic_templates.html)