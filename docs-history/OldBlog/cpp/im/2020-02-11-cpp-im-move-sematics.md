---
title: "(C++) move sematics"
date: 2020-02-11 00:00:00 -0000
---

```cpp
class Cat
{
  char* name;
  int age;
public:
  Cat(const char* n, int a) : age(a)
  {
    name = new char[strlen(n) + 1];
    strcpy(name, n);
  }
  ~Cat() { delete[] name; }
};

int main()
{
  Cat c1("NABI", 2);
  Cat c2 = c1;    // runtime error - 디폴트 복사 생성자로 메모리 주소를 그대로 복사, delete할때 에러가 발생
}
```

```cpp
// 깊은 복사로 만들어보자.
Cat(const Cat& c) : age(c.age)
{
  name = new char[strlen(c.name) + 1];
  strcpy(name, c.name);
}
```

```cpp
Cat foo()   // 값리턴 : 임시객체(rvalue)
{
  Cat cat("NABI", 2);
  return cat;
}

int main()
{
  Cat c = foo();    // 임시객체. 다음줄에서 메모리 할당 해제 됨.
  // 그런데 어차피 소멸될 메모리인데 깊은 복사 말고 얕은 복사를 하면 메모리 할당해제 안해도 되고 좋을꺼 같은데??
  // -> move semantics
}
```

```cpp
// 소유권 이전(자원전달)의 이동(move) 생성자
// 임시객체일때만 사용되게 만들어야함. -> rvalue reference로 받자
Cat(Cat&& c) : age(c.age), name(c.name)
{
  c.name = 0;   // 자원 포기
}
```

---

```cpp
// 정리
class Test
{
  int* resource;
public:
  Test() {}
  ~Test() {}
  Test(const Test& t) { cout << "Copy" << endl; }
  
  // Move 생성자 : 소유권 이전(자원 전달)
  Test(Test&& t) { cout << "Move" << endl; }
};

int main()
{
  Test t1;
  Test t2 = t1;
  Test t3 = Test();
}
```