---
layout: default
title: "01. 배열/동적배열/연결리스트"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 동적배열 구현

```csharp
class MyList<T>
    {
        const int DEFAULT_SIZE = 1;
        T[] _data = new T[DEFAULT_SIZE];
        public int Count = 0;
        public int Capacity { get { return _data.Length; } };

        // O(N)? -> O(1) 이사가 거의 일어나지 않아서 O(1)
        public void Add(T item)
        {
            if(Count >= Capacity)
            {
                // 현재 Count의 2배로 우선할당한다
                T[] newArray = new T[Count * 2];
                for(int i = 0; i < Count; i++)
                {
                    newArray[i] = _data[i];
                }
                _data = newArray;
            }

            _data[Count] = item;
        }

        // O(1) - indexer 표현법
        public T this[int index]
        {
            get { return _data[index]; }
            set { _data[index] = value; }
        }

        // O(N)
        public void RemoveAt(int index)
        {
            for(int i = index; i < Count- 1; i++)
            {
                _data[i] = _data[i + 1];
            }
            _data[Count - 1] = default(T);
            Count--;
        }
    }
```

---

## 연결리스트

```csharp
class Room<T>
{
    public T Data;
    public Room<T> Next;
    public Room<T> Prev;
}

class RoomList<T>
{
    public Room<T> Head = null;
    public Room<T> Tail = null;
    public int Count = 0;

    public Room<T> AddLast(T data)
    {
        Room<T> newRoom = new Room<T>();
        newRoom.Data = data;

        if(Head == null)
        {
            Head = newRoom;
        }

        if(Tail != null)
        {
            Tail.Next = newRoom;
            newRoom.Prev = Tail;
        }

        Tail = newRoom;
        Count++;

        return newRoom;
    }

    public void Remove(Room<T> room)
    {
        if(Head == room)
        {
            Head = Head.Next;
        }

        if(Tail == room)
        {
            Tail = Tail.Prev;
        }

        if(room.Prev != null)
        {
            room.Prev.Next = room.Next;
        }

        if(room.Next != null)
        {
            room.Next.Prev = room.Prev;
        }

        Count--;
    }
}
```