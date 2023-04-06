---
layout: default
title: "24. Quaternion"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://www.youtube.com/watch?v=zc8b2Jo7mno)

## 기존 오일러(Euler) 방식의 문제점.

```
     y
     |
     |
     |
     |
     |
     ------------------------- x
    /
   /
  /
 /
/
z
```

* 각도를 컨트롤할때 z->x->y순으로 컨트롤한다 가정하자
* z 를 90도 돌리면 x, y가 만나게된다.

```
    x, y 
     | 
     | 
     | 
     | 
     |  
    / 
   / 
  / 
 / 
/ 
z
```

* 이제 한 축이 삭제되었다 x, y를 어떻게 돌려도 기존 x축을 돌릴순 없다.

---

## Quaternion으로 해결해 보자.

* 우선 기초부터 학습 !
    * 복소수(Complex Number)는 다음과 같이 표현한다
    * `z = a + bi` (`i^2 = -1`, 허수)

```
(y)      z
 |      /
 |  r /      b(높이)
 |  /
 |/
 --------------(x)
     a
```

* `|z| = |a+bi| = sqrt(a^2 + b^2)`
* 참고로 켤레복소수는 `z*`로 표현하며 `a - bi`이다.
* `z * z* = a^2 + b^2`

```
(y)     z1 
 |     / 
 |  r1/       
 |  / 
 |/ (a) 
 --------------(x)

z1 = r1( cos(a) + i*sin(a) ) 라고 표현할 수 있다.
z2 = r2( cos(a2) + i*sin(a2) )
z1 * z2 = ???
=r1*r2*(cos(a)*cos(a2) +i*cos(a)*sin(a2) +i*sin(a)*cos(a2)-sin(a)*sin(a2))
=r1*r2*((cos(a)*cos(a2)-sin(a)*sin(a2))+i*(cos(a)*sin(a2)+sin(a)*cos(a2))
=r1*r2*( cos(a+a1) + i*sin(a+a2) )
```

* 만약 r2가 단위벡터라면
* `r1*( cos(a+a1) + i*sin(a+a2) )`
* z1에 z2를 곱해주면
* z1에 각도를 더한값이 나온다(핵심)
* z1의 회전이라 볼수 있다.

---

- 복소수를 이용해 2D회전이 가능함을 봤다.
- 그럼 3D로 확장하려면 어떻게 해야할까?

```
복소수는 Matrix로 표현이 가능하다

a + bi = 
[ a -b ]
[ b  a ]

실수의 단위
E = 
[ 1 0 ] 
[ 0 1 ]

허수의 단위
I = 
[ 0 -1 ] 
[ -1 0 ]

I^2 = -E이기에
a*E + b*I라 표현가능

(y)       
 |     /  
 |  r /    r*sin(a)  
 |  / 
 |/ (a)  
 --------------(x)
     r*cos(a)


[ cos(a) -sin(a) ]
[ sin(a)  cos(a) ]
```

- 위를 이용하면 3D회전을 해결할 수 있다.
- 단, 4x4 Matrix를 이용해야함

* 이게 뭔 소리? 이게 왜 기존 오일러(Euler) 방식의 문제점.을 해결했다는 건데?
    * 결론적으로 오일러 방식의 문제는 각을 조절시 하나의 축씩 이동하며 생기는 문제
    * 위 Matrix를 사용시 세 각을 동시에 변경이 가능하다