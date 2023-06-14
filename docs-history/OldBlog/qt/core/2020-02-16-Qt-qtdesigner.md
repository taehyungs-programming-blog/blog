---
title: "(Qt) qtdesigner이용 .ui파일 및 .h파일 생성하기"
date: 2020-02-16 00:00:00 -0000
---

## qtdesigner를 이용하여 .ui파일 만들기

> 모든 위젯작업을 코드상에서 하기는 힘들다.<br>
> designer를 이요해보자.<br>

![](/file/image/Qt_8_Image_01.png)

![](/file/image/Qt_8_Image_02.png)

> UI는 다음과 같은 방식으로 편집이 가능하다.

---

## .ui파일 IDE에서 불러오기

> uic.exe를 통해서 .h파일로 뽑아낸다.<br>
> uic.exe의 경로는 보통 `C:\Windows\System32\cmd.exe`이고<br>

```s
& uic uiname.ui -o uiname.h
```

> 위 명령어를 입력시 uiname.h파일로 뽑아내 준다.

---

## 생성된 .h파일 적용하기

```cpp
// LoginWidget.h
#pragma once
#include <qwidget.h>
#include "ui_test.h"

class LoginWidget : public QWidget, public Ui::loginWidget
{
	Q_OBJECT
public:
	explicit LoginWidget(QWidget* parent = nullptr);
};
```

```cpp
// LoginWidget.cpp
#include <qwidget.h>
#include "LoginWidget.h"

LoginWidget::LoginWidget(QWidget* parent) : QWidget(parent)
{
	setupUi(this);
}
```

```cpp
// main
#include <QtWidgets/qapplication.h>
#include "LoginWidget.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	LoginWidget* dialog = new LoginWidget;
	dialog->show();


	return app.exec();
}
```