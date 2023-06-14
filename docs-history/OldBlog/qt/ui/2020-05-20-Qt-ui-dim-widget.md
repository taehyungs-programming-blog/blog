---
title: "Qt : dim(dimming) widget 생성하기"
permalink: qt/ui/dim-widget/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-20 00:00:00 -0000
last_modified_at: 2020-05-20 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - UI
  - dim
  - dimming-widget
excerpt: ""
---

```cpp
// 될 수 있으면 Qt관련 Object는 QPointer를 사용하는게 좋음.
QPointer<dimmingWidget> m_dimmingWidget;
```

```cpp
class dimmingWidget : public QDialog {
Q_OBJECT
public:
    explicit dimmingWidget(QWidget *parent = 0);
    void setCurrentGeo();

private slots:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
};
```

```cpp
dimmingWidget::dimmingWidget(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
    setWindowOpacity(0.5);
    setStyleSheet("background: black");
}

void dimmingWidget::setCurrentGeo()
{
    setGeometry(geometry()->x(), geometry()->y(), geometry()->width(), geometry()->height());
}

bool dimmingWidget::nativeEvent(const QByteArray &eventType,void *message,long *result)
{
    if (eventType != "windows_generic_MSG") {
    return QDialog::nativeEvent(eventType, message, result);
    }

    const MSG *msg = reinterpret_cast<MSG *>(message);

    switch (msg->message)
    {
        case WM_NCCALCSIZE:
        {
            //this kills the window frame and title bar we added with
            //WS_THICKFRAME and WS_CAPTION
            *result = 0;
            return true;
        }
        default: {
            break;
        }
    }
    return QDialog::nativeEvent(eventType, message, result);
}
```