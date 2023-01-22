---
layout: default
title: "03. GLM(OpenGL Mathematics)"
parent: "(Graphics Basic 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Vector 복습

* 내적(Dot Product) : 
    * 수직이 되는 거리를 구한다
    * (1, 2) * (3, 4) = 1 * 3 + 2 * 4 = 11

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-3-2.png"/>
</p>

* 외적(Cross Product) : 
    * 3차원에서 두 백터의 수직이 되는 백터를 구한다.(왼손 법칙)
    * 두 백터의 평면에 수직인 백터를 구한다 생각하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-3-3.png"/>
</p>

---

## Vector와 GLM

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/6/04)

* OpenGL Mathmatics
* DirectX에도 계산용 api가 있지만 사용법이 복잡하여 비교적 간단한 GLM을 사용한다.

```bash
$ vcpkg install glm:x64-windows
$ vcpkg integrate install 
```

* 사용해보기

```cpp
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp> // cout 출력을 위한 string_cast()

#include <DirectXMath.h> // DirectXMath와 glm 비교용

// CPP 파일에서 using namespace std를 사용하는 것은 괜찮습니다.
using namespace std; // cout, endl;
using namespace glm;
using namespace DirectX;

int main()
{
    /*
    * glm 설치
    * vcpkg install glm:x64-windows
    */

    /*
    * glm의 기본적인 사용 방법
    */

    // 벡터 정의하는 방법 (vec3, vec4 등도 있어요)
    glm::vec3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(7.0f, 8.0f, 9.0f);

    // 벡터를 cout으로 문자열로 바꿔서 출력하는 방법
    cout << glm::to_string(b - a) << endl;

    // 벡터끼리 더하기 빼기 가능
    auto c = b - a; // b + a

    cout << "a = " << glm::to_string(a) << endl;
    cout << "b = " << glm::to_string(b) << endl;
    cout << "b - a = " << glm::to_string(c) << endl;

    // 벡터에 스칼라(숫자 하나)를 곱하면 벡터의 모든 원소에 곱하기
    c = a * 10.0f; 

    cout << "c = a * 10.0f; "<< glm::to_string(c) << endl;

    // 벡터의 길이 구하기
    float l = glm::length(c);

    cout << (b - a).length() << endl; // float라서 to_string 할 필요가 없음

    // 벡터를 길이가 1.0인 유닛(unit) 벡터로 만들기
    auto u = glm::normalize(b - a);
    // auto u = (b-a) / glm::length(b-a)
    cout << glm::length(u) << endl;

    /* 주의: 길이가 0.0인 벡터를 유닛 벡터로 만들려고 시도하면 오류 발생
    
    if (glm::length(u) < 1e-4f) {
        // 다른 방법으로 처리
    }

    if (glm::dot(u, u) < 1e-16f) { // sqrt()를 피하기 위해 dot 사용
        // 다른 방법으로 처리
    }

    */

    // 내적(inner product, dot product)
    auto aDotB = glm::dot(a, b);

    cout << aDotB << endl; // 내적의 결과는 float라서 to_string 할 필요가 없음

    cout << dot(vec3(1, 0, 0), vec3(0, 1, 0)) << endl; // 0
    cout << dot(vec3(1, 2, 3), vec3(1, 2, 3)) << endl; // 1*1 + 2*2 + 3*3 = 14

    // 외적(outer product, cross product)
    // 외적으로 normal을 구할 때는 a와 b가 unit vector 여야 함
    auto aCrossB = glm::cross(a, b); 
    
    cout << to_string(aCrossB) << endl; // 외적의 결과는 벡터
    cout << to_string(cross(vec3(1, 0, 0), vec3(0, 1, 0))) << endl; // vec3(0.0, 0.0, 1.0)
    cout << length(cross(vec3(1.5, 0, 0), vec3(0, 2, 0))) << endl; // 3

    /* 기타 사용 방법들 */
    glm::vec4 v4(glm::vec3(2.0f), 1.0f);

    glm::vec3 v3 = { 2.0f, 4.0f, 6.0f}; // List initialization
    // glm::vec3 v3 = vec3{ 2.0f, 2.0f, 2.0f};
    // glm::vec3 v3 = vec3( 2.0f, 2.0f, 2.0f);
    // glm::vec3 v3 = vec3(2.0f); // 모두 같은 값으로 초기화할 경우

    std::cout << v3.x << " " << v3.y << " " << v3.z << std::endl; // 2 4 6
    std::cout << v3.r << " " << v3.g << " " << v3.b << std::endl; // 2 4 6
    std::cout << v3[0] << " " << v3[1] << " " << v3[2] << std::endl; // 2 4 6

    /*
    * Double (광추적은 정밀도가 높아야 해서 double을 사용하는 경우도 많음)
    */
    auto doubleVector = glm::dvec3(1.0, 2.0, 3.0);

    /*
    * DirectXMath 보다 GLM이 더 직관적이에요.
    */

    // DirectXMath를 이용해서 벡터의 길이를 구하는 경우
    XMFLOAT4 xfloat4 = { 1.0f, 2.0f, 3.0f, 1.0f };
    auto xvector = XMLoadFloat4(&xfloat4);
    xvector = XMVector3Length(xvector); // sqrt(1*1 + 2*2 + 3*3), 함수 이름이 XMVector 숫자3 Length() 입니다.

    float length;
    XMStoreFloat(&length, xvector);

    cout << "DirectXMath Length = " << length << endl;
    cout << sqrt(1.0 * 1.0 + 2.0 * 2.0 + 3.0 * 3.0) << endl;

    // GLM을 이용해서 벡터의 길이를 구하는 경우
    {
        glm::vec3 v = { 1.0f, 2.0f, 3.0f };
        const float length = glm::length(v); // sqrt(1*1 + 2*2 + 3*3)
        cout << "GLM length = " << length << endl;
    }
}

```