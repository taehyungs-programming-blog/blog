---
layout: default
title: "(Graphics🌠)"
parent: C++
has_children: true
nav_order: 11
---

## 용어정리

### Texture, Sprite, Surface

* [참고사이트 🌍(surface, texture, sprite)](http://1st.gamecodi.com/board/zboard.php?id=GAMECODILAB_QnA_etc&page=37&sn1=&divpage=1&sn=off&ss=on&sc=on&select_arrange=last_comment&desc=asc&no=2060)

* **Texture** - Texture는 데이터 입니다. 실제 이미지를 가지고 있는 데이터!
* **Sprite** - Sprite는 그 값을 가지고 행위를 하는 개체 입니다. 얘가 데이터(Texture)를 어떻게 그려낼지 결정하죠.
* **Surface** - Surface는 결과가 출력되는 바닥?! 입니다. 도화지라고 생각하시면 되겠네요.
* **정리하자면**
    * Texture 를 가지고 Sprite를 통해서 Surface 에 그려낸다! 어떤가요? 너무 부족한가요?^^;
    * DX 에서는 Surface는 하나의 평면이고요. 
    * Texture는 하나 이상의 Surface를 가집니다. 
    * 2D에서는 보통 하나만 가지는데, 3D의 경우 밉맵 때문에 여러 개를 가지죠. 
    * 백버퍼 같은 건 Surface 입니다. Sprite는 좀더 상위 개념입니다. 
    * 주로 2D에서 사용되는 용어고, Texture를 어디에 어떻게 그릴 건지에 대한 정보가 추가로 더 들어갑니다.