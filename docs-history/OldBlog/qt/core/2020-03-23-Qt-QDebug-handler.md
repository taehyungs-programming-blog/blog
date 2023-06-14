---
title: "(Qt) 22. Installing a message handler"
permalink: qt/core/qdebug-handler/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);

void lhandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {

    QString txt;
    switch (type) {
    case QtInfoMsg:
        txt = QString("Info: %1 in %2").arg(msg);
        break;
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    QFile file("log.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream ts(&file);
        ts << QDateTime::currentDateTime().toString() << " - " << txt << " file: " << context.file << " line: " << context.line << "\r\n";
        ts.flush();
        file.close();
     }

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInstallMessageHandler(lhandler);

    qInfo() << "This is a info message";
    qDebug() << "This is a debug message";
    qWarning() << "This is a warning message";
    qCritical() << "This is a critical message";
    qFatal("THIS IS SPARTA!!!");

    return a.exec();
}
```