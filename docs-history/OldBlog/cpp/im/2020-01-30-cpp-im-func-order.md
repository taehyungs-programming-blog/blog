---
title: "(C++) 함수 찾는 순서"
date: 2020-01-30 00:00:00 -0000
---

> 똑같은 이름이 여러개 있을때 함수를 찾는 순서를 보자.

```cpp
struct FLOAT
{
  FLOAT(float) {}
};

template<typename T>
void foo(T)         { cout << "T" << endl; }
void foo(int)       { cout << "int" << endl; }
void foo(double)    { cout << "double" << endl; }
void foo(float)     { cout << "float" << endl; }
void foo(FLOAT)     { cout << "FLOAT" << endl; }
void foo(...)       { cout << "..." << endl; }

int main()
{
  float f = 3.4f;
  foo(f);     // float - exactly matching이라하며 정확하게 맞는 매개변수 타입이 있을시 선택됨

  // void foo(float)이 없다면?
  foo(f);     // T - tempate 버전이 호출됨
  
  // void foo(T)가 없다면?
  foo(f);     // double - promotion 데이터 손실이 없는 방향으로 선택
  
  // void foo(double)이 없다면?
  foo(f);     // int - standard conversion, 데이터 손실이 있더라도 기존 자료형에 들어감
  
  // void foo(int)가 없다면?
  foo(f);     // 사용자 정의변환으로 출력
  
  // 마지막은 가변인자 (...)
}
```