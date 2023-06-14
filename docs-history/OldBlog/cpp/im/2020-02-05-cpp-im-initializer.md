---
title: "(C++) 초기화 코드의 문제"
permalink: cpp/initializer/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-02-05 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - initializer
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 기존 초기화 코드의 문제

```cpp
int main()
{
  // 1. 초기화 방법이 너무 다양함.
  int n1 = 0;
  int n2(0);
  int ar[3] = { 1, 2, 3 };
  Point p = { 1, 2 };     // 구조체
  complex c(1, 2);        // 클래스
  
  // 2. 클래스 내부의 배열을 초기화 할 방법이 없다.
  class Test
  {
    int x[10];
  };
  
  // 3. 동적메모리할당을 할 경우 초기화 할 방법이 없다.
  int* p = new int[3];
  
  // 4. STL 컨테이너를 초기화하는 편리한 방법이 없다.
  vector<int> v;
  for(int i = 0; i < 10; i++)
    v.push_back(1);
}
```

```cpp
#include <iostream>
using namespace std;

int cnt = 0;

class Test
{
public:
  // int data = 0;   // member initializer
  int data = ++cnt;
  Test() {}
  Test(int n) : data(n) {}
};

int main()
{
  Test t1;      // data는 0으로 초기화
  Test t2(3);
  
  cout << cnt << endl;  // t1, t2 두 번 호출되기에 cnt는 2일까? Nope! 1임
  
  cout << t1.data << endl;
  cout << t2.data << endl;
}
```

```cpp
// 이렇게 하면 2가 나온다.
class Test
{
public:
  int data = ++cnt;
  Test() {}
  Test(int n) // : data(n) {}
  // data가 n으로 초기화 되는 것을 막는경우
};
```

```cpp
// 기존초기화의 문제점.
int main()
{
  // 1. 초기화 방법이 너무 다양함. -> 하나로 통일
  /*
  int n1 = 0;
  int n2(0);
  int ar[3] = { 1, 2, 3 };
  Point p = { 1, 2 };     // 구조체
  complex c(1, 2);        // 클래스
  */
  int n1{ 0 };
  int n2{ 0 };
  int ar[3]{ 1, 2, 3 };
  // Point p = { 1, 2 };     // 구조체
  // complex c(1, 2);        // 클래스
    
  // 2. 클래스 내부의 배열을 초기화 할 방법이 없다.
  class Test
  {
    // int x[10];
    int x[10]{ 1, 2, 3 ... , 10 };

  };

  
  // 3. 동적메모리할당을 할 경우 초기화 할 방법이 없다.
  // int* p = new int[3];
  int* p = new int[3]{1,2,3};
  
  // 4. STL 컨테이너를 초기화하는 편리한 방법이 없다.
 vector<int> v{1,2,3};
  /*
  vector<int> v;
  for(int i = 0; i < 10; i++)
    v.push_back(1);
}
*/
```

---

## 추가

```cpp
int main() {
  int values[] = {1, 4, 5};
  cout << values[0] << endl;    // 1

  class C1 {
  public:
    string text;
    int id;
  };

  C1 c1 = {"Hello", 7};
  cout << c1.text << endl;      // Hello

  struct R {
    string text;
    int id;
  } r1 = {"Hello", 7}, r2 = {"Hello2", 8};
  cout << r1.text << endl;      // Hello
  cout << r2.text << endl;      // Hello2

  // 컨테이너는 초기화 할 수 없을까?
  vector<string> strings;
  strings.push_back("One");
  strings.push_back("One1");
  strings.push_back("One2");

  return 0;
}
```

```cpp
int main() {
  int values[5];
  cout << values << endl;

  string text{"Hello"}
  cout << text << endl;

  int numbers[]{1,2,3};
  cout << numbers[1] << endl;

  int * pInts = new int[]{1,2,3}
  delete pInts;

  int value1{}; // 0으로 초기화 됨.
  int *pSomething{&value};  // 이런식 초기화 가능

  int numbers1[5]{};
  cout << numbers[1] << endl;

  struct {
    int value;
    string text;
  } s1{5, "Hi"};

  vector<string> strings{"one", "one1", "one2"};
  cout << strings[2] << endl;
}
```

---

## Initializer Lists

```cpp
#include <initializer_list>

class Test {
public:
  Test(initializer_list<string> text) {
    for(auto value : text) {
        cout << value << endl;
    }
  }

  void print(initializer_list<string> text) {
    for(auto value : texts) {
      cout << value << endl;
    }
  }
};

// 이런식으로 초기화가 가능해 진다.
Test test{"apple", "orange", "banana"};

// 이런거도 가능.
test.print({"one", "two", "three"});    // okay
```

---

## Object Initialization, Default and Delete

```cpp
class Test {
  int id{3};    // 될 수 있으면 이런식의 초기화를 하자.
  string name{"Mike"};

public:
  //Test() {}
  Test() = default;   // default를 활용하자
  Test(const Test& other) = default;    // 복사생성자도 기본형으로 생성
  Test &operator=(const Test& other) = default; // 비교연산자도 기본형 생성

  // Test(const Test& other) = delete;  // 아에 막아버릴수도 있다.

  // 물론 생성자에서 초기화를 할 수도 있다.
  Test(int id) : id(id) {

  }

  
  void print() {
    cout << id << " : " << name < endl;
  }
};

int main() {
  Test test;
  test.print();

  Test test1(77);
  test1.print();

  Test test2 = test1;   // 복사생성자
  test2 = test;

  return 0;
}
```

---

## 추가

* [참고](https://www.youtube.com/watch?v=U6mgsPqV32A&list=PL5jc9xFGsL8FWtnZBeTqZBbniyw0uHyaH)

```cpp
// 이런식으로 초기화 가능
#include <initializer_list>
class boVector {
  vector<int> m_vec;

public:
  boVector(const initializer_list<int>& v) {
    for(initializer_list<int>::iterator itr = v.begin(); itr != v.end(); ++itr)
      m_vec.push_back(*itr)
  }
};

boVector v = {0, 2, 3, 4};
boVector v{0, 2, 3, 4};
```