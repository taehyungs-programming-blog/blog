---
title: "(Qt) 42. Application plugins"
permalink: qt/core/appplugin/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-3-6)

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QPluginLoader>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include "../Myplugin/plugin.h"

QStringList getPlugins(QString path)
{
    QStringList filter;
    filter << "*.dll" << "*.so" << "*.dylib";
    QDir dir(path);
    QFileInfoList list = dir.entryInfoList(filter);
    QStringList plugins;

    foreach (QFileInfo file, list) {
        plugins.append(file.filePath());
        //Mac - if(!file.isSymLink()) plugins.append(file.filePath());
    }

    return plugins;
}

void testPlugins(QStringList plugins)
{
    foreach (QString file,plugins)
    {
        qInfo() << "Loading: " << file;

        QPluginLoader loader(file);
        if(!loader.load())
        {
            qInfo() << "Error: " <<  loader.fileName() << " Error: " << loader.errorString();
            continue;
        }

        qInfo() << "Loaded: " << loader.fileName();
        Plugin* plugin = qobject_cast<Plugin*>(loader.instance());

        if(plugin)
        {
            plugin->test("Bryan");
        }
        else
        {
            qInfo() << "Could not cast: " << loader.fileName();
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDir dir = QDir::current();
    dir.cdUp();
    dir.cd("Myplugin"); // Plugins
    qInfo() << "Path: " << dir.path();

    QStringList plugins = getPlugins(dir.path());
    qInfo() << "Plugins: " << plugins;

    testPlugins(plugins);
    qInfo() << "Done!";

    return a.exec();
}
```