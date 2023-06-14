---
title: "(Qt) Thread 확인"
permalink: qt/thread/check/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 현재 thread명 확인

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>
#include <qdatetime.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qInfo() << "App Thread: " << a.thread();

	return a.exec();
}
```

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>
#include <qdatetime.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qInfo() << "App Thread: " << a.thread();
	qInfo() << "Current Thread: " << QThread::currentThread();

	Q_ASSERT(a.thread() == QThread::currentThread());

	return a.exec();
}
```

출력해 보면 `QCoreApplication a(argc, argv);`의 thread와 main의 thread가 같다<br>
현재 thread가 하나기에 App thread = current thread가 된다.<br>

---

## thread 점검 각종 함수

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>
#include <qdatetime.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qInfo() << "App Thread: " << a.thread();
	qInfo() << "Current Thread: " << QThread::currentThread();

	Q_ASSERT(a.thread() == QThread::currentThread());

	qInfo() << "Running: " << QThread::currentThread()->isRunning();
	qInfo() << "LoopLevel: " << QThread::currentThread()->loopLevel();
	qInfo() << "StackSize: " << QThread::currentThread()->stackSize();

	return a.exec();
}
```

![이미지](/file/image/Qt_Core_AD_1_Image3.png)

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qthread.h>
#include <qdatetime.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	qInfo() << "App Thread: " << a.thread();
	qInfo() << "Current Thread: " << QThread::currentThread();

	Q_ASSERT(a.thread() == QThread::currentThread());

	qInfo() << "Running: " << QThread::currentThread()->isRunning();
	qInfo() << "LoopLevel: " << QThread::currentThread()->loopLevel();
	qInfo() << "StackSize: " << QThread::currentThread()->stackSize();
	qInfo() << "Finished: " << QThread::currentThread()->isFinished();
	qInfo() << "Before: " << QDateTime::currentDateTime().toString();

	QThread::sleep(5);
	qInfo() << "After: " << QDateTime::currentDateTime().toString();

	return a.exec();
}
```

![이미지](/file/image/Qt_Core_AD_1_Image4.png)