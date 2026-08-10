---
layout: default
title: "3.2. 애니메이션"
parent: "(Advanced)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/3.2.Ani)

---

* Player에 Animator를 붙여준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-1.png"/>
</p>

* Animator에는 Controller가 필요하니 Animator Controller를 만들어 붙여준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-2.png"/>
</p>

* Animation에 사용될 Sprite를 모두 드래그앤드롭하여 Player에 넣어주자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-3.png"/>
</p>

* 넣어주면 이런게 생긴다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-4.png"/>
</p>

* 아래와 같이 필요한 Animation을 모두 드래그 앤 드롭하고

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-5.png"/>
</p>

* 여기서 Sprite가 하나면 그냥 Player에 드래그 앤 드롭하면 object로 들어가 버리는데 아래와 같이 하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-2-7.png"/>
</p>

* 이제 PlayerController.cs에서 상태를 관리하면 애니메이션을 동작해 보자.

```cs
public class PlayerController : MonoBehaviour
{
    public float _speed = 5.0f;
    bool _isMoving = false;                 // 움직임이 가능한지 여부(움직이는 도중에 움직임은 불가능)
    Vector3Int _cellPos = Vector3Int.zero;  // 캐릭터의 위치
    public Grid _grid;                      // Unity에서 Grid는 받아준다.
    Animator _animator;

    MoveDir _moveDir = MoveDir.Down;        // 어느방향으로 움직일지
    public MoveDir Dir
    {
        get { return _moveDir; }
        set 
        {
            if (_moveDir == value)
                return;

            switch(value)
            {
                case MoveDir.Up:
                    _animator.Play("WALK_BACK");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;
                case MoveDir.Down:
                    _animator.Play("WALK_FRONT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;
                case MoveDir.Left:
                    _animator.Play("WALK_RIGHT");
                    // 스케일만 뒤집어 준다.
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;
                case MoveDir.Right:
                    _animator.Play("WALK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;
                case MoveDir.None:
                    if(_moveDir == MoveDir.Up)
                    {
                        _animator.Play("IDLE_BACK");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    else if (_moveDir == MoveDir.Down)
                    {
                        _animator.Play("IDLE_FRONT");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    else if (_moveDir == MoveDir.Left)
                    {
                        _animator.Play("IDLE_RIGHT");
                        transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    }
                    else
                    {
                        // (_moveDir == MoveDir.Right)
                        _animator.Play("IDLE_RIGHT");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    break;
            }

            _moveDir = value;
        }
    }

    // ...
```
