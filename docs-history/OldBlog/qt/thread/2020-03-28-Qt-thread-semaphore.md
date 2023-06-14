---
title: "(Qt) semaphore 사용"
permalink: qt/thread/semaphore/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-8)


## semaphore

```cpp
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QStringList data;
	for (int i = 0; i < 100; i++) {
		data.append(QString::number(i));
	}

	QThreadPool* pool = QThreadPool::globalInstance();
	QSemaphore sema(100);

	for (int i = 0; i < data.length();i++)
	{
		worker* w = new worker(&a, &data, &sema, i);        // semaphore를 여기 삽입
		w->setAutoDelete(true);
		pool->start(w);
	}
	pool->waitForDone();

	foreach(QString string, data)
	{
		qInfo() << string;
	}

	return a.exec();
}
```

```cpp
// worker.h
#pragma once

#include <QObject>
#include <qdebug.h>
#include <qthread.h>
#include <qrunnable.h>
#include <qsemaphore.h>

class worker : public QObject, public QRunnable
{
	Q_OBJECT

public:
	explicit worker(QObject *parent = nullptr, QStringList* data = nullptr, QSemaphore* sema = nullptr, int position = -1);
	~worker();

	void run();

signals:

public slots:


private:
	QStringList* data;
	QSemaphore* sema;
	int position;
};
```

```cpp
// worker.cpp
#include "worker.h"


worker::worker(QObject* parent, QStringList* data, QSemaphore* sema, int position) : QObject(parent), QRunnable()
{
	this->sema = sema;
	this->data = data;
	this->position = position;
}

worker::~worker()
{

}

void worker::run()
{
	if (!data || !sema) {
		qInfo() << "Missing pointers!";
		return;
	}

	QString t;
	t.sprintf("%08p", QThread::currentThread());
	sema->acquire(1);
	data->replace(position, QString::number(position) + " - " + t);
	sema->release();

	qInfo() << t << " Finished" << position;
}
```