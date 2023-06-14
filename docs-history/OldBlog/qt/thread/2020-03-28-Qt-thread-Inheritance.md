---
title: "(Qt) Thread 상속"
permalink: qt/thread/Inheritance/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-3)


## qthread 상속

```cpp
// test.h
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>

class test : public QThread
{
	Q_OBJECT

public:
	explicit test(QObject *parent = nullptr);
	~test();

signals:

public slots:
	void quit();

protected:
	void run();

private:
	bool ok;

};
```

```cpp
// test.cpp
#include "test.h"

test::test(QObject *parent)
	: QThread(parent)
{
}

test::~test()
{
}

void test::quit()
{
	ok = false;
	QThread::quit();
}


void test::run()
{
	ok = true;
	for (int i = 0; i < 1000; i++)
	{
		qInfo() << i;
		this->sleep(1);
		// this is bad because now code become unpredictable
		// control can come from anywhere

		if (!ok) break;
	}

	qInfo() << "Finished";
}
```

```cpp
static QSharedPointer<test> sptr;

void timeout()
{
	if (!sptr.isNull()) {
		qInfo() << "Timerout Stopping Thread";
		sptr.data()->quit();
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	test thread;
	sptr.reset(&thread);
	thread.start();

	QTimer timer;
	timer.singleShot(5000, &timeout);

	return a.exec();
}
```

---

### Example 2

> * [Github](https://github.com/8bitscoding/qtca-1-4)

```cpp
// main.cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>

#include "manager.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QThread::currentThread()->setObjectName("Main");

	manager m;
	m.start();

	return a.exec();
}
```

```cpp
// manager.h
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>
#include <qlist.h>

#include "counter.h"

class manager : public QObject
{
	Q_OBJECT

public:
	explicit manager(QObject *parent = nullptr);
	~manager();
	void start();

signals:

public slots:
	void started();
	void finished();

private:
	QList<QThread*> threads;
};
```

```cpp
// manager.cpp
#include "manager.h"

manager::manager(QObject *parent)
	: QObject(parent)
{
	for (int i = 0; i < 5; i++) {
		QThread* thread = new QThread(this);
		thread->setObjectName("Thread " + QString::number(i));
		qInfo() << "Created: " << thread->objectName();

		connect(thread, &QThread::started, this, &manager::started);
		connect(thread, &QThread::finished, this, &manager::finished);
	
		threads.append(thread);
	}
}

manager::~manager()
{
}


void manager::started()
{
	QThread* thread = qobject_cast<QThread*>(sender());

	if (!thread) return;

	qInfo() << "Started: " << thread->objectName();
}

void manager::finished()
{
	QThread* thread = qobject_cast<QThread*>(sender());

	if (!thread) return;

	qInfo() << "Finished: " << thread->objectName();
}

void manager::start()
{
	qInfo() << "Starting ... ";

	foreach(QThread * thread, threads) {
		qInfo() << "Starting: " << thread->objectName();
		counter* c = new counter();
		c->moveToThread(thread);

		connect(thread, &QThread::started, c, &counter::start);
		thread->start();
	}
}
```

```cpp
// counter.h
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>
#include <qrandom.h>

class counter : public QObject
{
	Q_OBJECT

public:
	explicit counter(QObject *parent = nullptr);
	~counter();

signals:

public slots:
	void start();

};
```

```cpp
// counter.cpp
#include "counter.h"

counter::counter(QObject *parent)
	: QObject(parent)
{
}

counter::~counter()
{
}

void counter::start()
{
	for (int i = 0; i < 20; i++)
	{
		qInfo() << QThread::currentThread()->objectName() << " = " << i;
		auto value = static_cast<unsigned long>(QRandomGenerator::global()->bounded(500));
		QThread::currentThread()->msleep(value);
	}

	QThread::currentThread()->quit();
}
```