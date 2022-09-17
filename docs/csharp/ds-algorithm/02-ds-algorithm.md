---
layout: default
title: "02. 미로만들기"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 테스트환경조성

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/01)

```csharp
using System;

namespace algorithm
{
    class Program
    {
        static void Main(string[] args)
        {
            const int WAIT_TICK = 1000 / 30;
            int lastTick = 0;
            const char CIRLE = '\u25cf';
            while (true)
            {
                #region 프레임 관리
                int currenttick = System.Environment.TickCount;
                if(currenttick - lastTick < WAIT_TICK)
                    continue;
                lastTick = currenttick;
                #endregion

                Console.SetCursorPosition(0, 0);
                for(int i = 0; i < 25; i++)
                {
                    for(int j = 0; j < 25; j++)
                    {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.Write(CIRLE);
                    }
                    Console.WriteLine();
                }
            }
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/ds-algorithm/ds-algorithm-1-1.png"/>
</p>

---

## 기본 board 만들기

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/02)

```csharp
using System;

namespace Algorithm_Example
{
    class Program
    {
        static void Main(string[] args)
        {
            Board board = new Board();
            board.Initialize(25);

            const int WAIT_TICK = 1000 / 30;
            int lastTick = 0;

            while (true)
            {
                #region 프레임 관리
                int currenttick = System.Environment.TickCount;
                if (currenttick - lastTick < WAIT_TICK)
                    continue;
                lastTick = currenttick;
                #endregion

                Console.SetCursorPosition(0, 0);
                board.Render();
            }
        }
    }
}
```

```csharp
namespace Algorithm_Example
{
    class Board
    {
        public TileType[,] _tile;
        public int _size;
        const char CIRLE = '\u25cf';

        public enum TileType
        {
            Empty,
            Wall
        }

        public void Initialize(int size)
        {
            _tile = new TileType[size, size];
            _size = size;

            for (int y = 0; y < _size; y++)
            {
                for (int x = 0; x < _size; x++)
                {
                    // 첫줄, 막줄은 Wall로 처리하자
                    if (x == 0 || x == _size - 1 || y == 0 || y == size - 1)
                    {
                        _tile[y, x] = TileType.Wall;
                    }
                    else
                    {
                        _tile[y, x] = TileType.Empty;
                    }
                }
            }
        }

        public void Render()
        {
            ConsoleColor prevColor = Console.ForegroundColor;

            for (int y = 0; y < _size; y++)
            {
                for (int x = 0; x < _size; x++)
                {
                    // 이런식의 기능분리도 주목하자.
                    Console.ForegroundColor = GetTileColor(_tile[y, x]);
                    Console.Write(CIRLE);
                }
                Console.WriteLine();
            }

            Console.ForegroundColor = prevColor;
        }

        ConsoleColor GetTileColor(TileType type)
        {
            switch (type)
            {
                case TileType.Empty:
                    return ConsoleColor.Green;
                case TileType.Wall:
                    return ConsoleColor.Yellow;
                default:
                    return ConsoleColor.Green;
            }
        }
    }
}
```

---

## Binary Tree 미로

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/03)

```csharp
public void SetBinaryTree()
{
    Random rand = new Random();
    for (int y = 0; y < _size; y++)
    {
        for (int x = 0; x < _size; x++)
        {
            if (x % 2 == 0 || y % 2 == 0) continue;
            if (y == _size - 2 && x == _size - 2) continue;

            if (y == _size - 2)
            {
                _tile[y, x + 1] = TileType.Empty;
                continue;
            }

            if (x == _size - 2)
            {
                _tile[y + 1, x] = TileType.Empty;
                continue;
            }

            // 1/2확률로 위/아래중 하나의 길을 뚫는다
            if (rand.Next(0, 2) == 0)
            {
                _tile[y, x + 1] = TileType.Empty;
            }
            else
            {
                _tile[y + 1, x] = TileType.Empty;
            }
        }
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/ds-algorithm/ds-algorithm-2-1.png"/>
</p>

---

## Side Winder 미로

* [Get Code 🌎](https://github.com/EasyCoding-7/Algorithm-Example/tree/04)

```csharp
public void SetSideWinder()
{
    Random rand = new Random();
    for (int y = 0; y < _size; y++)
    {
        int count = 1;
        for (int x = 0; x < _size; x++)
        {
            if (x % 2 == 0 || y % 2 == 0) continue;
            if (y == _size - 2 && x == _size - 2) continue;

            if (y == _size - 2)
            {
                _tile[y, x + 1] = TileType.Empty;
                continue;
            }

            if (x == _size - 2)
            {
                _tile[y + 1, x] = TileType.Empty;
                continue;
            }

            if (rand.Next(0, 2) == 0)
            {
                _tile[y, x + 1] = TileType.Empty;
                count++;
            }
            else
            {
                int randomIndex = rand.Next(0, count);
                _tile[y + 1, x - randomIndex * 2] = TileType.Empty;
                count = 1;
            }

        }
    }
}
```