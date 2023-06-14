---
title: "Qt : TextEdit 한글 타이핑 도중 입력받기 : QEvent::InputMethod 활용"
permalink: qt/ui/inputmethod/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-14 00:00:00 -0000
last_modified_at: 2020-04-14 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

TextEdit에서 아래 그림과 같이 한글입력 중에는 텍스트의 length가 구해지지 않는다.

![](/file/image/inputmethod.png)

이럴경우 EventFilter를 활용해서 inputmethod를 직접 컨트롤 해야한다.

```cpp
ui.m_TextEdit->installEventFilter(this);

// ...

bool QtProject::eventFilter(QObject obj, QEvent event)
{
    if (obj == ui.ui.m_TextEdit)
    {
        if (event->type() == QEvent::KeyPress) {
        QKeyEvent keyEvent = static_cast<QKeyEvent>(event);
        // enter key 예외처리
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) return true;
    } else if (event->type() == QEvent::InputMethod) {
        QInputMethodEvent * keyEvent = static_cast<QInputMethodEvent*>(event);
        QString s = keyEvent->preeditString();
        // 여기에서 입력중인 텍스트가 들어온다.
        if (s.isEmpty()) {
            // do something
        } else {
            // do something
        }
    }
    return false;
}
else {
    return QDialog::eventFilter(obj, event);
}
}
```