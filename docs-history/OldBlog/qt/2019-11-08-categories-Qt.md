---
title: "Qt"
permalink: qt/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-15 00:00:00 -0000
last_modified_at: 2020-05-09 00:00:00 -0000
sidebar:
  title: "Qt"
  nav: qt
tag:
  - qt
category:
  - category
classes: wide
excerpt: "Qt와 관련된 지식을 정리한 홈페이지 입니다."
header:
  teaser: /file/image/qt-page-teaser.gif
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
  actions:
    - label: "Qt HomePage"
      url: "https://www.qt.io/"
---

## Shot Know-How!

* [qpinter로 Text를 그리더라도 qss에 폰트가 지정이 되어 있다면 qss폰트가 우선한다]()

---

## Telegram Qt UI

* [Build](/opens/telegram/build/) : 만약 UI만 확인할거면 꼭 Build하지 않아도 된다. (코드만 분석하면 된다.)
* [DPI Setting](/opens/telegram/dpiset/)
* [Style을 어떻게 입히는가?](/opens/telegram/style/)
* [Ripple Button](/opens/telegram/ripple-btn/)

![](/file/image/telegram-ripple-btn-1.gif)

* [Fade Widget](/opens/telegram/fade-widget/)

![](/file/image/telegram-fade-widget-1.gif)

* [Slide Widget](/opens/telegram/slide-widget/)

![](/file/image/telegram-slide-widget-1.gif)

---

## Qt with OpenCV

* [Qt with OpenCV](/qt/opencv/1/)

## QML

* [Custom QML Document](https://doc.qt.io/qt-5/qtquickcontrols2-customize.html#customizing-a-control)

* [Qt QML](/qt/qml/)

### Interfacing to C++ 

* [String List Model From C++](/qt/qml/1/)

![](/file/image/qml-cpp-interfacing-1-1.png)

* [Object List Model From C++](/qt/qml/2/)

![](/file/image/qml-cpp-interfacing-2-1.png)

* [Editable Object List Model From C++](/qt/qml/3/)

![](/file/image/qml-cpp-interfacing-3-1.png)

* [Editable Object List Model From C++(QPROPERTY)](/qt/qml/4/)

* [Abstract Item Model From C++](/qt/qml/5/)

![](/file/image/qml-cpp-interfacing-5-1.png)

* [Decoupling Data Source and Model](/qt/qml/6/)

![](/file/image/qml-cpp-interfacing-6-1.png)

* [Using Object List Properties](/qt/qml/7/)

![](/file/image/qml-cpp-interfacing-7-1.png)

### Custom Visual Types

* [Custom Painted Types : Images-Demo](/qt/qml/8/)

![](/file/image/qml-cpp-interfacing-8-1.png)

* [Custom Painted Types : Draw a Bar chart](/qt/qml/9/)

![](/file/image/qml-cpp-interfacing-9-1.png)

* [Enhance Stair Chart with List Properties](/qt/qml/10/)

![](/file/image/qml-cpp-interfacing-10-1.png)

* [Pure QML Custom Shapes](/qt/qml/11/)

![](/file/image/qml-cpp-interfacing-11-1.png)

* [Rest Api Client](/qt/qml/12/)

![](/file/image/qml-cpp-interfacing-12-1.png)

### Digging QML From C++

* [Loading QML Objects from C++](/qt/qml/13/)

* [Custom FootBalleam Project](/qt/qml/14/)

![](/file/image/qml-cpp-interfacing-14-1.png)

* [Parsing a Custom C++ Type from QML](/qt/qml/15/)

* [Loading Visual Types with QQuickView](/qt/qml/16/)

* [QqmlApplicationEngine : The Convenience](/qt/qml/17/)

* [Invoking QML Methods](/qt/qml/18/)

* [Handling a QML Signal in C++](/qt/qml/19/)

---

## Qt Settings

qt ide settings 관련

* [Qt_KMS](/qt/settings/kms/) : 키보드 단축키 세팅

---

## Qt Desing Patterns

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns)

* [SOLID(Single responsibility principle)](/qt/desing-patterns/solid/)
* [각종 principle](/qt/desing-patterns/kis/) : kis, dry ...
* [Qt Property System](/qt/desing-patterns/property-system/)
* [Code deprecation](/qt/desing-patterns/code-deprecation/) : 빌드에서 코드막기

* [Qt Thread Patterns](/qt/desing-patterns/thread-patterns/)
* [Memory Pattern](/qt/desing-patterns/memory-patterns/)
* [Signals and Slots](/qt/desing-patterns/signals-slots/)
* [library patterns](/qt/desing-patterns/library/)

* [Template Patterns](/qt/desing-patterns/template/)
* [Creational Patterns](/qt/desing-patterns/creational/)

---

## Multi-Threading, IPC

우선 Qt Thread에 대해 잘 모른다면 아래의 Thread를 먼저 읽어보자.

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example)

Thread

* [Create Thread](/qt/mthread-ipc/create-thread/)
* [Move To Thread](/qt/mthread-ipc/move-to-thread/)
* [Subclass QThread](/qt/mthread-ipc/subclass-thread/)

* [QThread asynchronous(Create Thread)](/qt/mthread-ipc/a-create-thread/)
* [QThread asynchronous(Move To Thread)](/qt/mthread-ipc/a-move-to-thread/)
* [QThread asynchronous(Subclass QThread)](/qt/mthread-ipc/a-subclass-thread/)

* [ThreadPool and QRunnable](/qt/mthread-ipc/threadpool-runnable/)
* [ThreadPool and QRunnable - Sending feedback to ui](/qt/mthread-ipc/threadpool-runnable-2/)
* [ThreadPool and QRunnable - Async Code](/qt/mthread-ipc/threadpool-runnable-3/)

* [Custom Type Signal Parameters](/qt/mthread-ipc/custom-type-signal/)

Thread Synchronization

* [Mutex](/qt/mthread-ipc/mutex/)
* [Mutex Shared Variable](/qt/mthread-ipc/mutex-shared-variable/) : Mutex Example이라 보면 됨
* [ReadWrite Lock](/qt/mthread-ipc/readwrite-lock/) : Mutex이용 Thread간 Timing 조절

* [Semaphores](/qt/mthread-ipc/semaphores/) : Semaphore를 왜 쓰는지 Exmaple

* [WaitConditions](/qt/mthread-ipc/waitconditions/) : Mutex를 잡고 기다리는 방법
* [WaitConditions Pause Resume](/qt/mthread-ipc/waitconditions-pause-resume/) : WaitConditions을 이용해 Thread Pause, Resume 하는 방법

Thread Safety and Reentrancy(정보수집)

* [Cross Thread Signal and Slots - 1](/qt/mthread-ipc/crossthread-ss-1/) : signal and slot으로 두 object연결
* [Cross Thread Signal and Slots - 2](/qt/mthread-ipc/crossthread-ss-2/) : 
* [Cross Thread Signal and Slots - 3](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/CrossThreadSignalSlots3) : 코드만 넣음

* [Slot In QThread Subclass](/qt/mthread-ipc/slot-qthread/) : thread안에서 signal slot 연결

Qt Concurrent

* [run synchronous](/qt/mthread-ipc/run-synchronous/) : `QFuture` 사용하기(해당 쓰레드를 점유)
* [Asynchronous Return values](/qt/mthread-ipc/async-return-val/) : `QFuture`, `QFutureWatcher` 등록 방법

* [Concurrent Map](/qt/mthread-ipc/concurrent-map/)
* [Concurrent Mapped](/qt/mthread-ipc/concurrent-mapped/)

* [Concurrent Filter](/qt/mthread-ipc/concurrent-filter/)
* [Concurrent Filtered](/qt/mthread-ipc/concurrent-filtered/)
* [Concurrent Filter Reduce](/qt/mthread-ipc/concurrent-filter-reduce/)
* [Future Synchronizer](/qt/mthread-ipc/future-sync/)

QProcess

* [QProcess](/qt/mthread-ipc/qprocess/) : 새로운 프로세스(콘솔) 실행

IPC

* [SharedMemory](/qt/mthread-ipc/shared-memory/)

* [TcpClient](/qt/mthread-ipc/tcpclient/)
* [TcpServer](/qt/mthread-ipc/tcpserver/)

* [FileConsumer](/qt/mthread-ipc/fileconsumer/)
* [FileProducer](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/FileConsumer)

Dbus

당장 필요성을 못느껴서 정리하지 않음 코드만 확인

* [DBus](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/DBus)

* [ClientServer](/qt/mthread-ipc/clientserver/) : DBus를 통한 IPC

## Thread

> * [Qt Thread](/qt/thread/)

* [Q. std thread냐 QThread냐?]() : 
  - Thread - 1의 waitcondition예제를 보면 알겠지만 Signal and Slot의 처리에 있어 QThread가 유리하다, manager에서 Thread의 함수 호출을 쉽게 할 수 있다

* [Thread 세부 정리](/qt/thread/)
* [Qt Thread - 1](/qt/thread/theorem1/)
* [Qt Thread - 2](/qt/thread/theorem2/) : queue condition에 따라서 어떻게 동작하는지 보여준다.

## Core

* [Qt Core](/qt/core/)

* [QPixmap Rescale]()

> * [참고사이트](https://stackoverflow.com/questions/8211982/qt-resizing-a-qlabel-containing-a-qpixmap-while-keeping-its-aspect-ratio)

```cpp
QPixmap p; // load pixmap
// get label dimensions
int w = label->width();
int h = label->height();

// set a scaled pixmap to a w x h window keeping its aspect ratio 
label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
```

**스마트 포인터**

* [QPointer](/qt/core/QPointer/) : Qt용 포인터
* [QScopedPointer](/qt/core/QScopedPointer/) : 범위 내에서 delete를 하지 않아도 되는 포인터
* [QSharedPointer](/qt/core/QSharedPointer/) : 참조자를 체크하는 shared pointer

## UI

* [Qt UI](/qt/ui/)

* [Qt HWND 얻기]()

```cpp
HWND hwnd = HWND(ui->preview->winId());
```

* [QListWidgetItem 사용](/cpp/qt/qlistwidgetitem)

![](/file/image/qt-gdi-s6-52-image-1.png)

## Etc

* [Qt Etc](/qt/etc/)

* [Qt Creator에서 PlugIn을 못찾는 다면?]() : `QT_PLUGIN_PATH`를 환경변수로 지정해 줘야함. [참고 링크](https://forum.qt.io/topic/82643/application-can-t-start-because-could-not-find-or-load-qt/23)