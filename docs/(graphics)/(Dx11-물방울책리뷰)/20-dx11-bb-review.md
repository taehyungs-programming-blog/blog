---
layout: default
title: "20. Normal & Displacement Map"
parent: "(DirectX11 물방울책 리뷰)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/15)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/bb/bb-20-1.png"/>
</p>

## Normal Mapping &Tangent Space

* Normal Mapping
    * 일반적인 텍스처 매핑은 오브젝트의 표면에 색을 입히는 데 사용됩니다. 그러나 그래픽에서 더 고차원의 디테일을 표현하기 위해 "노말 맵(normal map)"이라는 특별한 유형의 텍스처가 사용될 수 있습니다. 노말 맵은 표면의 각 픽셀이 어떤 방향을 향하고 있는지를 나타내는 벡터 정보를 저장합니다. 이 정보는 렌더링 시에 표면에 입혀지는 광택, 그림자 등을 더 현실적으로 표현하는 데 활용됩니다.
    * 노말 맵은 주로 RGB 색상으로 표현되며, 각 색상 채널(R, G, B)은 3D 벡터의 x, y, z 성분을 나타냅니다. 이렇게 해서 3D 객체의 각 표면에 방향 벡터를 할당할 수 있고, 이 방향 벡터는 빛의 반사나 굴절 방식을 결정하는 데 사용됩니다.
* Tangent Space
    * 노말 맵이 효과적으로 작동하려면 "탄젠트 공간(tangent space)"에 대한 이해가 필요합니다. 3D 모델의 각 버텍스(정점)에는 일반적으로 세 가지 주요 벡터가 있습니다:
    * Normal: 해당 버텍스에서 표면의 외부를 향하는 방향
    * Tangent: 해당 버텍스에서 표면을 따라 "가로로" 향하는 방향
    * Bitangent: 해당 버텍스에서 표면을 따라 "세로로" 향하는 방향
    * 이 세 벡터는 특정 버텍스에 대한 로컬 좌표계를 형성합니다. 노말 맵의 정보는 대체로 이 탄젠트 공간에서 해석됩니다. 즉, 노말 맵의 벡터는 월드 공간이나 오브젝트 공간이 아니라 이 탄젠트 공간에 대한 것입니다.
* 이러한 정보를 활용하면 3D 그래픽에서 노말과 탄젠트, 비탄젠트 벡터를 사용하여 렌더링 시 빛의 행동을 계산할 수 있습니다. 이는 표면의 디테일을 훨씬 더 현실적으로 표현할 수 있게 해줍니다.

---

