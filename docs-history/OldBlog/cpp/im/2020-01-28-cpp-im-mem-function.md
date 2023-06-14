---
title: "(C++) 멤버함수 포인터"
date: 2020-01-28 00:00:00 -0000
---

```cpp
class Dialog
{
public:
  void Close() {}
};

void foo() {}

int main()
{
  void(*f1)() = &foo;   // ok
  void(*f2)() = &Dialog::Close;   // error(this call로 인해서 첫 번째 멤버변수로 this가 들어감.)
}
```

* 일반 함수 포인터에 멤버함수를 주소로 담을수 없다.
* 단, 일반 함수 포인터에 static 멤버하수의 주소는 담을 수 있다.

```cpp
void(Dialog::*f3)() = &Dialog::Close;   // ok - 멤버 함수 포인터라 한다.

f3();   // error! this가 없기에...
Dialog dlg;
dlg.f3();   // 역시 error! 컴파일러는 Dialog내부에서 f3을 먼저찾게된다.

dlg.*f3();    // pointer to member operator라 한다.
// 단 연산자 우선순위의 문제로 ()가 먼저 호출이된다.

(dlg.*f3)();    // ok! ->정확한 pointer to member operator
// Or
(dlg->*f3)();
```

* 멤버 함수 포인터의 모양과 사용법을 알자

