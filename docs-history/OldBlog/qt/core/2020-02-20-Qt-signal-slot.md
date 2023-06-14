---
title: "(Qt) Signal and Slot"
date: 2020-02-20 00:00:00 -0000
---

## 핵심정리

> 전체 구조가 이런식<br>
> `QObject::connect(시그널, 슬럿);`<br>
> 시그널은 보내는 쪽<br>
> 슬럿은 받는 쪽이라 생각하자.<br>
> 예를 들어 설명<br>

```cpp
QObject::connect(&a, SIGNAL(valueChanged(int)), &b, SLOT(setValue(int)));
```

```cpp
QObject::connect(&qnode, SIGNAL(updatePCService(int)), this, SLOT(update_VS_Edit(int)));
// qnode라는 클래스에 updatePCService 시그널 등록
// 본(this) 클래스에 update_VS_Edit 슬럿 등록
```

```cpp
// signal은 아래와 같이 사용됨.
void QNode::topic_VDS_callback(const asv_msgs::OutboardThrottleFeedback &msg)
{
    for(std::vector<asv_msgs::OutboardThrottle>::const_iterator it = msg.outboard_throttles.begin(); it != msg.outboard_throttles.end(); ++it)
    {
        temp_VDS = *it;
    }

    // 여기
    Q_EMIT updateVDS(&temp_VDS);
}
```

```cpp
// *.h 파일에서 시그널을 아래와 같이 등록
Q_SIGNALS:
    void updateVDS(asv_msgs::OutboardThrottle*);
```

```cpp
// *.h 파일에서 슬럿도 아래와 같이 등록
public Q_SLOTS:
        void update_VDS_Edit(asv_msgs::OutboardThrottle* msg);
```

---

## 코드 전체

```cpp
class Counter
{
public:
    Counter() { m_value = 0; }

    int value() const { return m_value; }
    void setValue(int value);

private:
    int m_value;
};
```

```cpp
#include <QObject>

class Counter : public QObject
{
    Q_OBJECT

public:
    Counter() { m_value = 0; }

    int value() const { return m_value; }

public slots:
    void setValue(int value);

signals:
    void valueChanged(int newValue);

private:
    int m_value;
}
```

```cpp
void Counter::setValue(int value)
{
    if(value != m_value) {
        m_value = value;
        emit valueChaneged(value);
    }
}
```

```cpp
Counter a, b;
QObject::connect(&a, SIGNAL(valueChanged(int)), &b, SLOT(setValue(int)));
```

---

## 참고사이트

* [참고사이트](http://doc.qt.io/archives/qt-4.8/signalsandslots.html)
* [참고사이트](https://makersweb.net/qt/940)