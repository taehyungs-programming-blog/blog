---
layout: default
title: "32. std::set"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

## std::set

🤡 핵심만 말하자면 **이진트리(자동정렬)**, **중복없는 키** 인 컨테이너

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
	std::set<int> _set;
	for(int i = 0; i < 20; i++)
	{
		_set.insert(i%10);
	}
	for(auto& item : _set)
	{
		std::cout << item << std::endl;
	}
	return 0;
}
```

```
0
1
2
3
4
5
6
7
8
9
```

```cpp
#include <iostream>
#include <set>
using namespace std;

int main() {
	std::set<pair<int, int>> _set;
	for(int i = 0; i < 20; i++)
	{
		_set.insert(make_pair(i%10, i));
	}
	for(auto& item : _set)
	{
		std::cout << item.first << std::endl;
	}
	return 0;
}
```

```
0
0
1
1
2
2
3
3
4
4
5
5
6
6
7
7
8
8
9
9
```

🤡 `pair`가 하나의 key가 됨을 알 수 있다.

---

## std::unordered_set

```cpp
#include <iostream>
#include <unordered_set>
using namespace std;

int main() {
	std::unordered_set<int> _set;
	for (int i = 20; i > 0; i--)
	{
		_set.insert(i % 10);
	}

	for (auto& item : _set)
	{
		std::cout << item << std::endl;
	}
	return 0;
}
```

```
8
0
9
7
6
5
4
3
2
1
```

🤡 예상대로 정렬은 안된다.

---

## std::unordered_set + std::pair / std::tuple 활용기

* [참고 사이트 🌍](https://www.techiedelight.com/ko/use-pair-key-std-unordered_set-cpp/)

🤡 `std::unordered_set`은 아래와 같은 사용이 불가능하다

```cpp
std::unordered_set<std::pair<int, int>> _set;   // error
```

🤡 뭘 기준으로 key값을 정해야하는지 알려줘야한다.

```cpp

#include <iostream>
#include <unordered_set>
#include <utility>
 
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (std::pair<T1, T2> const &pair) const
    {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
 
        return h1 ^ h2;
    }
};
 
int main()
{
    std::unordered_set<std::pair<std::string, int>, pair_hash> set = {
        {"two", 2}, {"one", 1}, {"four", 4}, {"three", 3}
    };
 
    for (auto const &p: set) {
        std::cout << p.first << ": " << p.second << std::endl;
    }
 
    return 0;
}
```

{% endraw %}