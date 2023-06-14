---
title: "(C++) move 다시 정리"
permalink: cpp/move/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-04 00:00:00 -0000
last_modified_at: 2020-10-08 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - move
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## move의 개념

우선 문제는 이렇게 시작한다<br>
복사 생성자로 r, l value를 모두 받을 순 없나?<br>

```cpp
// 우리가 아는 복사 생성자의 모습은 이렇다
Point(const Point& p) {}
```

위 모습데로라면 l value만 복사할 수 있다.<br>
r value로 복사하고 싶은데 난??<br>

```cpp
// 음 그렇다면 r value reference로 받으면 되겠네
Point(Point&& p);
```

앗? 그리고 또 하나<br>
r value를 복사해 달라고 들어온 객체라면 어차피 다음줄로 넘어가면 없어진다(이는 rvalue 특징임)<br>
그럼 r value 복사 생성자에서 복사를 하지말고 **r value메모리 자체를 옮겨(move)보자.**<br>
여기서 move의 개념이 시작된다.<br>

```cpp
class Test
{
public:
    Test() {}
    ~Test() {}
    Test(const Test& t) { cout << "Copy" << endl; }
    Test(Test&& t) { cout << "Move" << endl; }
};

int main()
{
    Test t1;
    Test t2 = t1;           // Copy
    Test t3 = Test();       // Move
    Test t4 = t1;           // Copy -> 그러런데 t1을 더 이상사용하지 않을 거라면?

    // !아래가 핵심!
    Test t4 = static_cast<Test&&>(t1);    // Move, 생성자 호출됨.
    Test t5 = move(t2);                   // Move, 위와 동일한 동작

    // move = static_cast<Test&&> 라고 봐도 좋다.
}
```

`Test t4 = static_cast<Test&&>(t1);` r value로 형변환 후 t4에 넣어버린다. 주소를 move해버린 효과<Br>
std에서는 이런 move를 많이 쓰기에 함수로 만들어 둠! -> `Test t5 = move(t2);`<br>

동일한 방식으로 대입연산자도 만들 수 있다.

```cpp
Test& operator=(const Test& t) { return *this; }    // 복사 대입연산자
Test& operator=(Test&& t) { return *this; }         // move 대입연산자
```

---

## 언제쓰는데?

버퍼를 늘릴때

```cpp
int main()
{
  Test* p1 = new Test[2];
  // 버퍼를 늘리고 싶다 2 -> 4
  Test* p2 = new Test[4];
  
  for(int i = 0; i < 2; i++)
    p2[i] = p1[i];          // copy 대입! -> 값 복사가 일어나며 역시 성능저하가 발생
    p2[i] = move(p1[i]);    // move 대입 -> 값 복사를 하짐라고 레퍼런스만 만들어라, 성능 향상을 본다.
}
```

move생성자에서

```cpp
// Buffer가 아래와 같다면
class Buffer
{
  size_t sz;
  int* buf;
  string tag;
  Test test;
public:
  Buffer(size_t s, string t) : sz(s), tag(t), buf(new int[s]) {}
  ~Buffer() { ~delete[] buf; }
  
  // 깊은 복사
  Buffer(const Buffer& b) : sz(b.sz), tag(b.tag), test(b.test)
  {
    buf = new int[sz];
    memcpy(buf, b.buf, sizeof(int)*sz)
  }
};
```

```cpp
// 깊은 복사로 생성가능
Buffer(Buffer&& b) noexcept
: sz(move(b.sz), tag(move(b.tag)), test(move(b.test))
{
    buf = b.buf;
    b.buf = 0; // 자원 포기
}
```

---

## 참고사항 noexcept에 관하여

move 동작 중 except사항이 발생하면 시스템에 큰 문제를 야기할 수 있다.<br>
따라서 std는 noexcept의 사용을 강제 한다.<br>

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
```

---

## 추가

```cpp
Test(Test &&other) {
  // 이게 Move와 동일한 동작이다
  _pBuffer = other._pBuffer;
  other._pBuffer = nullptr;
}
```

```cpp
Test &operator=(Test &&other) {
  // 이게 Move와 동일한 동작이다
  delete [] _pBuffer
  _pBuffer = other._pBuffer;
  other._pBuffer = nullptr;

  return *this;
}
```

---

## 추가2(20.10.08)

* [참고사이트](https://www.youtube.com/watch?v=IOkgBrXCtfo&list=PL5jc9xFGsL8FWtnZBeTqZBbniyw0uHyaH&index=3)

```cpp
void printInt(int& i) { cout << "lvalue reference: " << i << endl; }
void printInt(int&& i) { cout << "rvalue reference: " << i << endl; }
// void printInt(int i) {} // 이걸 넣어버리면 컴파일이 안됨. 컴파일러가 어떤 함수를 사용해야할지 모르기 때문이다.

int main() {
  int a = 5;    // a is a lvalue
  int& b = a;   // b is a lavlue(reference)
  int&& c;      // c is a rvalue

  printInt(a);    // Call printInt(int& i)
  printInt(6);    // Call printInt(int&& i)
}
```

```cpp
class boVector {
  int size;
  double* arr_;    // a big array
public:
  boVector(const boVector& rhs) {
    size = rhs.size;
    arr_ = new double[size];
    for(int i = 0; i < size; i++) { arr_[i] = rhs.arr_[i]; }
  }
  ~boVector() { delete arr_; }
};

void foo(boVector v);
boVector createBoVector();  

void main() {
  boVector resuable = createBoVector();
  foo(resuable);    

  foo(createBoVector()); // big array가 복사되어 버린다. -> 메모리의 낭비 및 시간의 낭비이다.

  // Move constructor로 해결해보자.
}
```

```cpp
class boVector {
  int size;
  double* arr_; 
public:
  boVector(const boVector& rhs) {
    size = rhs.size;
    arr_ = new double[size];
    for(int i = 0; i < size; i++) { arr_[i] = rhs.arr_[i]; }
  }
    boVector(boVector&& rhs) {    // Move constructor
    size = rhs.size;
    arr_ = rhs.arr_;
    rhs.arr_ = nullptr;
  }
  ~boVector() { delete arr_; }
};

void foo_by_value(boVector v);
void foo_by_ref(boVector& v);

int main() {
  foo_by_ref(createBoVector());
}
```

좀 더 C++11스럽게 해결해 보자

```cpp
void foo(boVector v);
void foo_by_ref(boVector& v);

int main() {
  boVector resuable = createBoVector();
  foo(resuable);            // Call copy constructor
  foo(std::move(resuable)); // Call Move constructor

  foo_by_ref(resuable);             // Call no constructor
}
```