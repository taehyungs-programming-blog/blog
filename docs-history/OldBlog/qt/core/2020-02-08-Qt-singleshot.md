---
title: "(Qt) singleShot"
date: 2020-01-04 00:00:00 -0000
---

> 특정시간 뒤에 `SLOT`을 호출

```cpp
void MyObject::helloWave()
{
    qDebug() << "hello !";
    QTimer::singleShot(5 * 1000, this, SLOT(helloWave()));
}

// (주의) private slot: void helloWave(); 선언 잊지말것
```

---

> * [참고사이트](https://riptutorial.com/ko/qt/example/29068/qtimer----singleshot-%EA%B0%84%EB%8B%A8%ED%95%9C-%EC%82%AC%EC%9A%A9%EB%B2%95)

