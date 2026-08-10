---
layout: default
title: "size와 sizeHint + widget의 정확한 size는 언제 구할 수 있을까?"
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

* [Clone Code 🌎](https://github.com/EasyCoding-7/qt_exmaple_sizehint)

## sizeHint

🐱‍🏍 헷갈리는 부분! Widget의 size / sizeHint의 차이는 뭔가?<br>
🐱‍🏍 **size** - 현재 Widget의 실제 그려지고 있는 size를 의미한다.<br>
🐱‍🏍 **sizeHint** - Widget이 그려 졌으면하는 size를 의미한다(???) 그려 졌으면하는 이란 무슨 말인가?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/sizehint-1.png"/>
</p>

🐱‍🏍 예를들어 위와 같이 `.ui`파일을 구성했다고 가정해 보자. 파란색 Widget의 Width는 현재 Widget의 Width와 비슷할 것이고 다른 Widget이 추가될 수록 줄어들 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/sizehint-2.png"/>
</p>

🐱‍🏍 결론적으로 **sizeHint**는 Layout에 의해 **권장되는** 사이즈라 할 수있다.

---

## widget의 정확한 size는 언제 구할 수 있을까?

🐱‍🏍 결론부터 말하자면 QWidget의 `showEvent`를 사용하면된다.

```cpp
void CustomQWidget::showEvent(QShowEvent* e)
{
    std::cout << "CustomQWidget::showEvent object name : " << objectName().toStdString() << std::endl;
    std::cout << "CustomQWidget::showEvent object sizeHint : " << sizeHint().width() << " " << sizeHint().height() << std::endl;
    std::cout << "CustomQWidget::showEvent object size : " << size().width() << " " << size().height() << std::endl;

    return QWidget::showEvent(e);
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/sizehint-3.png"/>
</p>

🐱‍🏍 Widget이 show가 되기전 width, height를 알 수 있다.