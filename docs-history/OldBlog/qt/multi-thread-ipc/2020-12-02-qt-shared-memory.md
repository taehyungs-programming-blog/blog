---
title: "(Qt) SharedMemory"
permalink: qt/mthread-ipc/shared-memory/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-02 00:00:00 -0000
last_modified_at: 2020-12-02 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Multi Thread
category:
  - shared memory
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/SharedMemory)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSharedMemory>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_loadImageButton_clicked();

    void on_loadShareMemoryButton_clicked();

    void detach();

private:
    Ui::Widget *ui;
    QSharedMemory sharedMemory;

};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFileDialog>
#include <QBuffer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    sharedMemory("SharedMemoryDemoApp")
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_loadImageButton_clicked()
{
    if (sharedMemory.isAttached()){
        qDebug() << "Memory is attached, detaching";
        detach();
    }

    ui->imageLabel->setText(tr("Select an image file"));
    QString fileName = QFileDialog::getOpenFileName(nullptr, QString(), QString(),
                                                    tr("Images (*.png *.xpm *.jpg)"));
    QImage image;
    if (!image.load(fileName)) {
        ui->imageLabel->setText(tr("Selected file is not an image, please select another."));
        return;
    }

    ui->imageLabel->setPixmap(QPixmap::fromImage(image));


    // load into shared memory
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << image;
    int size =  buffer.size();


    qDebug() << "Size : " << size;
    if (!sharedMemory.create(size)) {
        ui->imageLabel->setText(tr("Unable to create shared memory segment."));
        qDebug() << sharedMemory.errorString();
        qDebug()  << "Is attached : " << sharedMemory.isAttached();
        return;
    }

    sharedMemory.lock();

    char *to = (char*)sharedMemory.data();
    const char *from = buffer.data().data();

    memcpy(to,from,qMin(sharedMemory.size(), size));

    sharedMemory.unlock();

}

void Widget::on_loadShareMemoryButton_clicked()
{

    if (!sharedMemory.attach()) {
        ui->imageLabel->setText(tr("Unable to attach to shared memory segment.\n" \
                                   "Load in the data first."));
        return;
    }


    QBuffer buffer;
    QDataStream in(&buffer);
    QImage image;

    sharedMemory.lock();

    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> image;
    sharedMemory.unlock();

    sharedMemory.detach();

    ui->imageLabel->setPixmap(QPixmap::fromImage(image));

}

void Widget::detach()
{
    if(!sharedMemory.detach()){
        ui->imageLabel->setText("Can not detach from the shared memory");
    }
}
```