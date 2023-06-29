---
layout: default
title: "30. 기본, 복사, 이동 생성자 호출"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

```cpp
class MyString {
public:
    // default constructor
    MyString(const char* str) {
        std::cout << "default constructor " << std::endl;
    }

    // copy constructor
    MyString(const MyString& str) {
        std::cout << "copy constructor " << std::endl;
    }

    // move constructor
    MyString(const MyString&& str) {
        std::cout << "move constructor " << std::endl;
    }

    ~MyString() {}
};

int main()
{
    MyString("abc");                // default
    MyString a = MyString("abc");   // default - 디폴드 생성 후 a에 주소를 넣는다고 생각
    MyString b = a;                 // copy
    MyString c = std::move(b);      // move
    MyString d = "abc";             // default
}
```

{% endraw %}