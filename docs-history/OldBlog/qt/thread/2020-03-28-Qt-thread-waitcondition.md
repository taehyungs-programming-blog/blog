---
title: "(Qt) waitcondition 사용"
permalink: qt/thread/waitcondition/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-9)

## waitcondition

```cpp
// main.cpp
#include <QtCore/QCoreApplication>

#include "manager.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	manager m_manager;
	m_manager.start();

	return a.exec();
}
```

```cpp
// manager.h
#pragma once

#include <QObject>
#include <qmutex.h>
#include <qrandom.h>
#include <qthread.h>
#include <qdebug.h>
#include <qlist.h>
#include <qwaitcondition.h>

#include "Producer.h"
#include "consumer.h"

class manager : public QObject
{
	Q_OBJECT

public:
	explicit manager(QObject *parent = nullptr);
	~manager();

signals:

public slots:
	void start();
	void ready();

private:
	QList<int> data;
	QMutex mutex;
	QThread producerThread;
	QThread consumerThread;
	QWaitCondition condition;
	Producer producer;
	consumer m_consumer;
};
```

```cpp
// manager.cpp
#include "manager.h"

manager::manager(QObject *parent)
	: QObject(parent)
{
	connect(&producerThread, &QThread::started,&producer,&Producer::start,Qt::QueuedConnection);
	connect(&producer, &Producer::ready, this, &manager::ready, Qt::QueuedConnection);
	connect(&consumerThread, &QThread::started, &m_consumer, &consumer::start, Qt::QueuedConnection);

	producerThread.setObjectName("Producer Thread");
	consumerThread.setObjectName("consumer Thread");
	this->thread()->setObjectName("Main Thread");

	producer.moveToThread(&producerThread);
	m_consumer.moveToThread(&consumerThread);
}

manager::~manager()
{
}


void manager::start()
{
	producer.setMutex(&mutex);
	producer.setData(&data);
	m_consumer.setMutex(&mutex);
	m_consumer.setData(&data);
	m_consumer.setCondition(&condition);

	producerThread.start();
	consumerThread.start();
}

void manager::ready()
{
	qInfo() << "Data is ready" << this->thread();
	condition.wakeAll();
}
```

```cpp
// producer.h
#pragma once

#include <QObject>
#include <qmutex.h>
#include <qrandom.h>
#include <qthread.h>
#include <qdebug.h>
#include <qlist.h>

class Producer : public QObject
{
	Q_OBJECT

public:
	explicit Producer(QObject *parent = nullptr);
	~Producer();

	void setData(QList<int>* data);
	void setMutex(QMutex* mutex);

signals:
	void ready();


public slots:
	void start();

private:
	QList<int>* data;
	QMutex* mutex;
};
```

```cpp
// producer.cpp
#include "Producer.h"

Producer::Producer(QObject *parent)
	: QObject(parent)
{
}

Producer::~Producer()
{
}


void Producer::setData(QList<int>* data)
{
	this->data = data;
}

void Producer::setMutex(QMutex* mutex)
{
	this->mutex = mutex;
}

void Producer::start()
{
	do {
		int value = QRandomGenerator::global()->bounded(1000);
		mutex->lock();
		data->append(value);
		if (data->length() >= 100) emit ready();
		mutex->unlock();
	} while (true);
}
```

```cpp
// consumer.h
#pragma once

#include <QObject>
#include <qmutex.h>
#include <qrandom.h>
#include <qthread.h>
#include <qdebug.h>
#include <qlist.h>
#include <qwaitcondition.h>

class consumer : public QObject
{
	Q_OBJECT

public:
	explicit consumer(QObject *parent = nullptr);
	~consumer();

	void setData(QList<int>* data);
	void setMutex(QMutex* mutex);
	void setCondition(QWaitCondition* condition);

signals:
	

public slots:
	void start();

private:
	QList<int>* data;
	QMutex* mutex;
	QWaitCondition* condition;
};
```

```cpp
// consumer.cpp
#include "consumer.h"

consumer::consumer(QObject *parent)
	: QObject(parent)
{
}

consumer::~consumer()
{
}


void consumer::setData(QList<int>* data)
{
	this->data = data;
}

void consumer::setMutex(QMutex* mutex)
{
	this->mutex = mutex;
}

void consumer::setCondition(QWaitCondition* condition)
{
	this->condition = condition;
}

void consumer::start()
{
	qInfo() << "Staring consumer on: " << this->thread();

	do
	{
		qInfo() << "Consuming on: " << this->thread();

		mutex->lock();

		data->clear();

		// pause
		condition->wait(mutex);

		mutex->unlock();
	} while (true);
}
```
