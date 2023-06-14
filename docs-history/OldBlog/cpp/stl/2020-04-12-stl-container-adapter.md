---
title: "STL : Container Adapter"
permalink: cpp/stl/container-adapter/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2020-04-12 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## adapter 패턴을기반으로한 list를 기반으로 stack을 만들어보자  

```cpp
#include <iostream>
using namespace std;

#include <list>
// list를 기반으로 stack을 만들어보자.
template<typename T> class Stack
{
    list<T> st;
public:
    inline void push(const T& a) { st.push_back(a); }
    inline void pop() { st.pop_back(a); }
    inline T& top() { return st.back(); }
    inline bool empty() { return st.empty(); }
};

int main()
{
    Stack<int> s;
    s.push(10);
    s.push(20);

    cout << s.top() << endl;        // 20
    s.pop();

    cout << s.top() << endl;        // 10
}
```

기능 추가

```cpp
#include <iostream>
using namespace std;

#include <list>
// list를 기반으로 stack을 만들어보자.
template<typename T, typename C = deque<T>> class Stack
{
    C st;
public:
    inline void push(const T& a) { st.push_back(a); }
    inline void pop() { st.pop_back(a); }
    inline T& top() { return st.back(); }
    inline bool empty() { return st.empty(); }
};

int main()
{
    Stack<int, vector<int>> s;
    s.push(10);
    s.push(20);

    cout << s.top() << endl;        // 20
    s.pop();

    cout << s.top() << endl;        // 10
}
```

물론 위 기능을 `#include <stack>`에서 제공한다.

---

```cpp
#include <iostream>
#include <list>
#include <vector>
#include <deque>
#include <stack>
#include <queue>
using namespace std;

int main()
{
    stack<int, vector<int>> s;

    queue<int> q;
    queue<int, vector<int>> q2; // error - queue는 양쪽이 모두 삽입/삭제가 가능해야한다.
    queue<int, list<int>> q3;   // ok
    priority_queue<int> pq;
}
```

---

## priority_queue

```cpp
#include <iostream>
#include <queue>
using namespace std;

int main()
{
    priority_queue<int> pq;     // 집어 넣은 순서와 상관없이 우선순위로 출력
    // 내부적 우선 순위는 '<' 로 분석한다.

    pq.push(10);
    pq.push(20);
    pq.push(15);
    pq.push(12);

    cout << pq.top() << endl;   // 20
    pq.pop();
    cout << pq.top() << endl;   // 15
}
```

우선순위를 바꿔보자.

```cpp
#include <iostream>
#include <queue>
#include <functional>
#include <vector>
using namespace std;

int main()
{
    priority_queue<int, vector<int>, greater<int>> pq;

    pq.push(10);
    pq.push(20);
    pq.push(15);
    pq.push(12);

    cout << pq.top() << endl;   // 10
    pq.pop();
    cout << pq.top() << endl;   // 12
}
```

```cpp
#include <iostream>
#include <queue>
#include <functional>
#include <vector>
using namespace std;

struct abs_greater
{
    bool operator()(int a, int b) const
    {
        return abs(a) > abs(b);
    }
}

int main()
{
    priority_queue<int, vector<int>, abs_greater> pq;

    pq.push(10);
    pq.push(20);
    pq.push(15);
    pq.push(12);

    cout << pq.top() << endl;   // 10
    pq.pop();
    cout << pq.top() << endl;   // 12
}
```