---
title: "Qt : SubClassing"
permalink: qt/core/subclassing/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-02-16 00:00:00 -0000
last_modified_at: 2020-06-15 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
  - core
category:
  - subclassing
excerpt: ""
---

* 서브 클래싱은 별도의 h/cpp를 만들어야 진행 된다.
* 그리고 cmake 혹은 빌드를 다시 진행 해야 서브클래싱이 된다.

> 다음과 같이 서브클래싱이 가능하다.

```cpp
// mydialog.h
#pragma once

#include <QtWidgets/qdialog.h>

class MyDialog : public QDialog{
public:
	explicit MyDialog(QDialog* parent = nullptr);
};
```

```cpp
// mydialog.cpp
#include "mydialog.h"

MyDialog::MyDialog(QDialog* parent):QDialog(parent) {

}
```

```cpp
#include <QtCore/QCoreApplication>
#include <QtWidgets/qapplication.h>
#include "mydialog.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MyDialog* dialog = new MyDialog;
	dialog->show();

	return app.exec();
}
```

---

## 이벤트 처리하기

```cpp
#pragma once

#include <QtWidgets/qdialog.h>

class MyDialog : public QDialog{
	Q_OBJECT
public:
	explicit MyDialog(QDialog* parent = nullptr);

	void closeEvnet(QCloseEvent* event);
};
```

```cpp
#include "mydialog.h"
#include <QtWidgets/qmessagebox.h>
#include <QtGui/qevent.h>

MyDialog::MyDialog(QDialog* parent) : QDialog(parent) {

}

void MyDialog::closeEvnet(QCloseEvent* event) {
	QMessageBox::StandardButton yesno = QMessageBox::question(this, "title", "yes or no", QMessageBox::Yes | QMessageBox::No);

	if (yesno == QMessageBox::Yes) {
		event->accept();
	}
	else if (yesno == QMessageBox::No) {
		event->ignore();
	}
}
```

