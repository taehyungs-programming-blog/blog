---
title: "(C++) shared_ptr"
permalink: cpp/stl/spointer/shared_ptr/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-15 00:00:00 -0000
last_modified_at: 2020-10-08 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - shared_ptr
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 언제쓰냐에 제일 좋은 답변

* 어쨋든 스마트 포인터가 사용자가 delete를 안하게 하고 싶다는게 핵심이다.

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

void foo(Car* p)
{
    cout << "Delete Car" << endl;
}

int main() {
	vector<shared_ptr<Car>> m_carVec;

	for(int i =0 ; i < 10; i++)
	{
		m_carVec.push_back(make_shared<Car>());
	}

    // ~Car() 10번 호출됨.
    // 이런식으로 delete를 직접해주지 않아도 된다.

	return 0;
}
```

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

void foo(Car* p)
{
    cout << "Delete Car" << endl;
}

int main() {
	vector<shared_ptr<Car>> m_carVec;

	for(int i =0 ; i < 10; i++)
	{
		m_carVec.push_back(make_shared<Car>());
	}

	m_carVec.pop_back();        // pop_back을 해도 역시하 소멸이 된다.

	cout << " New Line" << endl;

	return 0;
}
```

---

## 1

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

int main()
{
    int a = 0;      // 복사 생성자
    int a(0);       // direct 생성자

    shared_ptr<Car> p = new Car;        // error
    shared_ptr<Car> p(new Car);         // ok
}
```

* shared_ptr은 복사 생성자로 생성이 불가능하다

```cpp
shared_ptr<Car> p1(new Car);
shared_ptr<Car> p2 = p1;
```

* 이렇게 여러 shared_ptr이 한 객체를 가리킬때는 두(p1, p2) 모두 사용을 종료할 경우만 메모리가 해제된다.

* [Run This Code](https://ideone.com/x9lNjb)

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

void foo(Car* p)
{
    cout << "Delete Car" << endl;
    delete p;
}

int main()
{
    shared_ptr<Car> p(new Car, foo);    // 삭제자를 넣을 수 있다.

    shared_ptr<Car> p(new Car, [](Car* p){ delete p; }]);
}
```

```cpp
shared_ptr<Car> p(new Car, foo, MyAlloc<Car>);  // 할당자 역시 넣을 수 있다.
```

---

* 배열형태로 메모리를 할당한다면??

* [Run This Code](https://ideone.com/5y2IYw)

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

void foo(Car* p)
{
    cout << "Delete Car" << endl;
    delete p;
}

int main()
{
    shared_ptr<Car> p(new Car[10], [](Car* p){ delete[] p; });
    // 배열을 쓸 경우 삭제자를 무조건 넣어줘야한다.

    p[0].Go();      // error - shared_ptr은 배열을 고려해서 만들어지지 않았음
}
```

```cpp
// C++17 이후에는 shared_ptr에 배열을 지원한다.
shared_ptr<Car> p1(new Car[10]);       // 삭제자를 넣어줘야함!
shared_ptr<Car[]> p1(new Car[10]);      // ok!
```

---

## 2

* [Run This Code]()

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

int main()
{
    shared_ptr<Car> p(new Car);

    p->Go();
    //p.something; // shared_ptr자체 멤버에 접근
    p.get();                // 대상체의 포인터
    p.usecount();           // 참조계수 반환
    p.reset(new Car);       // 대상체 변경
    p.swap(new Car);        // 대상체 교환
}
```

* make_shared

* [Run This Code](https://ideone.com/AWFDJO)

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

void* operator new(size_t sz)
{
    cout << "new sz : " << sz << endl;
    return malloc(sz);
}

int main()
{
    shared_ptr<Car> p(new Car);     // new를 2번 호출하게 됨.
    // 그 이유는 new Car와 shared_ptr를 관리하는 메모리가 별도로 존재하기 때문 ...
    // 메모리를 붙이는 방법은 없나??
```

![](/file/image/stl-shared_ptr.png)

* [Run This Code](https://ideone.com/VUdgob)

```cpp
shared_ptr<Car> p1 = make_shared<Car>();
```

![](/file/image/stl-shared_ptr2.png)

* make_shared는 다음과 같은 장점이 있다.
    * 대상 객체와 제어 블럭을 동시에 메모리 할당 하기에 메모리관리에 효율적(위의 예제)
    * 예외 상황에 좀 더 안전(아래 예제)

```cpp
f(shared_ptr<Car>(new Car), foo());
// 위 f라는 함수를 실행시 이러한 문제가 있을 수 있다.
// new Car를 통해 메모리를 할당 후 컴파일러에 따라 foo()를 먼저 부를 수 있다.
// foo()에서 예외상황이 발생하면 new Car는 메모리 릭이 된다.

f(make_shared<Car>(), foo());       // 그런 상황을 방지가능!
```

```cpp
shared_ptr<Car> p1 = allocated_shared<Car>(MyAlloc<Car>());
```

---

## 3

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

int main()
{
    Car* p = new Car;

    shared_ptr<Car> sp1(p);
    shared_ptr<Car> sp2(p);
    // sp1이 p를 삭제해도 sp2는 모른다.
```

* raw pointer를 사용해서 2개 이상의 shared_ptr를 생성하면 안된다.

* [Run This Code](https://ideone.com/Nc1SFA)

```cpp
#include <iostream>
#include <memory>       // for shared_ptr
#include <thread>
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

class Worker
{
    Car c;
public:
    void Run()
    {
        thread t(&Worker::Main, this);
        t.detach();
    }
    void Main()
    {
        c.Go();
        cout << "Finish Thread" << endl;
    }
};

int main()
{
    {
        shared_ptr<Worker> sp = make_shared<Worker>();
        sp->Run();
    }
    // thread가 종료되기 전 block을 벗어나며 메모리를 해제해 버린다면?? -> 문제가 된다.
}
```

```cpp
class Worker : public enable_shared_from_this<Worker>
{
    Car c;
    shared_ptr<Worker> holdMe;
public:
    void Run()
    {
        holdMe = shared_from_this();
        // 자신을 제어하는 스마트 포인터 제어블럭을 받는다.

        thread t(&Worker::Main, this);
        t.detach();
    }
    void Main()
    {
        c.Go();
        cout << "Finish Thread" << endl;
    }
};
```

---

## 추가

* [참고사이트](https://www.youtube.com/watch?v=qUDAkDvoLas&list=PL5jc9xFGsL8FWtnZBeTqZBbniyw0uHyaH&index=7)

```cpp
#include <iostream>
#include <string>
#include <memory>
using namespace std;

class Dog {
    string name_;
public:
    Dog(string name) { 
        cout << "Dog is created : " << name << endl;
        name_ = name; 
    }
    Dog() {
        cout << "Nameless dog created." << endl;
        name_ = "nameless";
    }
    ~Dog() {
        cout << "dog is destroyed: " << name_ << endl;
    }
    void bark() {
        cout << "Dog " << name_ << " rules!" << endl;
    }
}

void foo(){
    Dog* p = new Dog("Gunner");
    // ...
    delete p;
    // ...

    p->bark();  // error : p is a dangling pointer
}   // Memory leak
// 결국은 delete time이 문제이다. 이걸해결해보자 -> smart pointer

int main() {

}
```

```cpp
void foo() {
    shared_ptr<Dog> p(new Dog("Gunner"));       // Count = 1
    p->bark();
}   // out of scope
// Count = 0
// delete Dog
```

```cpp
void foo() {
    shared_ptr<Dog> p(new Dog("Gunner"));       // Count = 1
    {
        shared_ptr<Dog> p2 = p;                 // Count = 2
        p2->bark();
        // cout << p.use_count();       // Count를 출력가능
    }
    // Count = 1
    p->bark();
}   // out of scope
// Count = 0
// delete Dog
```

아래처럼 쓰지말자

```cpp
Dog* d = new Dog("Tank");
shared_ptr<Dog> p(d);       // p.use_count() == 1
shared_ptr<Dog> p2(d);      // p2.use_count() == 1
// 이렇게 쓰면안된다는 말.
```

An object should be assigned to a smart pointer as soon as it is created<br>
Raw pointer should not be used.

아래처럼 쓰자

```cpp
shared_ptr<Dog> p = make_shared<Dog>("Tank");
```

왜?

```cpp
shared_ptr<Dog> p(new Dog("Gunner"));
// step 1 : Gunner is created
// step 2 : p is created

shared_ptr<Dog> p = make_shared<Dog>("Tank");
// step 1에 p, Tank 모두 created
```

```cpp
shared_ptr<Dog> p1 = make_shared<Dog>("Gunner");
shared_ptr<Dog> p2 = make_shared<Dog>("Tank");
p1 = p2;        // Gunner is deleted
p1 = nullptr;   // Gunner is deleted
p1.reset();     // Gunner is deleted
```

삭제자를 선언해보자

```cpp
shared_ptr<Dog> p2 = shared_ptr<Dog>(new Dog("Tank"),
            [](Dog* p) {cout << "Custom deleteing."; delete p;} );

shared_ptr<Dog> p3(new Dog[3]);     // dog[1], dog[2] is memory leak
shared_ptr<Dog> p4(new Dog[3], [](Dog* p) {delete[] p;});
```

get함수

```cpp
Dog* d = p1.get();
// Return the raw pointer

// 이건 쓰지말자
delete d;
shared_ptr<Dog> p2(d);
function(d);    // shared_ptr로 보내야 안전
```