---
title: "(C++) Nested Template Classes"
permalink: cpp/nest-template/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-20 00:00:00 -0000
last_modified_at: 2020-09-20 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - Nested Template Classes
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
// 아래와 같은 기능이 가능하게 ring class를 구현해보자
#include "ring.h"

int main() {
    ring<string>::iterator it;

    cout << *it << endl;

    ring<string> textring(3);

    textring.add("one");
    textring.add("two");
    textring.add("three");

    for(int i = 0; i < textring.size(); i ++) {
        cout << textring.get(i) << endl;
    }

    return 0;
}
```

```cpp
#ifndef RING_H_
#define RING_H_

#include <iostream>
using namespace std;

class ring {
public:
    class iterator {
    public:
        void print() {
            cout << "Hellow from iterator" << endl;
        }
    };
};

#endif  // RING_H_
```

```cpp
int main() {
    ring<string>::iterator it;
    it.print();
    // 여기까지는 구현됨.
```

코드를 조금 정리해보자.

```cpp
#ifndef RING_H_
#define RING_H_

#include <iostream>
using namespace std;

class ring {
public:
    class iterator;
};

class ring::iterator {
public:
    void print();
};

void ring::iterator::print() {
    cout << "Hellow from iterator" << endl;
}

#endif  // RING_H_
```

ring의 자료형을 정의해보자

```cpp
#ifndef RING_H_
#define RING_H_

#include <iostream>
using namespace std;

template<class T>
class ring {
public:
    class iterator;
};

template<class T>
class ring<T>::iterator {
public:
    void print();
};

void ring::iterator::print() {
    cout << "Hellow from iterator : " << T() << endl;
}

#endif  // RING_H_
```

---

## A Ring Buffer Classes

위 내용의 연장이다.

```cpp
#ifndef RING_H_
#define RING_H_

#include <iostream>
using namespace std;

template<class T>
class ring {
private:
    int m_pos;
    int m_size;
    T *m_values;

public:
    class iterator;

public:
    ring(int size) : m_size(size), m_values(NULL), m_pos(0) {
        m_values = new T[size];
    }

    ~ring() {
        delete [] m_values;
    }

    int size() {
        return m_size;
    }

    void add(T value) {
        m_values[m_pos++] = value;

        if(m_pos == m_size) {
            m_pos = 0;
        }
    }

    T& get(int pos) {
        return m_values[pos];
    }
};

template<class T>
class ring<T>::iterator {
public:
    void print();
};

void ring::iterator::print() {
    cout << "Hellow from iterator : " << T() << endl;
}

#endif  // RING_H_
```

---

## Making Classes iterable

```cpp
#include "ring.h"

int main() {
    ring<string>::iterator it;

    cout << *it << endl;

    ring<string> textring(3);

    textring.add("one");
    textring.add("two");
    textring.add("three");

    for(int i = 0; i < textring.size(); i ++) {
        cout << textring.get(i) << endl;
    }

    // for까지 구현
    // 예전 구현 방식
    for(ring<string>::iterator it = textring.begin(); it != textring.end(); it++) {
        cout << *it << endl;
    }

    cout << endl;

    // C++11이후 구현
    for(string value : textring) {
        cout << value << endl;
    }

    return 0;
}
```

```cpp
#ifndef RING_H_
#define RING_H_

#include <iostream>
using namespace std;

template<class T>
class ring {
private:
    int m_pos;
    int m_size;
    T *m_values;

public:
    class iterator;

public:
    ring(int size) : m_size(size), m_values(NULL), m_pos(0) {
        m_values = new T[size];
    }

    ~ring() {
        delete [] m_values;
    }

    int size() {
        return m_size;
    }

    iterator begin() {
        return iterator(0, *this);
    }

    iterator end() {
        return iterator(m_size, *this);
    }

    void add(T value) {
        m_values[m_pos++] = value;

        if(m_pos == m_size) {
            m_pos = 0;
        }
    }

    T& get(int pos) {
        return m_values[pos];
    }
};

template<class T>
class ring<T>::iterator {
private:
    int m_pos;
    ring m_ring;

public:
    iterator(int pos, ring& aring) : m_pos(pos), m_ring(aring){

    }

    iterator &operator++(int a) {
        m_pos++;
        return *this;
    }

    iterator &operator++() {
        m_pos++;
        return *this;
    }

    T & operator*() {
        return m_ring.get(m_pos);
    }

    bool operator!=(const iterator & other) const {
        return m_pos != other.m_pos;
    }
};

#endif  // RING_H_
```