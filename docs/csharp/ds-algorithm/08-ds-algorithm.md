---
layout: default
title: "08. 우선순위큐"
parent: "(자료구조 & 알고리즘)"
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```csharp
class PriortyQueue
{
    List<int> _heap = new List<int>();

    // O(logN)
    public void Push(int data)
    {
        // 힙에 맨 끝에 새로우 데이터 삽입
        _heap.Add(data);

        // 부모와 비교해서 부모보다 클 경우 뒤집는다
        int now = heap.Count - 1;
        while(now>0)
        {
            int next = (now - 1)/ 2;    // 부모의 index(정렬된 이진트리기에 가능)
            if(_heap[now] < _heap[next])
                break;

            int temp = _heap[now];
            _heap[now] = _heap[next];
            _heap[next] = temp;

            // 검사 위치 이동
            now = next;
        }
    }

    // O(logN)
    public int Pop()
    {
        // 제일 큰 값을 리턴
        int ret = _heap[0];

        // 아래 값들을 다시 비교
        int lastIndex = _heap.Count - 1;
        _heap[0] = _heap[lastIndex];
        _heap.RemoveAt(lastIndex);
        lastIndex--;

        int now = 0;
        while(true)
        {
            int left = 2 * now + 1;
            int right = 2 * now + 2;

            int next = now;
            if(left <= lastIndex && _heap[next] < _heap[left])
                next = left;
            if(right <= lastIndex && _heap[next] < _heap[right])
                next = right;

            if(next == now)
                break;  // 왼쪽오른쪽 현재보다 작음
            
            int temp = _heap[now];
            _heap[now] = _heap[next];
            _heap[next] = temp;

            now = next;
        }

        return 0;
    }

    public int Count()
    {
        return _heap.Count;
    }
}
```

```csharp
class PriortyQueue<T> where T : IComparable<T>
{
    List<T> _heap = new List<T>();
    
    // ...
```