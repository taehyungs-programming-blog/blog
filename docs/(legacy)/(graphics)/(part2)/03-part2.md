---
layout: default
title: "03. 삼각형 레스터화"
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

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/1_Triangle)

## 이론

* 여기서 할 것은 내가 그릴 Pixel의 값이 삼각형안에 있는지 확인하는 것인데
* 어떻게 해야할까???
    * 사실 이전시간에 배운 **무게 중심 좌표계**를 이용하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-3-1.jpg"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-3-2.jpg"/>
</p>

* 세 Edge에 대해 모두 Edge Function을 돌리면 되겠군??

---

## 구현

```cpp
float Rasterization::EdgeFunction(const vec2 &v0, const vec2 &v1,
                                  const vec2 &point) {

    // 어떤 3차원 벡터 a = (ax, ay, az)와 b = (bx, by, bz)가 있을 때,
    // a x b = (ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx) 이다.
    // 앞에서는 glm::cross()로 간단히 계산
    // 여기서는 3차원 정점들을 2차원 평면으로 투영했기 때문에
    // az와 bz가 둘 다 0이라고 놓고 계산하면
    // a x b = (0, 0, ax*by - ay*bx)
    // 여기서 ax*by - ay*bx 반환

    const vec2 a = v1 - v0;
    const vec2 b = point - v0;
    return a.x * b.y - a.y * b.x;
}
```

```cpp
void Rasterization::Render(vector<vec4> &pixels) {

    // DirectX에서 자동으로 처리해주는 부분들이기 때문에 흐름만 봐두셔도
    // 충분합니다. 대부분 내부적으로 하드웨어 제조사에서 개발한 드라이버를
    // 사용합니다.
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage

    // 삼각형을 하나만 그리는 경우입니다.
    // 뒤에서 여러 개의 삼각형으로 확장해봅시다.

    // World 좌표계에 정의된 정점의 좌표들을 Screen Raster 좌표계로 변환
    // 변수 이름을 간단히 하기 위해서 v0, v1, v2를 사용했지만 좌표계가
    // 다릅니다.
    const auto v0 = ProjectWorldToRaster(triangle.v0.pos);
    const auto v1 = ProjectWorldToRaster(triangle.v1.pos);
    const auto v2 = ProjectWorldToRaster(triangle.v2.pos);

    // Bounding box 찾기 (xMin, yMin, xMax, yMax)

    // min(), max(), clamp()가 std::와 glm::에 모두 정의되어 있어서
    // glm::min()과 같이 구체적으로 적어줬습니다.
    // glm::min(), std::min() 중 어떤 것을 사용해도 상관 없습니다.
    // std::min({a, b, c})는 a, b, c 중에서 가장 작은 값을 반환해줍니다.

    const auto xMin = size_t(glm::clamp(
        glm::floor(std::min({v0.x, v1.x, v2.x})), 0.0f, float(width - 1)));
    const auto yMin = size_t(glm::clamp(
        glm::floor(std::min({v0.y, v1.y, v2.y})), 0.0f, float(height - 1)));
    const auto xMax = size_t(glm::clamp(glm::ceil(std::max({v0.x, v1.x, v2.x})),
                                        0.0f, float(width - 1)));
    const auto yMax = size_t(glm::clamp(glm::ceil(std::max({v0.y, v1.y, v2.y})),
                                        0.0f, float(height - 1)));

    // Bounding box에 포함되는 픽셀들의 색 결정
    for (size_t j = yMin; j <= yMax; j++) {
        for (size_t i = xMin; i <= xMax; i++) {

            // Rasterizing a triangle
            // 1. 픽셀이 삼각형에 포함되는지 확인
            // 2. 픽셀의 색 결정
            // 참고: A Parallel Algorithm for Polygon Rasterization

            // 3D에서 bary centric coordinates 구하던 것과 동일한데
            // 2D라서 z값을 0으로 고정하면 간단해짐

            const vec2 point = vec2(float(i), float(j));

            const float alpha0 = EdgeFunction(v1, v2, point);
            const float alpha1 = EdgeFunction(v2, v0, point);
            const float alpha2 = EdgeFunction(v0, v1, point);

            if (alpha0 >= 0.0f && alpha1 >= 0.0f && alpha2 >= 0.0f) {
                // 여기에 들어왔다는 것은 삼각형 내에 속한다는 말.
                
                const float area = alpha0 + alpha1 + alpha2;

                // 픽셀의 색 결정
                // 주의: 원근투영(perspective projection)에서는
                // depth 값을 고려해서 보정해줘야 합니다.

                // 넓이로 나눠줍니다.
                const float w0 = alpha0 / area;
                const float w1 = alpha1 / area;
                const float w2 = alpha2 / area;


                // Bary-centric coordinates를 이용해서 color interpolation
                const vec3 color =
                    (w0 * triangle.v0.color + w1 * triangle.v1.color +
                     w2 * triangle.v2.color);

                pixels[i + width * j] = vec4(color, 1.0f);
            }
        }
    }
}
```