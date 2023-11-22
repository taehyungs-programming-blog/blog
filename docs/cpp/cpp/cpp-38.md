---
layout: default
title: "38. std::reference_wrapper"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이걸 왜 쓰지?

* 다음과 같은 객체가 있다고 가정하자

```cpp
class apple {
public:
    int data;
    apple(int tmpData):data(tmpData) {
        cout<<"Init"<<'\n';
    }
    ~apple() {
        cout <<data <<" deleting" << '\n';
    }
    apple(const apple& other) {
        cout << "Copying"<<'\n';
        data = other.data;
    }
    void Shout(int x,int y) {
        cout << "I have" <<x+y<< "apples!!!!" << '\n';
    }
    void BindShout(function<void(int)>& other) {
        other = std::bind(&apple::Shout, this,std::placeholders::_1, 2);
    }
};
```

* apple을 `vector`에 담고 싶다 가장 좋은 방법은? 

```cpp
vector<apple> v; // apple 객체를 복사해서 저장
vector<apple*> v; // apple 객체의 포인터를 저장
vector<reference_wrapper<apple>> v; // apple 객체의 참조를 저장
```

* 세 번째 방법이다.
    * 두 번째는 왜? -> 포인터는 null의 위험이 ...

---

## 그럼 reference_wrapper란 뭘까?

* C++ 표준 라이브러리의 일부로, 참조를 안전하게 저장하고 복사할 수 있게 해주는 래퍼(wrapper)입니다.
* 일반적으로 참조는 객체에 대한 직접적인 링크를 나타내며, **참조 자체를 바꾸는 것은 불가능**합니다. 
    * 즉, 참조는 한 번 초기화되면 **다른 객체를 가리키도록 변경할 수 없습니다.**

```cpp
// Q - 다음 에러의 원인은?

int a{10};

std::map<int, std::reference_wrapper<int>> m_list;
m_list.insert({1, a});
m_list.insert({2, a});

int b{20};
m_list[1] = std::ref(b); // Error
```

* 이 문제가 발생하는 이유는 std::map의 operator[] 사용과 관련이 있습니다. 
* std::map의 operator[]는 키에 해당하는 요소가 맵에 존재하지 않을 경우, 해당 키에 대한 새로운 요소를 생성합니다. 
* 이 과정에서 std::reference_wrapper의 기본 생성자가 필요하게 되는데, std::reference_wrapper에는 기본 생성자가 없기 때문에 에러가 발생합니다.

```cpp
// 해결책
m_list.insert_or_assign(1, std::ref(b));
```

---

## std::ref ??

* std::ref는 C++의 표준 라이브러리에서 제공하는 함수 템플릿으로, 주로 함수 인자를 전달할 때 참조로 전달하도록 도와주는 유틸리티입니다. 
* 이를 통해 값에 대한 참조를 래핑(wrap)하여 값 대신 참조를 전달할 수 있게 됩니다.

### std::ref의 기본적인 사용

* 참조 래핑: std::ref는 주어진 인자에 대한 std::reference_wrapper 객체를 생성합니다. 이 객체는 내부적으로 참조를 저장하고, 이 참조를 다양한 컨텍스트에서 값처럼 사용할 수 있게 합니다.
* 함수 인자 전달: 특히 스레드 생성이나 알고리즘 라이브러리에서 함수에 인자를 참조로 전달해야 할 때 유용합니다. 예를 들어, std::thread를 생성할 때 객체의 참조를 전달하려면 std::ref를 사용합니다.

### std::ref와 std::reference_wrapper

* std::ref는 std::reference_wrapper<T> 타입의 객체를 반환합니다. std::reference_wrapper는 다음과 같은 특징을 가지고 있습니다:

* 참조 보존: std::reference_wrapper는 내부적으로 참조를 저장합니다. 복사나 할당 시에도 원본 객체의 참조를 유지합니다.
* 재할당 불가: 한 번 생성된 std::reference_wrapper 객체는 다른 객체로의 참조를 변경할 수 없습니다.
* 함수 호출 지원: std::reference_wrapper는 operator()를 오버로드하여, 참조하는 객체가 함수인 경우 해당 함수를 호출할 수 있습니다.

```cpp
void increment(int& n) {
    ++n;
}

int main() {
    int x = 0;
    std::thread t(increment, std::ref(x));  // x의 참조를 스레드 함수에 전달
    t.join();
    // 이 시점에서 x는 1이 됩니다.
    return 0;
}
```