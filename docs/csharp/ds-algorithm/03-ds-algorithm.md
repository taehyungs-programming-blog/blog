---
layout: default
title: "03. 플레이어 이동"
parent: "(자료구조 & 알고리즘)"
grand_parent: "(C#)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 랜덤하게 움직이는 플레이어 추가

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/05)

```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Algorithm_Example
{
    class Player
    {
        public int PosY { get; private set; }
        public int PosX { get; private set; }
        Random _random = new Random();   

        Board _board;

        public void Initialize(int posY, int posX, int destY, int destX ,Board board)
        {
            PosX = posX;
            PosY = posY;
            _board = board;
        }

        const int MOVE_TICK = 100;
        int _sumTick = 0;
        public void Update(int deltaTick)
        {
            _sumTick += deltaTick;
            if(_sumTick > MOVE_TICK)
            {
                _sumTick = 0;
                int randValue = _random.Next(0, 5);
                switch(randValue)
                {
                    case 0:         // 상
                        if (PosY - 1 > 0 && _board.Tile[PosY -1 , PosX] == Board.TileType.Empty)
                            PosY -= 1;
                        break;
                    case 1:         // 하
                        if (PosY + 1 > 0 && _board.Tile[PosY + 1, PosX] == Board.TileType.Empty)
                            PosY += 1;
                        break;
                    case 2:         // 좌
                        if (PosX - 1 > 0 && _board.Tile[PosY, PosX - 1] == Board.TileType.Empty)
                            PosX -= 1;
                        break;
                    case 3:         // 우
                        if (PosX + 1 > 0 && _board.Tile[PosY, PosX + 1] == Board.TileType.Empty)
                            PosX += 1;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/ds-algorithm/ds-algorithm-3-1.png"/>
</p>

---

## 오른손 법칙으로 탈출시키기

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/06)

```csharp
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Algorithm_Example
{
    class Pos
    {
        public Pos(int y, int x) { PosX = x; PosY = y; }
        public int PosX { get; private set; }
        public int PosY { get; private set; }
    }

    class Player
    {
        public int PosY { get; private set; }
        public int PosX { get; private set; }
        Random _random = new Random();   

        Board _board;
        List<Pos> _points = new List<Pos>();

        enum Dir
        {
            Up = 0,
            Left = 1,
            Down = 2,
            Righ = 3
        }

        int _dir = (int)Dir.Up;

        public void Initialize(int posY, int posX, Board board)
        {
            PosX = posX;
            PosY = posY;
            _board = board;

            _points.Add(new Pos(PosY, PosX));

            // 바라보고 있는 방향 기준 좌표 변화를 나타낸다.
            int[] frontY = new int[] { -1, 0, 1, 0 };
            int[] frontX = new int[] { 0, -1, 0, 1 };

            int[] rightY = new int[] { 0, -1, 0, 1 };
            int[] rightX = new int[] { 1, 0, -1, 0 };

            while (PosX != board.DestX || PosY != board.DestY)
            {
                if(_board.Tile[PosY + rightY[_dir], PosX + rightX[_dir]] == Board.TileType.Empty)
                {
                    // 바라보는 방향 기준 우측으로 돈다
                    _dir = (_dir - 1 + 4) % 4;

                    // 한 보 전진시킨다
                    PosX = PosX + frontX[_dir];
                    PosY = PosY + frontY[_dir];

                    _points.Add(new Pos(PosY, PosX));
                }
                else if (_board.Tile[PosY + frontY[_dir], PosX + frontX[_dir]] == Board.TileType.Empty)
                {
                    // 한 보 전진시킨다
                    PosX = PosX + frontX[_dir];
                    PosY = PosY + frontY[_dir];

                    _points.Add(new Pos(PosY, PosX));
                }
                else
                {
                    // 바라보는 방향 기준 좌측으로 돈다
                    _dir = (_dir + 1 + 4) % 4;

                }

                
            }
        }

        const int MOVE_TICK = 100;
        int _sumTick = 0;
        int _lastIndex = 0;
        public void Update(int deltaTick)
        {
            if (_lastIndex >= _points.Count)
                return;

            _sumTick += deltaTick;
            if(_sumTick > MOVE_TICK)
            {
                _sumTick = 0;

                PosY = _points[_lastIndex].PosY;
                PosX = _points[_lastIndex].PosX;
                _lastIndex++;
            }
        }
    }
}
```

