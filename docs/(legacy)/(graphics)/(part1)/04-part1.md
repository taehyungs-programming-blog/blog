---
layout: default
title: "04. 원 그리기"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이미지 좌표계만 사용

* [Clone Code 🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/05)

```cpp
bool IsInside(const glm::vec2 &x)
{
    // 가장 간단한 방법으로 구현

    const float distance = sqrt((x.x - center.x) * (x.x - center.x) + (x.y - center.y) * (x.y - center.y));

    if (distance <= this->radius)
    {
        return true;
    }
    else return false;
}
```

```cpp
bool IsInside(const glm::vec2 &x)
{
    const float distance = glm::length(x - center);

    if (distance <= this->radius)
    {
        return true;
    }
    else return false;
}
```

```cpp
bool IsInside(const glm::vec2 &x)
{
    // 최적화 - sqrt가 느리기에 dot을 활용해 보자
        // 자기 자신의 dot product = 거리의 제곱
    const float distanceSquared = glm::dot(x - center, x - center);
    if (distanceSquared <= this->radiusSquared /* radius의 제곱 */)
    {
        return true;
    }
    else return false;
}
```

---

## 좌표계 변환 적용해 보기

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/8/06)

```cpp
glm::vec2 TransformScreenToWorld(glm::vec2 positionScreen)
{
    const float aspect = float(width) / height;
    const float xScale = 2.0f * aspect / (this->width - 1);
    const float yScale = 2.0f / (this->height - 1);

    return glm::vec2(positionScreen.x * xScale - aspect, -(positionScreen.y * yScale - 1.0f));
}
```

---

## TIPs - 폴더 비교하기

* Visual Studio Code -> Diff Folders -> Ctrl + Shift + p -> new diff panel