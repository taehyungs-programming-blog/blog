---
title: "(Qt) 11. Groups in QSettings"
permalink: qt/core/Groups-QSettings/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 사용법

```cpp
// 변수 지정
setting->beginGroup(group);
setting->setValue(name, value);
setting->endGroup();

// 변수 호출
setting->value(name).toInt(&ok)
```

---

## 전체코드

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

void saveAge(QSettings *setting,QString group, QString name, int age) {
    setting->beginGroup(group);
    setting->setValue(name,age);
    setting->endGroup();
}

int getAge(QSettings *setting,QString group, QString name) {
    setting->beginGroup(group);

    if(!setting->contains(name)) {
        qWarning() << "Does not contain " << name << " in " << group;
        setting->endGroup();
        return 0;
    }

    bool ok;
    int value = setting->value(name).toInt(&ok);
    setting->endGroup();

    if(!ok) {
        qWarning() << "Failed to convert ot int!!!";
        return 0;
    }

    return value;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setOrganizationName("VoidRealms");
    QCoreApplication::setOrganizationDomain("VoidRealms.com");
    QCoreApplication::setApplicationName("Tester");

    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    //Save the setting
    //settings.setValue("test", 123);

    //Read the setting back
    //qInfo() << settings.value("test").toString();
    //qInfo() << settings.value("test").toInt();

    saveAge(&settings,"people","Bryan",44);
    saveAge(&settings,"beer","twoheart",1);
    saveAge(&settings,"beer","Bryan",11);

    qInfo() << "TwoHeart" << getAge(&settings,"beer","twoheart");
    qInfo() << "Bryan (people)" << getAge(&settings,"people","Bryan");
    qInfo() << "Bryan (beer)" << getAge(&settings,"beer","Bryan");


    return a.exec();
}
```