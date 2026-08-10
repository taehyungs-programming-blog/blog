---
layout: default
title: "3.1. 이동"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/UnityPortfolio/tree/3.1.Move)

---

* Player로 사용할 **Tiny RPG Forest/Artwork/sprites/hero/** 를 하나 만들자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-1-1.png"/>
</p>

* 그런데 Scene에 넣어도 캐릭터가 너무작아서 안보이는데 Pixels Per Unit을 16으로 바꾸면 나타난다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-1-2.png"/>
</p>

* Player Controller를 Player에 붙이자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity-adv/unity-adv-3-1-3.png"/>
</p>

```cs
public class PlayerController : MonoBehaviour
{
    public float _speed = 5.0f;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKey(KeyCode.W))
        {
            transform.position += Vector3.up * Time.deltaTime * _speed;
        }
        else if (Input.GetKey(KeyCode.S))
        {
            transform.position += Vector3.down * Time.deltaTime * _speed;
        }
        else if (Input.GetKey(KeyCode.A))
        {
            transform.position += Vector3.left * Time.deltaTime * _speed;
        }
        else if (Input.GetKey(KeyCode.D))
        {
            transform.position += Vector3.right * Time.deltaTime * _speed;
        }
        else
        {

        }
    }
}
```

* 셀에 맞춰서 움직이게 해보자.

```cs
public class PlayerController : MonoBehaviour
{
    public float _speed = 5.0f;
    MoveDir _moveDir = MoveDir.None;        // 어느방향으로 움직일지
    bool _isMoving = false;                 // 움직임이 가능한지 여부(움직이는 도중에 움직임은 불가능)
    Vector3Int _cellPos = Vector3Int.zero;  // 캐릭터의 위치
    public Grid _grid;                      // Unity에서 Grid는 받아준다.

    void Start()
    {
        Vector3 pos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f);
        transform.position = pos;
    }

    void Update()
    {
        GetDirectionInput();
        UpdatePosition();
        MoveDirection();
    }

    void UpdatePosition()
    {
        if (_isMoving == false)
        {
            return;
        }
        else
        {
            Vector3 destPos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f);
            Vector3 movePos = destPos - transform.position;

            // 도착여부 체크
            float dist = movePos.magnitude;
            if(dist < _speed*Time.deltaTime)
            {
                // 도착
                transform.position = destPos;
                _isMoving = false;
            }
            else
            {
                // 가는중
                transform.position += movePos.normalized * _speed * Time.deltaTime;
                _isMoving = true;
            }
        }
    }

    void MoveDirection()
    {
        if(_isMoving == false)
        {
            switch(_moveDir)
            {
                case MoveDir.Up:
                    _cellPos += Vector3Int.up;
                    _isMoving = true;
                    break;
                case MoveDir.Down:
                    _cellPos += Vector3Int.down;
                    _isMoving = true;
                    break;
                case MoveDir.Left:
                    _cellPos += Vector3Int.left;
                    _isMoving = true;
                    break;
                case MoveDir.Right:
                    _cellPos += Vector3Int.right;
                    _isMoving = true;
                    break;
            }
        }
    }

    void GetDirectionInput()
    {
        if (Input.GetKey(KeyCode.W))
        {
            _moveDir = MoveDir.Up;
        }
        else if (Input.GetKey(KeyCode.S))
        {
            _moveDir = MoveDir.Down;
        }
        else if (Input.GetKey(KeyCode.A))
        {
            _moveDir = MoveDir.Left;
        }
        else if (Input.GetKey(KeyCode.D))
        {
            _moveDir = MoveDir.Right;
        }
        else
        {
            _moveDir = MoveDir.None;
        }
    }
}
```

