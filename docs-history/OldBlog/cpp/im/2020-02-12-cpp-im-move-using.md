---
title: "(C++) using move"
date: 2020-02-11 00:00:00 -0000
---

```cpp
class Test
{
publid:
  Test() {}
  ~Test() {}
  Test(const Test& t) { cout << "Copy" << endl; }
  Test(Test&& t)      { cout << "Move" << endl; }
  
  Test& operator=(const Test& t)
  {
    cout << "Copy=" << endl;
    return *this;
  }
  Test& operator=(Test&& t)
  {
    cout << "Move=" << endl;
    return *this;
  }
};

template<typename T> void Swap(T& x, T& y)
{
  Test temp = x;    // 복사 생성자 호출
  x = y;            // 복사 대입 호출
  y = temp;         // 복사가 너무 잦군... -> 성능저하의 요인이 된다.
}

int main()
{
  Test t1, t2;
  Swap(t1, t2);
}
```

```cpp
template<typename T> void Swap(T& x, T& y)
{
  Test temp = move(x);
  x = move(y);
  y = move(temp);
}
```

```cpp
// 이럴줄 알고 STL이 준비했다.
#include <algorithm>

int main()
{
  Test t1, t2;
  swap(t1, t2);   // STL에서 기본으로 제공해주는 함수. -> move를 이용해 복사한다.
}
```

---

```cpp
int main()
{
  Test* p1 = new Test[2];
  // 버퍼를 늘리고 싶다 2 -> 4
  Test* p2 = new Test[4];
  
  for(int i = 0; i < 2; i++)
    p2[i] = p1[i];    // copy 대입! -> 역시 성능저하가 발생
    p2[i] = move(p1[i]);    // move 대입 -> 성능 향상을 본다.
}
```

```cpp
vector<Test> v(2);
v.resize(4);      // copy 대입이 호출되는데 강제로 move 대입을 호출을 원한다면
// 아래와 같이 noexcept 선언

class Test
{
publid:
  Test() {}
  ~Test() {}
  Test(const Test& t) { cout << "Copy" << endl; }
  Test(Test&& t) noexcept { cout << "Move" << endl; }
  
  Test& operator=(const Test& t)
  {
    cout << "Copy=" << endl;
    return *this;
  }
  Test& operator=(Test&& t) noexcept
  {
    cout << "Move=" << endl;
    return *this;
  }
};

// 좀 더 자세한 사항은 다음강에 ...
```