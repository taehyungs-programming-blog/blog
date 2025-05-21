---
layout: default
title: "42. std::bind"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## `std::bind` 란?

* `std::bind`는 C++11에서 도입된 함수 어댑터로, **함수 객체를 다른 함수 객체로 변환하는 데 사용**. 

* 주로 다음과 같은 용도로 활용
    * 함수의 일부 인자를 미리 고정(바인딩)
    * 인자 순서 변경
    * 콜백 함수 생성

```cpp
auto bound_function = std::bind(callable, arg1, arg2, ..., argN);
```

* 이렇게 봐선 무슨말 인지 모르니 예시코드로 확인해 보자.

---

## Example

### 인자 바인딩

```cpp
#include <iostream>
#include <functional>

void print_sum(int a, int b) {
    std::cout << "합계: " << a + b << std::endl;
}

int main() {
    // 첫 번째 인자를 10으로 고정
    auto add_ten = std::bind(print_sum, 10, std::placeholders::_1);
    
    add_ten(5);  // print_sum(10, 5) 호출 -> "합계: 15" 출력
    add_ten(20); // print_sum(10, 20) 호출 -> "합계: 30" 출력
    
    return 0;
}
```

### 인자 순서 변경

```cpp
#include <iostream>
#include <functional>

void divide(int a, int b) {
    if (b == 0) {
        std::cout << "0으로 나눌 수 없습니다." << std::endl;
        return;
    }
    std::cout << a << " / " << b << " = " << (a / b) << std::endl;
}

int main() {
    // 인자 순서 변경 (b, a) -> (a, b)
    auto safe_divide = std::bind(divide, std::placeholders::_2, std::placeholders::_1);
    
    safe_divide(2, 10);  // divide(10, 2) 호출 -> "10 / 2 = 5" 출력
    safe_divide(0, 5);   // divide(5, 0) 호출 -> "0으로 나눌 수 없습니다." 출력
    
    return 0;
}
```

### 멤버 함수 바인딩

```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

class Calculator {
public:
    int add(int a, int b) const {
        return a + b;
    }
    
    int multiply(int a, int b) const {
        return a * b;
    }
};

int main() {
    Calculator calc;
    
    // 멤버 함수 바인딩 (첫 번째 인자는 객체 인스턴스)
    auto add_five = std::bind(&Calculator::add, calc, 5, std::placeholders::_1);
    auto multiply_by_ten = std::bind(&Calculator::multiply, calc, std::placeholders::_1, 10);
    
    std::cout << "5 + 3 = " << add_five(3) << std::endl;  // "5 + 3 = 8" 출력
    std::cout << "7 * 10 = " << multiply_by_ten(7) << std::endl;  // "7 * 10 = 70" 출력
    
    // 벡터에 적용
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::vector<int> results(5);
    
    std::transform(numbers.begin(), numbers.end(), results.begin(), multiply_by_ten);
    
    std::cout << "결과: ";
    for (int n : results) {
        std::cout << n << " ";  // "결과: 10 20 30 40 50" 출력
    }
    std::cout << std::endl;
    
    return 0;
}
```

### 콜백 함수로 활용

```cpp
#include <iostream>
#include <functional>
#include <vector>

class Button {
private:
    std::function<void()> on_click;
    std::string name;
    
public:
    Button(const std::string& btn_name) : name(btn_name) {}
    
    void set_click_handler(const std::function<void()>& handler) {
        on_click = handler;
    }
    
    void click() {
        std::cout << name << " 버튼이 클릭되었습니다." << std::endl;
        if (on_click) {
            on_click();
        }
    }
};

class MessagePrinter {
private:
    std::string prefix;
    
public:
    MessagePrinter(const std::string& p) : prefix(p) {}
    
    void print_message(const std::string& message) const {
        std::cout << prefix << ": " << message << std::endl;
    }
};

int main() {
    Button btn1("확인");
    Button btn2("취소");
    
    MessagePrinter printer("알림");
    
    // 멤버 함수와 인자를 바인딩하여 콜백 함수 생성
    btn1.set_click_handler(std::bind(&MessagePrinter::print_message, printer, "확인 작업이 완료되었습니다."));
    btn2.set_click_handler(std::bind(&MessagePrinter::print_message, printer, "작업이 취소되었습니다."));
    
    btn1.click();  // "확인 버튼이 클릭되었습니다." 및 "알림: 확인 작업이 완료되었습니다." 출력
    btn2.click();  // "취소 버튼이 클릭되었습니다." 및 "알림: 작업이 취소되었습니다." 출력
    
    return 0;
}
```

* C++11 이후로는 std::bind 대신 람다 표현식을 사용하는 경우가 많습니다. 람다가 더 읽기 쉽고 컴파일러 최적화에 유리한 경우가 많기 때문입니다.