---
layout: default
title: "Qt Concurrency 정리"
parent: (QT)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://8bitscoding.github.io/qt/)

🔥 Qt Concurrency를 모두 정리할 순 없고, 나름 많이 쓰일것 같은 사항위주로 정리하였다.

---

## QFuture

🔥 큰 설명은 필요없는듯. 예제로 보자.

```cpp
#include <QFuture>
// ...
// QFuture 변수선언
QFuture<void> m_future;

// 아래와 같이 무거운 작업이 있다고 가정하자.
void heavyWork()
{
    qDebug() << "Heavy work running in thread : " << QThread::currentThread();
    for(int i{0} ; i < 1000000001 ; i++){

        if((i%100000) == 0){

            double percentage = ((i/1000000000.0)) * 100;
                       qDebug() << "Percentage : " << QVariant::fromValue(percentage).toInt()
                                << " | Thread : " << QThread::currentThread();
        }
    }
}

// ...


// main
m_future = QtConcurrent::run(heavyWork);
// ... do something
qDebug() << "do something!!";
m_future.waitForFinished();
qDebug() << "Computation done!";
```

🔥 문제점? 

```cpp
// (주의!!) m_future.waitForFinished();를 호출한다면 Main Thread를 잡고 있기에 아래처럼, cancel, pause, resume이 동작하지 않는다!!
void Widget::on_cancelButton_clicked()
{
    m_future.cancel();
}

void Widget::on_pauseButton_clicked()
{
    m_future.pause();
}

void Widget::on_resumeButton_clicked()
{
    m_future.resume();
}
```

---

## QFutureWatcher

🔥 Future의 결과를 받을 방법은 없나? 👉 `QWatcher`

```cpp
#include <QFuture>
#include <QFutureWatcher>
// ...
QFuture<QVector<int>>           m_future;
QFutureWatcher<QVector<int>>    m_watcher;

// ...

connect(&m_watcher,&QFutureWatcher<void>::finished,[=](){
        qDebug() << "---------------------------Computation done----------------------";


        if(!m_watcher.isCanceled()){

            QVector<int> resultVector = m_watcher.result();

            qDebug() << "Number of items in resultVector : " << resultVector.count();

            for( int i{0}; i < resultVector.count(); i++){
                qDebug() << " element [" << i << "] : " << resultVector[i];
            }

        }else{
            qDebug() << "Watcher already canceled ";
        }
    });

// ...

QFuture<QVector<int> > heavyWorkWithReturn()
{
    auto heavyWork = [=](){

        qDebug() << "Heavy work running in thread : " << QThread::currentThread();

        QVector<int> intVector;

        for(int i{0} ; i < 1000000001 ; i++){

            if((i%100000) == 0){
                double percentage = ((i/1000000000.0)) * 100;
                qDebug() << "Heavy work with return : " << QVariant::fromValue(percentage).toInt() ;
                intVector.push_back(QVariant::fromValue(percentage).toInt());
            }
        }
        return  intVector;
    };

    return  QtConcurrent::run(heavyWork);
}

// ...

m_future =  heavyWorkWithReturn();
m_watcher.setFuture(m_future);
```

```cpp
void Widget::on_cancelButton_clicked()
{
    m_watcher.cancel();
}

void Widget::on_pauseButton_clicked()
{
    m_watcher.pause();
}

void Widget::on_resumeButton_clicked()
{
    m_watcher.resume();
}
```

---

## QtConcurrent::map

🔥 이런 Concurrecy는 자료구조 순회를 할때 자주 사용될 것 같은데? 좀 더 쉽게 쓸 순 없나?? 👉 `QtConcurrent::map`

```cpp
#include <QFuture>
#include <QFutureWatcher>
// ...
QList<int>              m_list;        // 순회할 자료구조
QFuture<void>           m_future;
QFutureWatcher<void>    m_futureWatcher;

// ...

connect(&m_futureWatcher, &QFutureWatcher<void>::started,[=](){
    qDebug() << "asynchronous : Work started.";
});

connect(&m_futureWatcher, &QFutureWatcher<void>::finished,[=](){
    qDebug() << "asynchronous : Work done.";
});

connect(&m_futureWatcher, &QFutureWatcher<void>::progressValueChanged,[=](int value){
    qDebug() << "Progress : " << value;
});

// ...

// 이렇게 순회해 주세요
void modify(int &value)
{
    qDebug() << "Modifying " << value << " result : " << value * 10
             << " Thread :" << QThread::currentThread();
    value = value * 10;
}

// ...

m_future = QtConcurrent::map(m_list,modify);
m_futureWatcher.setFuture(m_future);
```

---

## QtConcurrent::filter

🔥 filter를 걸수도 있다.

```cpp
#include <QFuture>
#include <QFutureWatcher>
// ...
int                     m_filterValue;
QList<int>              m_intList;
QFuture<void>           m_future;
QFutureWatcher<void>    m_futureWatcher;

// ...

// 이런식으로 필터를 걸수있다.
auto filter = [=](const int value){

    if(value >= m_filterValue){
        qDebug() << "Thread " << QThread::currentThread() << ". Value " << value << " greater that threshold " <<m_filterValue << " filtering out.";
        return false;
    }
    return true;
};
m_future = QtConcurrent::filter(m_intList,filter);
```