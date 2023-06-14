---
title: "(Qt) 47. QFileSystemWatcher"
permalink: qt/core/QFileSystemWatcher/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci11-3)

```cpp
#include "watcher.h"

Watcher::Watcher(QObject *parent) : QObject(parent)
{
    fsw.addPath(QDir::currentPath());
    fsw.addPath(QDir::currentPath() + QDir::separator() + "test.txt");

    connect(&fsw,&QFileSystemWatcher::fileChanged, this, &Watcher::fileChanged);
    connect(&fsw,&QFileSystemWatcher::directoryChanged, this, &Watcher::directoryChanged);
}

void Watcher::fileChanged(const QString &path)
{
    qInfo() << "File changed: " << path;
    if(fsw.files().length()) {
        qInfo() << "Files that have changed:";
        foreach(QString file, fsw.files()) {
            qInfo() << file;
        }
    }
}

void Watcher::directoryChanged(const QString &path)
{
    qInfo() << "Directory changed: " << path;
    if(fsw.directories().length()) {
        qInfo() << "Dir changed:";
        foreach(QString dir, fsw.directories()) {
            qInfo() << dir;
        }
    }
}
```