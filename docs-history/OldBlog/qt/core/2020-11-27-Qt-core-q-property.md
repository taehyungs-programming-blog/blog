---
title: "(Qt) q-property 사용하기"
permalink: qt/core/q-property/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-11-27 00:00:00 -0000
last_modified_at: 2020-11-27 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Core
category:
  - q-property
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [참고사이트](https://wiki.qt.io/Qt_Style_Sheets_and_Custom_Painting_Example)
* [참고사이트2](https://doc.qt.io/qt-5/properties.html)

---

```cpp
class QMyWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor myColor READ getmyColor WRITE setmyColor DESIGNABLE true)

    // ...

public:
    QColor getLineColor() const { return m_myColor; }
    void setLineColor( QColor c ) { m_myColor = c; } 

    // ...

private:
    QColor m_myColor;
};
```

```css
QMyWidget {
    qproperty-myColor: yellow;
}
```