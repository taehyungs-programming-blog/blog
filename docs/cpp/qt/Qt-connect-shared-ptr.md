---
layout: default
title: "QSharedPointer는 Connect가 될까?"
parent: (QT)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🏄‍♂️ 정답은? **된다.**<br>
🏄‍♂️ 예시를 보자.

```cpp
class A : public QWidget
{
    Q_OBJECT

public:
    A() {}
    ~A() {}

public slots:
    void Fn();
};

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

private:
    Ui::QtWidgetsApplication1Class ui;

    QSharedPointer<A> a;
    std::shared_ptr<A> aa;

private slots:
    void Fn();
};
```

```cpp
QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    a = QSharedPointer<A>(new A);       // QSharedPointer도 되고
    aa = std::make_shared<A>();         // std::shared_ptr도 됨.

    QTimer* t = new QTimer();
    connect(t, SIGNAL(timeout()), aa.get(), SLOT(Fn()));
    connect(t, SIGNAL(timeout()), a.data(), SLOT(Fn()));

    t->setInterval(100);
    t->start();
}
```

🏄‍♂️ 그럼 sharedpointer의 this는 어떻게 표현할까?

```cpp
class A : public QWidget, public QEnableSharedFromThis<A>
{
    Q_OBJECT

public:
    A() {}
    ~A() {}

public slots:
    void Fn();
};
```

```cpp
void A::Fn()
{
    int a = 0;

    QTimer* t = new QTimer();

    connect(t, SIGNAL(timeout()), sharedFromThis().data(), SLOT(Fn()));
    
    t->setInterval(100);
    t->start();
}
```