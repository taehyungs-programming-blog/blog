---
layout: default
title: "05. Virtual Trackball"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/5/5.virtualtrackball)

* 구를 드래그하여 돌려보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-5-1.png"/>
</p>

* 가령 위 그림처럼 P1에서 P2로 드래그한다 가정하자
* 원점을 알기에 P1, P2 Vector는 알고 있고
* 회전축 n은 P1, P2의 Cross Product로 알 수 있다
* 회전각은 P1, P2의 Normalize후 Dot Product로 알 수 있다

```cpp
void ExampleApp::Update(float dt) {

    // ...

            
    // mainSphere를 어떻게 회전시킬지 결정
    if (m_dragStartFlag) { // 드래그를 시작하는 경우
        m_dragStartFlag = false;

        prevVector = pickPoint - m_mainBoundingSphere.Center;

    } else {
        Vector3 currentVector = pickPoint - m_mainBoundingSphere.Center;
        
        // 마우스가 조금이라도 움직였을 경우에만 회전시키기
        if ((currentVector - prevVector).Length() > 1e-3) {

            // Quaternion::FromToRotation()
            q = SimpleMath::Quaternion::FromToRotation(prevVector, currentVector);
            cout << q.x << " " << q.y << " " << q.z << " " << q.w << endl;
            prevVector = currentVector;
        }
    }
}

```
