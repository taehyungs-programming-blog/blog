---
title: "(Qt) mutex 사용"
permalink: qt/thread/mutex/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-6)

## mutex

```cpp
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	Counter counter;
	QMutex mutex; (QMutex::Recursive);
	QThreadPool* pool = QThreadPool::globalInstance();

	qInfo() << "Count: " << counter.count();

	for (int i = 0; i < pool->maxThreadCount();i++) {
		worker* w = new worker(&a, &counter, &mutex);       // 여기서 mutex 삽입
		w->setAutoDelete(true);
		pool->start(w);
	}

	pool->waitForDone();

	qInfo() << "Done, the count is: " << counter.count();

	return a.exec();
}
```

```cpp
// worker.h
#pragma once

#include <QObject>
#include <qthread.h>
#include <qrunnable.h>
#include <qdebug.h>
#include <qrandom.h>
#include <qmutex.h>

#include "Counter.h"

class worker : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit worker(QObject *parent = nullptr, Counter* counter = nullptr, QMutex* mutex = nullptr);
	
	void run();

signals:

public slots:


private:
	Counter* counter;
	QMutex* mutex;
};
```

```cpp
// worker.cpp
#include "worker.h"


worker::worker(QObject* parent, Counter* counter, QMutex* mutex) : QObject(parent), QRunnable()
{
	this->counter = counter;
	this->mutex = mutex;
}

void worker::run()
{
	if (!counter) return;

	qInfo() << this << " Starting";

	for (int i = 0; i < 100; i++) {
		// Without our mutex, the count is wildy out of control
		// Only lock for short term durations:

		QMutexLocker locker(mutex);
		counter->increment();
		qInfo() << this << " Count: " << counter->count();
		counter->decrement();
	}

	qInfo() << this << " Finished";
}
```

```cpp
// Counter.h
#pragma once

#include <QObject>
#include <qdebug.h>

class Counter : public QObject
{
	Q_OBJECT

public:
	explicit Counter(QObject *parent = nullptr);
	~Counter();

	void increment();
	void decrement();
	int count();

signals:

public slots:

private:
	int value = 0;
};
```

```cpp
// Counter.cpp
#include "Counter.h"

Counter::Counter(QObject *parent)
	: QObject(parent)
{
}

Counter::~Counter()
{
}


void Counter::increment()
{
	value++;
}

void Counter::decrement()
{
	value--;
}

int Counter::count()
{
	return value;
}
```
