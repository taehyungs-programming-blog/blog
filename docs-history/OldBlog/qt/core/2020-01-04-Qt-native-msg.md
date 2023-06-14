---
title: "(Qt) win native msg 처리"
date: 2020-01-04 00:00:00 -0000
---

### .h

```cpp
#pragma once

#include <QtWidgets/QWidget>
#include <Windows.h>
#include "ui_QtGuiApplication1.h"

class QtGuiApplication1 : public QWidget
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtGuiApplication1Class ui;

	bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
};
```

---

### .cpp

```cpp
#include "QtGuiApplication1.h"

QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}


bool QtGuiApplication1::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
	if (eventType != "windows_generic_MSG")
		return false;

	const MSG* msg = reinterpret_cast<MSG*>(message);

	switch (msg->message) {
	case WM_NCPAINT:
		// do_something
		break;
	}

	return false;
}
```