---
layout: default
title: "QT Event에 관한 몇가지 사실"
parent: (QT)
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🥽 너무 쉬운내용은 생략, 내가 몰랏던 내용위주의 정리

## QObject에 Event를 보내는 방법?

🥽 Qt Object에 Event를 보내는 방법은 크게 **세 가지**가 있다.<br>
🥽 **Windows Native Event**(Qt Event로 Translate됨), **postEvent**, **sendEvent**<br>
🥽 대략적 절차는 아래와 같은데 세 가지 방법 중 하나로 Event를 보내게 된다면 Main Event Loop로 들어가게된다. (여기서 말하는 Main Event Loop는 `QApplication::exec()`의 EventLoop를 의미한다.)<br>
🥽 Main Event Loop는 `QApplication::notify()`를 통해 destination object로 event를 보내준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/event-1.png"/>
</p>

🥽 (추가) `QApplication::notify()` override해보기

```cpp
// application.h
#ifndef APPLICATION_H
#define APPLICATION_H

#include "qapplication.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char** argv);

signals:


    // QCoreApplication interface
public:
    bool notify(QObject * dest, QEvent * event);
};

#endif // APPLICATION_H
```

🥽 하지만 ... OOP관점에서 이렇게 override는 스파게티코드를 만드는 주범이 될 수 있다. 그냥 해당 object에서 event를 처리하게 만드는게 좋음

---

## postEvent와 sendEvent

🎈 `QCoreApplication::postEvent` : Event Loop에 Event를 쌓는다. 즉, 비동기-논블록으로 동작하게 된다. <br> 
🎈 `QCoreApplication::sendEvent` : Event Loop에 Event를 쌓은 후 처리를 대기한다. 즉, 동기로 동작하게 된다.<br>
🎈 이러면 `QCoreApplication::postEvent`를 쓰는 이유가 너무 없지않나? 대부분 내가보낸 Event를 바로 처리해주길 원할텐데?? 👉 `QCoreApplication::postEvent`는 단순 Event Loop에 Event를 쌓기만 하는것은 아니다. 만약 중첩된 Event가 있다면 하나로 통합해주는 등 기능이 있다. 예를 들자면 repaint로 인한 filckering이 발생한다면 `QCoreApplication::postEvent`를 사용할 경우 filckering이 없어질 수 있다.

---

## event와 eventfilter 중 어디가 먼저 잡힐까?

🎨 정답부터 말하자면 `eventfilter`에 event가 먼저 잡히게 된다.<br>
🎨 Qt에서 이렇게 구현해둔 것은 생각해보면 당연한데 만약 실제 object에서 해당 event를 처리하고 싶지않은경우 `eventfilter`를 통해 **reject**이 가능하게 만들기 위해서이다.

---

## QEventLoop를 쓰는이유?

🎀 Main Event Loop에서 **ui thread**를 멈추지 않고 코드를 블록하고 싶을때 사용된다.<br>
🎀 사실 Main Event Loop는 동작하고 있는 상태이기에 마구사용하면 위험하다. 조심해 사용하자(꼭 필요할때만 쓰자, 복잡한 시스템에선 쓰지말자)

