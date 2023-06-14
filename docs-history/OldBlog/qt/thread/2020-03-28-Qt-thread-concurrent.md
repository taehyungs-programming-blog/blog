---
title: "(Qt) QtConcurrent::run"
permalink: qt/thread/concurrent/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-12)


## concurrent

```cpp
#include <QtCore/QCoreApplication>

#include <qlist.h>
#include <qdebug.h>
#include <QtConcurrent/qtconcurrentmap.h>

int do_map(int value)
{
	qInfo() << "Do stuff: " << value;
	int num = value + 10;
	return num;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QList<int> values;
	for (int i = 0; i < 9; i++) {
		values << i;
	}
	qInfo() << "Starting blocked";
	QList<int> updated = QtConcurrent::blockingMapped(values,&do_map);

	qInfo() << updated;
	qInfo() << "Finished";

	return a.exec();
}
```

![](/file/image/Qt_Core_AD_11_Image.png)

---

## QtConcurrent::run

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <QtConcurrent>
#include <qthread.h>
#include <qthreadpool.h>

void loop()
{
	for (int i = 0; i < 10; i++)
	{
		qInfo() << "Loop" << i << "on: " << QThread::currentThread();
	}
}

void test()
{
	qInfo() << "Test on: " << QThread::currentThread();
	loop();
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QThread::currentThread()->setObjectName("Main");

	QThreadPool pool;
	QFuture<void> future = QtConcurrent::run(&pool, test);
	qInfo() << "Main on: " << QThread::currentThread();
	loop();

	return a.exec();
}
```

![](/file/image/Qt_Core_AD_12_Image.png)