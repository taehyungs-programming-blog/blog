---
layout: default
title: "05. 2차원 변환"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/3_Transformations)

## 이론

* x, y Scale / 회전(Translation) 등을 구현해보자

```
* 회전식만 유도해 보자면 ...
x = r * cos(a)
y = r * sin(a)

// b만큼 회전해 보자
x' = r * cos(a + b)
y' = r * sin(a + b)

x' = r * cos(a + b)
   = r * cos(a)cos(b) - r * sin(a)sin(b)
   = x * cos(b) - y * sin(b)
y' = r * sin(a + b)
   = r * cos(a)sin(b) + r * sin(a)cos(b)
   = x * sin(b) + y * cos(b)
```

---

## 구현

```cpp
vec3 RotateAboutZ(const vec3 &v, const float &theta) { 
    return vec3(v.x * cos(theta) - v.y * sin(theta),
                v.x * sin(theta) + v.y * cos(theta), v.z);
}

void Rasterization::Update() {
    // 애니메이션 구현

    // GUI 입력에 대해서 vertexBuffer 업데이트
    // 이 예제에서는 this->vertexBuffer만 업데이트하고
    // colorBuffer와 indexBuffer는 변화 없음

    for (size_t i = 0; i < circle.vertices.size(); i++) {
        auto temp = RotateAboutZ(circle.vertices[i], this->rotation1);
        temp = temp * vec3(scaleX, scaleY, 1.0f);
        temp = temp + this->translation1;
        temp = RotateAboutZ(temp, this->rotation2);
        temp = temp + this->translation2;
        this->vertexBuffer[i] = temp;
    }

    // 중요: 버텍스 쉐이더(Vertex shader)가 하는 일들입니다.
}
```

---

## 2차원 애니메이션

* 쉽기에 별도 정리안함

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/4_animation)