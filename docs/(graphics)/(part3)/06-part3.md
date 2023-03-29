---
layout: default
title: "06. 드래그로 이동시키기"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/6/6.dragmove)

```cpp
void ExampleApp::Update(float dt) {

    // ...

    // mainSphere를 어떻게 이동시킬지 결정
    if (m_dragStartFlag) { // 드래그를 시작하는 경우
        m_dragStartFlag = false;
        prevRatio = dist / (cursorWorldFar - cursorWorldNear).Length();
        prevPos = pickPoint;
    } else {
        // 비율과 위치로 구한다.
        Vector3 newPos = cursorWorldNear + prevRatio * (cursorWorldFar - cursorWorldNear);

        if ((newPos - prevPos).Length() > 1e-3) {
            dragTranslation = newPos - prevPos;
            prevPos = newPos;
        }
    }
}
}
```