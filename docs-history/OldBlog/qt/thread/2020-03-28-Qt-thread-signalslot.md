---
title: "(Qt) thread signal and slot"
permalink: qt/thread/signalslot/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-10)


## Signal and Slot

여기서 하고싶은 말은 AutoConnection은 QueuedConnection, DirectConnection 둘 중 하나를 사용하는데 OS에 따라 다르게 실행될 수 있다.<br>
따라서 시스템의 안정성을 위해서 QueuedConnection, DirectConnection 둘 중 하나를 지정해 주는 것이 좋다.<br>

* QueuedConnection : Slots 함수를 등록한 객체의 Queue에 넣어서 객체의 이벤트 루프를 통해 실행(queue에 넣기에 실행의 순서를 보장할 순 없다.)
* DirectConnection : Signal을 emit한 객체의 쓰레드에서 실행

우선 기본코드는 아래와 같다.

```cpp
// main.cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>

#include "consumer.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QThread::currentThread()->setObjectName("Main thread");
	qInfo() << "Application started on " << QThread::currentThread();

	consumer c;
	c.start();

	return a.exec();
}
```

```cpp
// consumer.h
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>

#include "timer.h"

class consumer : public QObject
{
	Q_OBJECT

public:
	explicit consumer(QObject *parent = nullptr);
	~consumer();

signals:

public slots:
	void timeout();
	void start();
	void finished();

private:
	QThread thread;
	timer m_timer;
};
```

```cpp
// consumer.cpp
#include "consumer.h"

consumer::consumer(QObject *parent)
	: QObject(parent)
{
	qInfo() << "Created on " << QThread::currentThread();
	thread.setObjectName("Timer Thread");

	/*
	Qt::AutoConnection - run on both
	Qt::QueuedConnection - run on main thread
	Qt::DirectConnection - run on thread
	Qt::BlockingQueuedConnection - blocks
	Qt::uniqueConnection - combined with others
	*/

	connect(&m_timer, &timer::timeout, this, &consumer::timeout);
	connect(&thread, &QThread::started, &m_timer, &timer::started);
	connect(&thread, &QThread::finished, this, &consumer::finished);
}

consumer::~consumer()
{
}


void consumer::timeout()
{
	qInfo() << "Consumer timeout on " << QThread::currentThread();
	thread.quit();
}

void consumer::start()
{
	qInfo() << "Consumer start on " << QThread::currentThread();
	m_timer.setInternval(500);
	m_timer.moveToThread(&thread);
	thread.start();
}

void consumer::finished()
{
	qInfo() << "Consumer finished on " << QThread::currentThread();
}
```

```cpp
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>

class timer : public QObject
{
	Q_OBJECT

public:
	explicit timer(QObject *parent = nullptr);
	~timer();

	void setInternval(int value);

signals:
	void timeout();

public slots:
	void started();

private:
	int interval = -1;
};
```

```cpp
#include "timer.h"

timer::timer(QObject *parent)
	: QObject(parent)
{
}

timer::~timer()
{
}


void timer::setInternval(int value)
{
	this->interval = value;
}

void timer::started()
{
	qInfo() << "Timer started on " << QThread::currentThread();
	this->thread()->msleep(this->interval);
}
```

![](/file/image/Qt_Core_AD_10_Image.png)


---

Connect의 옵션을 변경하면서 해본다.

### Qt::AutoConnection

```cpp
connect(&m_timer, &timer::timeout, this, &consumer::timeout);
connect(&thread, &QThread::started, &m_timer, &timer::started);
connect(&thread, &QThread::finished, this, &consumer::finished);
```

![](/file/image/Qt_Core_AD_10_Image.png)

---

### Qt::QueuedConnection

```cpp
connect(&m_timer, &timer::timeout, this, &consumer::timeout, Qt::QueuedConnection);
connect(&thread, &QThread::started, &m_timer, &timer::started, Qt::QueuedConnection);
connect(&thread, &QThread::finished, this, &consumer::finished, Qt::QueuedConnection);
```

![](/file/image/Qt_Core_AD_10_Image2.png)

---

### Qt::DirectConnection

```cpp
connect(&m_timer, &timer::timeout, this, &consumer::timeout, Qt::DirectConnection);
connect(&thread, &QThread::started, &m_timer, &timer::started, Qt::DirectConnection);
connect(&thread, &QThread::finished, this, &consumer::finished, Qt::DirectConnection);
```

![](/file/image/Qt_Core_AD_10_Image3.png)

---

### Qt::BlockingQueuedConnection

```cpp
connect(&m_timer, &timer::timeout, this, &consumer::timeout, Qt::BlockingQueuedConnection);
connect(&thread, &QThread::started, &m_timer, &timer::started, Qt::BlockingQueuedConnection);
connect(&thread, &QThread::finished, this, &consumer::finished, Qt::BlockingQueuedConnection);
```

![](/file/image/Qt_Core_AD_10_Image4.png)

---

### Qt::uniqueConnection

```cpp
connect(&m_timer, &timer::timeout, this, &consumer::timeout, Qt::UniqueConnection);
connect(&thread, &QThread::started, &m_timer, &timer::started, Qt::UniqueConnection);
connect(&thread, &QThread::finished, this, &consumer::finished, Qt::UniqueConnection);
```

![](/file/image/Qt_Core_AD_10_Image5.png)