---
layout: default
title: "2. std::shared_ptr"
parent: (C++)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

##  shared_ptr이란?

* 결론적으론 개발자가 delete를 직접하지 않게 하겠다가 핵심이다.

```cpp
// Example
class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
    void Go() { cout << "Car go" << endl; }
};

int main() {
    vector<shared_ptr<Car>> m_carVec;
    for(int i =0 ; i < 10; i++)
    {
        m_carVec.push_back(make_shared<Car>());
    }
    // ~Car() 10번 호출됨.
    // 이런식으로 delete를 직접해주지 않아도 된다.
    /*
        참고)
        m_carVec.pop_back();        // pop_back을 해도 역시하 소멸이 된다.
    */
    return 0;
}
```

```cpp
// 생성시 유의 사항
shared_ptr<Car> p = new Car;        // error
shared_ptr<Car> p(new Car);         // ok, 사실 추천은 make_shared이다.
```

---

## 삭제자 넣기

```cpp
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
// 자세한건 내용이 길어져 iocp내용 참고
```

---

## 배열사용하기

```cpp
shared_ptr<Car> p(new Car[10], [](Car* p){ delete[] p; });
// 배열을 쓸 경우 삭제자를 무조건 넣어줘야한다.
p[0].Go();      // error - shared_ptr은 배열을 고려해서 만들어지지 않았음
```

```cpp
// C++17 이후에는 shared_ptr에 배열을 지원한다.
shared_ptr<Car> p1(new Car[10]);       // error - 삭제자를 넣어줘야함!
shared_ptr<Car[]> p1(new Car[10]);      // ok!
```

---

## 참고 함수들

```cpp
shared_ptr<Car> p(new Car);
p->Go();
//p.something; // shared_ptr자체 멤버에 접근
p.get();                // 대상체의 포인터
p.usecount();           // 참조계수 반환
p.reset(new Car);       // 대상체 변경
p.swap(new Car);        // 대상체 교환
```
