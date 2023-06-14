---
title: "(C++) Function Pointer"
permalink: cpp/function-pointer/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-18 00:00:00 -0000
last_modified_at: 2020-09-18 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - function pointer
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
void test() {
    cout << "Hello" << endl;
}

int main() {
    test();     // 일반적 함수콜은 이렇게 한다.

    // 변수에 함수를 담고 싶다면??
    void (*pTest)();      // 함수를 담을 변수를 만들고
    pTest = test;       // 변수에 함수를 담는다.

    pTest();           // 호출은 이렇게..


    // 보통은 이렇게
    void (*ppTest)() = test;
    ppTest();

    return 0;
}
```

매개변수가 있다면?

```cpp
void test(int _a) {
    cout << "Hello" << endl;
}

void (*pTest)(int) = test;
```

---

## Using Function Pointers

```cpp
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

bool match(string test){
  return test.size() == 3;
}

int main() {
  vector<string> texts;
  texts.push_back("one");
  texts.push_back("one1");
  texts.push_back("one2");
  texts.push_back("one3");
  texts.push_back("one4");
  texts.push_back("one5");
  texts.push_back("one6");

  // 이런식으로 쓰면된다.
  cout << match("one") << endl;

  // 모두 돌리고 싶다면?
  cout << count_if(texts.begin(), texts.end(), match) << endl;

  return 0;
}
```

```cpp
// function pointer를 이용해 처리
bool match(string test){
  return test.size() == 3;
}

int countStrings(vector<string> &texts, bool (*match)(string test)) {
  int tally = 0;
  for(int i = 0; i < testx.size(); i++) {
    if(match(texts[i]) {
      tally++;
    }
  }

  return tally;
}

int main() {
  vector<string> texts;
  texts.push_back("one");
  texts.push_back("one1");
  texts.push_back("one2");
  texts.push_back("one3");
  texts.push_back("one4");
  texts.push_back("one5");
  texts.push_back("one6");

  cout << countString(texts, match) << endl;

  return 0;
}
```

---

## Object Slicing and Polymorphism

```cpp
#include <iostream>
using namespace std;

class Parent {
public:
  void print() {
    cout << "parent" << endl;
  }
};

class Child : public Parent {
public:
  void print() {
    cout << "child" << endl;
  }
};

int main() {
  Child c1;
  Parent &p1 = c1;
  p1.print();   // parent

  return 0;
}
```

```cpp
#include <iostream>
using namespace std;

class Parent {
public:
  // 만약 virtual 선언을 한다면?
  virtual void print() {
    cout << "parent" << endl;
  }
};

class Child : public Parent {
public:
  void print() {
    cout << "child" << endl;
  }
};

int main() {
  Child c1;
  Parent &p1 = c1;
  p1.print();   // child -> child가 출력됨.

  Parent p2 = Child();
  p2.print();   // parent -> =를 넣으며 복사 생성자가 생성.
  // 복사생성자를 재생성해보자

  return 0;
}
```

```cpp
#include <iostream>
using namespace std;

class Parent {
private:
  int one;
public:
  Parent() : one(0) {
    // 복사 생성자를 쓰기위해서 만듦
  }

  Parent(const Parent& other) : one(0) {
    one = other.one;
    cout << "copy parent" << endl;
  }

  virtual void print() {
    cout << "parent" << endl;
  }

  virtual ~Parent() {}
};

class Child : public Parent {
private:
  int two;

public:
  Child() : two(0) {}
  void print() {
    cout << "child" << endl;
  }
};

int main() {
  Child c1;
  Parent &p1 = c1;
  p1.print();   // child 

  Parent p2 = Child();  // 복사생성자로 Parent의 one변수를 변경할 수 있음.
  p2.print();   // 
  
  return 0;
}
```

---

## Abstract Classes and Pure Virtual Functions

```cpp
#include <iostream>
using namespace std;

// Abstract Class가 될 것이다.
class Animal {
  virtual void run() = 0;
  virtual void speak() = 0;
};

class Dog : public Animal {
  virtual void speak() {
    cout << "Woof!" << endl;
  }
};

class Labrador : public Dog {
public:
  Labrador() {
    cout << "new Labrador" << endl;
  }
  virtual void speak() {
    cout << "Labrador:Woof!" << endl;
  }
};

void test(Animal& a){
  a.run();
}

int main() {
  Animal animal;  // error - 추상가상이라
  Dog dog;        // error - run을 구현하지 않음
  Labrador l;     // okay
  l.speak();

  Animal *pAnimal[5];
  pAnimal[0] = &l;    // okay
  pAnimal[0]->speak();

  test(l);  // == l.run();

  return 0;
}
```

---

## Functor

```cpp
#include <iostream>
using namespace std;

struct Test {
  virtual bool operator()(string& text) = 0;
  virtual ~Test() {} 
};

struct MatchTest : public Test {
  virtual bool operator()(string &text) {
    return text == "lion";
  }
};

void check(string text, Test& test) {
  // functor는 이런식으로 활용이가능
  if(test(text)) {
    cout << "Text matches!" << endl;
  }
}

int main() {
  MatchTest pred;

  string value = "lion";

  // 마치 function 같지만 객체를 function처럼 쓰는 fuctor이다 
  cout << pred(value) << endl;

  MatchTest m;
  check("lion", m);

  return 0;
}
```