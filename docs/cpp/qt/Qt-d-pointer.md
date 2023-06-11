---
layout: default
title: "D Pointer"
parent: (QT)
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://idlecomputer.tistory.com/105)
* [참고사이트 🌎](https://wiki.qt.io/D-Pointer)

```cpp
#define Q_D(Class) Class##Private * const d = d_func() 
#define Q_Q(Class) Class * const q = q_func()
```

* 자신의 private 클래스에 접근하기 위해 사용

* **Q.** private 클래스는 왜 만드나?

```cpp
class Widget 
 { 
 // ... 
 private: 
    Rect m_geometry; 
 }; 
 class Label : public Widget 
 { 
 public: 
    // ...  
    String text() const  
    { 
        return m_text; 
    } 
 private: 
     String m_text; 
 };
```

* 가 있다고 가정하자 만약 private 클래스가 없다면 어떻게 될까?
* 위와같은 Widget를 상속해 Label이 쓴다고 가정하자

```cpp
class Widget 
 { 
    // ... 
 private: 
     Rect m_geometry; 
     String m_stylesheet; // NEW in WidgetLib 1.1 
 }; 
 class Label : public Widget 
 { 
 public: 
     // ... 
     String text() const 
     { 
         return m_text; 
     } 
 private: 
     String m_text; 
 };
```

* 버전이 업그레이드 되어 1.1이 되었는데 실수?로 라이브러리는 1.0을쓴다면?
* crash가 발생하게된다. -> wrapping이 필요하다

```cpp
class WidgetPrivate; 
class Widget 
{ 
    // ... 
    Rect geometry() const; 
    // ...  
private: 
    WidgetPrivate *d_ptr; 
};
```

```cpp
/* widget_p.h (_p means private) */ 
struct WidgetPrivate 
{ 
    Rect geometry; 
    String stylesheet; 
};
```

```cpp
// With this #include, we can access WidgetPrivate. 
#include "widget_p.h" 
Widget::Widget() : d_ptr(new WidgetPrivate) 
{ 
    // Creation of private data 
} 
Rect Widget::geometry() const 
{ 
    // The d-ptr is only accessed in the library code 
    return d_ptr->geometry; 
}
```

---

## 그럼 Q_Q, Q_D는 뭘까?

```cpp
#define Q_D(Class) Class##Private * const d = d_func() 
#define Q_Q(Class) Class * const q = q_func()
```

* Q_D - Private 클래스의 맴버에 접근 권한을 준다

```cpp
// With Q_D you can use the members of LabelPrivate from Label 
void Label::setText(const String &text) 
{ 
    Q_D(Label); 
    d->text = text; 
}
```

* Q_Q - Public 클래스의 접근 권한을 준다

```cpp
// With Q_Q you can use the members of Label from LabelPrivate 
void LabelPrivate::someHelperFunction() 
{ 
    Q_Q(Label); 
    q->selectAll(); 
}
```

* ??? Q_D는 알겠는데 Q_Q는 왜 필요하지 ???
* Private에서 Public에 정의된 함수가 필요할때가 있다.
* 그 때 사용하기 위해서 만듦.!