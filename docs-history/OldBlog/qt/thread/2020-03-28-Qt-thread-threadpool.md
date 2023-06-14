---
title: "(Qt) threadpool 사용"
permalink: qt/thread/threadpool/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-1-5)

## threadpool 사용

현재 돌아가는 thread 카운트

```cpp
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QThread::currentThread()->setObjectName("Main");
	QThreadPool* pool = QThreadPool::globalInstance();
	
	qInfo() << pool->maxThreadCount() << " Threads";

	return a.exec();
}
```

![이미지](/file/image/Qt_Core_AD_5_Image2.png)

