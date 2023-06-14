---
title: "(Qt) QFutureWatcher"
permalink: qt/thread/watcher/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-14)


## QFutureWatcher

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlist.h>
#include <QtConcurrent>

void multiply(int &value)
{
	qInfo() << "Called: " << value;
	value = value * 10;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QList<int> list;

	for (int i = 0; i < 100; i++) {
		list.append(i);
	}

	QFutureWatcher<void> watcher;
	QFuture<void> future = QtConcurrent::map(list, &multiply);
	watcher.setFuture(future);
	qInfo() << "Back in main!";

	watcher.waitForFinished();
	qInfo() << "done!";
	qInfo() << list;

	return a.exec();
}
```

![](/file/image/Qt_Core_AD_14_Image.png)

---

## QFuture getting values

```cpp
#include <QtCore/QCoreApplication>

#include <qdebug.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlist.h>
#include <QtConcurrent>

int multiply(int &value)
{
	qInfo() << "Called: " << value;
	int num = value * 10;
    return num;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QList<int> list;

	for (int i = 0; i < 100; i++) {
		list.append(i);
	}

	QFutureWatcher<int> watcher;
	QFuture<int> future = QtConcurrent::mapped(list, &multiply);
	watcher.setFuture(future);
	qInfo() << "Back in main!";

	watcher.waitForFinished();
	qInfo() << "done!";
	qInfo() << list;

	return a.exec();
}
```