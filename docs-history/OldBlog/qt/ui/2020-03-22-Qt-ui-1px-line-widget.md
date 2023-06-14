---
title: "1px line widget"
permalink: qt/ui/1px_line_widget/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-22 00:00:00 -0000
last_modified_at: 2020-03-22 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

특정 위젯아래 라인 그리기

![](/file/image/qt-1px-line-widget.png)

```cpp
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter;
    QPen pen(Qt::red);

    painter.begin(this);    // start drawing

    painter.setPen(pen);
    painter.drawLine(0, ui->draw_here_widget->height(), width(), ui->draw_here_widget->height());  // line

    painter.end();      // close Paint device

}
```

> * [Github](https://github.com/8bitscoding/1px_line_paint)