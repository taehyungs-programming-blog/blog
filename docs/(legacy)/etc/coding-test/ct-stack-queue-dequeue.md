---
layout: default
title: "stack, queue, dequeue"
parent: "Coding Test 💻"
grand_parent: "Etc 🛠"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Stack

* LIFO(Last In First Out) 방식

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/coding-test/stack.png"/>
</p>

```cpp
#include <iostream>
#include <stack>
using namespace std;
int main(void) {

	stack<int> st;
	stack<int> st2;

	st.push(1);
	st.push(2);
	st.push(3);

	st2.push(10);
	st2.push(20);
	st2.push(30);

	swap(st, st2);

	while (!st.empty()) {
		cout << st.top() << endl;
		st.pop();
	}

	return 0;
}
```

---

## Queue

* FIFO(First In First Out) 방식

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/coding-test/queue.png"/>
</p>

```cpp
#include <iostream> 
#include <queue> 

using namespace std; 

int main(void) { 
    queue<int> q1; 
    queue<int> q2; 
    
    q1.push(1); 
    q1.push(2); 
    q1.push(3); 
    q2.push(10); 
    q2.push(20); 
    q2.push(30); 
    
    swap(q1, q2); 
    
    while (!q1.empty()) 
    { 
        cout << q1.front() << endl; 
        q1.pop(); 
    } 
    
    return 0; 
}
```

---

## Dequeue

* 양쪽으로 삽입/삭제 가능

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/etc/coding-test/dequeue.png"/>
</p>

```cpp
#include<iostream>
#include<deque>
using namespace std;

int N;
deque<int> dq;

void print_dq()
{
	cout << "dq의 원소 : ";
	for (int i = 0; i < dq.size(); i++)
		cout << dq.at(i) << " ";
}

int main()
{
	cout << "\ndq에 1~3까지 삽입\n";
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	print_dq(); // 1 2 3

	cout << "\n\ndq의 1번째 위치에 4 삽입\n";
	dq.insert(dq.begin()+1, 4);
	print_dq(); // 1 4 2 3

	cout << "\n\ndq의 첫번째와 마지막 원소 삭제\n";
	dq.pop_front();
	dq.pop_back();
	print_dq(); // 4 2

	cout << "\n\ndq의 첫번째와 마지막 원소에 각각 5와 6 삽입\n";
	dq.push_front(5);
	dq.push_back(6);
	print_dq(); // 5 4 2 6

	cout << "\n\ndq 원소 전체 clear!\n";
	dq.clear();
	print_dq(); // empty

}
```