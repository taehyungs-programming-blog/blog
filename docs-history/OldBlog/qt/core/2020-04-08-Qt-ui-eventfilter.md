---
title: "qt : eventFilter 사용하기"
permalink: qt/core/eventFilter/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-08 00:00:00 -0000
last_modified_at: 2020-04-08 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
QWidget->installEventFilter(this);
```

```cpp
bool Object::eventFilter(QObject obj, QEvent event)
{
    if (obj == ui->textEdit_room_name)
    {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent keyEvent = static_cast<QKeyEvent>(event);
            // enter key 예외처리
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) return true;	
            return false;
        } 
        else {
        return QDialog::eventFilter(obj, event);
        }
    }
}
```

참고로 `return false;`로 처리되면 내가 작성한 코드가 무시되고<br>
`return true;`로 처리해야 내가 작성한 코드가 적용된다.<br>