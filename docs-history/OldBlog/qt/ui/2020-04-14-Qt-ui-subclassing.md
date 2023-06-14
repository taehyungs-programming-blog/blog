---
title: "Qt : Creator Promoted to (Subclassing) 사용하기"
permalink: qt/ui/subclassing/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-04-25 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

* [Github](https://github.com/8bitscoding/qtsubclassing)

Qt Creator에서 Subclassing을 지정할 수 있다.

![](/file/image/qt-ui-subclassing01.png)

이후 클래스를 아래와 같이 생성

```cpp
// MyPushButton.h
#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    MyPushButton(QWidget *parent = nullptr) : QPushButton(parent) {}
};

#endif // MYPUSHBUTTON_H
```

```cpp
// Ui_Dialog.h
#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include "mypushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    MyPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(800, 600);
        pushButton = new MyPushButton(Dialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(60, 60, 271, 161));
        pushButton_2 = new QPushButton(Dialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(400, 60, 291, 151));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        pushButton->setText(QCoreApplication::translate("Dialog", "MyPushButton", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Dialog", "OriginalPushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
```