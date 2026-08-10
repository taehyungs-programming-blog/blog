---
layout: default
title: "35. std::forward"
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

* [참고 사이트 🌍](https://ansohxxn.github.io/cpp/chapter19-7/)

🐯 아래의 결과를 예측해 보자.

```cpp
#include <iostream>	
#include <vector>
#include <utility>

using namespace std;

struct myStruct
{};

void func(struct myStruct& s) { cout << "Pass by L-ref\n"; }  // L-value Reference 오버로딩
void func(struct myStruct&& s) { cout << "Pass by R-ref\n"; }  // R-value Reference 오버로딩

template<typename T>   // 여러가지 타입을 T로 받을 수 있다.
void func_wrapper(T t)
{
    func(t);
}

int main()
{
	myStruct s;

	func(s);			// 적당한 것을 IDE가 잘 찾아서 연결해준다.
	func(myStruct());

    cout << endl;

	func_wrapper(s);
	func_wrapper(myStruct());
}
```

```
💎출력💎

Pass by L-ref
Pass by R-ref

Pass by L-ref
Pass by L-ref
```

🐯 우리가 원하는건 `template`으로 wrapper를 씌우더라도 r, l value에 맞게 오버로딩되는 것이다.

```cpp
// 해결법
template<typename T> 
void func_wrapper(T && t)
{
	func(std::forward<T>(t));
}
```

🐯 정확히 말하면 `std::forward`는 타입캐스팅만을 해준다. `static_cast<T&&>` r, l 로 캐스팅이 되는지 체크만 해준다는 말!

* [사용 예시) perfect forwarding](https://taehyungs-programming-blog.github.io/blog/docs/cpp/cpp/2022-02-17-cpp-19/) - r, l에 상관없이 완벽하게 보내고 싶다!

{% endraw %}