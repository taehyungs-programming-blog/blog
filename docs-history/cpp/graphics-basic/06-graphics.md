---
layout: default
title: "06. 조명 효과 넣기"
parent: "(Graphics Basic 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/10/08)

## Shading, Material

* **Shading** - 음영을 줘서 입체감을 표현하는 것.
* 그럼 어떻게 Shading을 표현할까?
* Phong Reflection Model을 적용할 예정
	* **Ambient** - 물체에서 전반적으로 반사되는 광
	* **Diffuse** - 빛의 난반사 광
	* **Specular** - 빛의 정반사 광
* 세개를 반영하면 된다.
* 내가 예전에 정리한 Shading 관련 정리 [클릭!](https://taehyungs-programming-blog.github.io/blog/docs/cpp/directx-12/g-18/)
* 참고로 Material이라고도 하는데 말 그대로 물체의 표면? 성질? 을 나타내기에 Material이라고도 한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-1.png"/>
</p>

---

## Diffuse

* ambient는 쉽고 Diffuse먼저 설명해 보자면 ...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-3.png"/>
</p>

* 우선 변수 정의
    * N(물체 표면의 Normal Vector)
    * L(빛이 들어오는 방향의 반대방향 Vector)
* N과 L의 각도가 클수록 빛이 비스듬하게 들어오기에 빛의 영향은 작아지고
작을수록 빛의 영향은 커지게 된다.
* 좀 쉽게말해서 각도가 90일때 값이 가장 작고 그 외 일때 값이 크게하면 될 것이다.
    * 어디서 많이 본 거 같은데? 맞다 `cos(a)`이다.
* 그런데 cos(a)는 소수점이 길어지고 계산이 오래 걸리기에 
* 좀 더 최적화를 해보자면
* 우리가 알고 있는 값은 N, L이기에 N, L을 통해 cos(a)를 구할수 있다.
    * `cos(a) = N 내적 L`
* 증명? -> [여기를 참조 하자](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=jihyoseok&logNo=221481723291)
    * 대략 이런식 -> `cos(a) = coa(a1-a2) = cos(a1)cos(a2) + sin(a1)sin(a2) ...`

---

## Specular

* 들어온 빛의 정반사광을 R이라 할때 우리 눈이 E라는 Vector로 표현가능하다면
* R과 E를 통해 b를 구하면 된다.

```
               | (a) (l)
               |-----/
               |    /
            (b)|   /
               |  /
               | /
               |/
-----------------------------------

l = a + b
a = l - b
a = l - (n*l)n

그럼

l의 정반사 r을
r = l - a - a = l - 2a
라 표현할수 있지 않을까?
r = l - 2(l - (n*l)n) = 2(n*l)n - l
여기까지하면 정반사광을 구했다.

이제 눈과 r과의 관계를 넣자면
```


```cpp
// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray)
{
    auto hit = sphere->IntersectRayCollision(ray);

    if (hit.d < 0.0f)
    {
        return vec3(0.0f);
    }
    else
    {
        // return sphere->color * hit.d; // <- 앞에서 사용했던 색 결정

        // 여기서 퐁 모델(Phong reflection model)으로 조명 효과 계산!
        // 참고 자료
        // https://en.wikipedia.org/wiki/Phong_reflection_model
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/phong-shader-BRDF

        // Diffuse
        const vec3 dirToLight = glm::normalize(light.pos - hit.point);
        const float diff = glm::max(glm::dot(hit.normal, dirToLight), 0.0f);

        // Specular
        const vec3 reflectDir = 2.0f * dot(hit.normal, dirToLight) * hit.normal - dirToLight;
        // 제곱을 함으로서 빛을 모이게 보이는 효과를 준다
        // 제곱을 하지 않으면 빛이 퍼져보이게 된다.
        const float specular = glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), sphere->alpha);

        // ambient + diffuse + specular
        return sphere->amb + sphere->diff * diff + sphere->spec * specular * sphere->ks;
        // return sphere->diff * diff;
        // return sphere->spec * specular * sphere->ks;
    }
}
```

