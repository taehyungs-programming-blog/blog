---
title: "(Qt) Thread 생성"
permalink: qt/thread/make/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-2)

## 새로운 thread 생성하기

```cpp
static QSharedPointer<QThread> sptr;
void timeout()
{
	if (!sptr.isNull())
	{
		qInfo() << "Time out - stopping thread from" << QThread::currentThread();;
		sptr.data()->quit();
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qInfo() << "App Thread: " << a.thread();
	qInfo() << "Current Thread: " << QThread::currentThread();

	test t;
	qInfo() << "Timer Thread: " << t.thread();
	// App과 같은 thread아래 돌아간다.

	QThread thread;
	t.moveToThread(&thread);        // t라는 QObject의 thread를 변경
	// test t(&a);
	// test가 App의 자녀라면 thread를 옮길 수 없다.(에러가 발생)
	qInfo() << "Timer Thread(moved): " << t.thread();

	t.start();

	qInfo() << "Thread State: " << thread.isRunning();
	// 이대로 실행하면 thread가 죽어있다.(false)

	thread.start();
	// thread를 실행하는 절차가 필요하다.

    QTimer timer;
	timer.singleShot(5000, &timeout);

	return a.exec();
}
```

```cpp
#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QThread>

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);

signals:

public slots:
    void timeout();
    void start();

private:
    QTimer timer;
};

#endif // TEST_H
```

```cpp
#include "test.h"

test::test(QObject *parent) : QObject(parent)
{

}

void test::timeout()
{
    qInfo() << QDateTime::currentDateTime().toString() << " on " << QThread::currentThread();
}

void test::start()
{
    connect(&timer, &QTimer::timeout, this, &test::timeout);
    timer.setInterval(1000);
    timer.start();
}
```