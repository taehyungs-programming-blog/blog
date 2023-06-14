---
title: "(Qt) QProcess"
permalink: qt/mthread-ipc/qprocess/                # link 직접 지정
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
  - QProcess
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/QProcess)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>

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
    void on_chooseButton_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void  finished(int exitCode, QProcess::ExitStatus exitStatus);


    private:
        Ui::Widget *ui;
    QString processPath;
    QProcess * mProcess;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    mProcess = new QProcess(this);

    connect(mProcess,&QProcess::started,[](){
        qDebug() << "Process started";

    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_chooseButton_clicked()
{
    processPath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                               "/home",
                                               tr("Programs (*.exe )"));
    if(!processPath.isNull()){
        ui->processPathLineEdit->setText(processPath);
    }


}

void Widget::on_startButton_clicked()
{
    if(!processPath.isNull()){
        mProcess->start(processPath);
    }
}

void Widget::on_stopButton_clicked()
{
    mProcess->close();
}

void Widget::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished, exit code : " << exitCode ;

    qDebug() << "Process finished, exit status : " << exitStatus ;
}
```