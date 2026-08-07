---
layout: default
title: "09. 그림자"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/11_Shadow)

## 들어가기 전

{% raw %}
```cpp
Raytracer(const int &width, const int &height)
    : width(width), height(height)
{
    // sphere 하나와
    auto sphere1 = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.6f), 0.4f);

    sphere1->amb = vec3(0.2f, 0.0f, 0.0f);
    sphere1->dif = vec3(1.0f, 0.1f, 0.1f);
    sphere1->spec = vec3(1.5f);
    sphere1->alpha = 50.0f;

        // GUI 연결하기 위해 보관
    this->sphere = sphere1; 

    objects.push_back(sphere1);

    // trainagle 2개로 바닥을 구성할 예정
        // 매번 이렇게 만들기 힘드니 Squre Class를 별도로 만들어 보자.
    auto triangle1 = make_shared<Triangle>(vec3(-2.0f, -1.0f, 0.0f), vec3(-2.0f, -1.0f, 4.0f), vec3(2.0f, -1.0f, 4.0f));
    auto triangle2 = make_shared<Triangle>(vec3(-2.0f, -1.0f, 0.0f), vec3(2.0f, -1.0f, 4.0f), vec3(2.0f, -1.0f, 0.0f));
    triangle1->amb = vec3(0.2f);
    triangle1->dif = vec3(0.8f);
    triangle1->spec = vec3(1.0f);
    triangle1->alpha = 50.0f;
    triangle2->amb = vec3(0.2f);
    triangle2->dif = vec3(0.8f);
    triangle2->spec = vec3(1.0f);
    triangle2->alpha = 50.0f;
    objects.push_back(triangle1);
    objects.push_back(triangle2);

    light = Light{{0.0f, 1.0f, 0.2f}}; // 화면 뒷쪽
}
```
{% endraw %}

```cpp
class Square : public Object
{
public:
    Triangle triangle1, triangle2;

    Square(vec3 v0, vec3 v1, vec3 v2, vec3 v3)
        : triangle1(v0, v1, v2), triangle2(v0, v2, v3)
    {
    }

    virtual Hit CheckRayCollision(Ray &ray)
    {
        auto hit1 = triangle1.CheckRayCollision(ray);
        auto hit2 = triangle2.CheckRayCollision(ray);

        if (hit1.d >= 0.0f && hit2.d >= 0.0f)
        {
            return hit1.d < hit2.d ? hit1 : hit2;
        }
        else if (hit1.d >= 0.0f)
        {
            return hit1;
        }
        else
        {
            return hit2;
        }
    }
};
```

---

## 그림자 구현

* 빛을 받지 못하는 부분을 안그리지말고 어둡게 그려보자.
    * Ambient Color만 반영해 주면된다.
* 여러 방법이 있겠지만
    * 물체에세 카메라로 Ray를 하나 더 쏘면 해결할 수 있다.

```cpp
vec3 traceRay(Ray &ray)
{
    // Render first hit
    const auto hit = FindClosestCollision(ray);

    if (hit.d >= 0.0f)
    {
        glm::vec3 color(hit.obj->amb);

        const vec3 dirToLight = glm::normalize(light.pos - hit.point);

        Ray shadowRay = { hit.point + dirToLight * 1e-4f /*약간 움직여야지 본인한테 Hit판정이 날수 있음.*/
                        , dirToLight };

        if(FindClosestCollision(shadowRay).d < 0.0f)
        {
            const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);
            const vec3 reflectDir = 2.0f * dot(hit.normal, dirToLight) * hit.normal - dirToLight;
            const float specular = glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), hit.obj->alpha);

            color += hit.obj->dif * diff + hit.obj->spec * specular;
        }

        return color;
    }

    return vec3(0.0f);
}
```
