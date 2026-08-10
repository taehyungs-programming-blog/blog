---
layout: default
title: "12. Lock-Free Stack 구현"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 질문 1. - Lock-Free는 무조건 성능이 좋은가요? 
	* 👉 꼭 그렇지는 않다. 오히려 Lock을 잡는게 성능은 별 차이 없고 안정적일 수 있음
* 질문 2. - 그럼 Lock-Free를 우리는 왜 배워야 하나요? 
	* 👉 필요한 경우가 발생하며 사실 달달 외어서 사용하지는 않고 그때그때 찾아 보면서 사용하면 된다.

---

## 1차 해결책

* 구현을 두 번에 나눠서 한다. 첫 구현 이후 필요성을 설명하고 추가하는 방향이 이해하기가 쉬움!

```cpp
// 설명 없는 코드(설명있는 코드는 아래에)
template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value)
		{

		}

		T data;
		Node* next;
	};

public:
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		while (_head.compare_exchange_weak(node->next, node) == false)
		{
            // _head == node->next 일시 _head = node 된다.
        }
    }

    bool TryPop(T& value)
	{
		Node* oldHead = _head;

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
        }

		if (oldHead == nullptr)
			return false;

		value = oldHead->data;

		return true;
	}

private:
	atomic<Node*> _head;
};
```

```cpp
// 설명 추가한 코드

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value)
		{

		}

		T data;
		Node* next;
	};

public:
	
	/*
		이렇게 할 예정
		(Push)
		node->next에 기존의 node정보를 넣고
		head에 새로 생성한 node를 넣을예정

		(Pop)
		head의 node를 가져오고
		head->next를 다시 head로 연결
	*/

	// 1) 새 노드를 만들고
	// 2) 새 노드의 next = head
	// 3) head = 새 노드

	// [ ][ ][ ][ ][ ][ ][ ]
	// [head]
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;
        // atomic<Node*> _head;

        // _head에 새로추가된 node의 주소를 넣고싶다
        // 단, _head는 멀티 쓰레드에 의해 위험성이 존재

		/*
        * compare_exchange_weak의 동작, 아래동작을 atomic하게 처리해준다.

		if (_head == node->next)
		{
            // 정상적이라면 여기에 들어와야함
            // 위에서 node->next = _head; 해줬기 때문
			_head = node;
			return true;
		}
		else
		{
            // 다른 쓰레드에 의해 _head가 변경되었다면 여기로 오게된다.
			node->next = _head;
			return false;
		}
		*/

		// (주의) 많은 쓰레드가 여기에 몰리며, 여기서 경합이 너무 심해서 라이브 락이 발생할 수 있음
		while (_head.compare_exchange_weak(node->next, node) == false)
		{
            /*
                _head에는 새로생성된 node의 주소가
                node->next에는 이전 node의 주소가 들어간다(<-이름때문에 이게 헷갈림)
            */
			//node->next = _head;
		}

		// 이 사이에 새치기 당하면?
		//_head = node;
	}
	
	// 1) head 읽기
	// 2) head->next 읽기
	// 3) head = head->next
	// 4) data 추출해서 반환
	// 5) 추출한 노드를 삭제

	// [ ][ ][ ][ ][ ][ ]
	// [head]
	bool TryPop(T& value)
	{
		Node* oldHead = _head;

		/*
		if (_head == oldHead)
		{
			_head = oldHead->next;
			return true;
		}
		else
		{
			oldHead = _head;
			return false;
		}
		*/

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
            /*
                여기도 말로 설명하자면
                oldHead에는 Pop할 node주소를 넣고
                oldHead->next에는 이전 노드 주소가 들어간다
            */
			//oldHead = _head;
		}

		if (oldHead == nullptr)
			return false;

		// Exception X
		value = oldHead->data;

		// 잠시 삭제 보류
		//delete oldHead;
        // 지우는게 문제이다. 내가 지우는 순간에 다른 쓰레드에서 쓰고있을 수 있다는게 어려운부분
        // 다음강에서 해결한다.

		// C#, Java 같이 GC가 있으면 사실 여기서 끝

		return true;
	}

private:
	// [ ][ ][ ][ ][ ][ ]
	// [head]
	atomic<Node*> _head;	// atomic은 초기값이 nullptr(0)임
};
```

---

## 2차 해결책

* 결론적으로 node의 delete를 해야한다는 말인데 우리는 가장 좋은 방법을 알고있다. 
	* 👉 **smart pointer!**
* `shared_ptr`로 간단히 해결해보자
* 충격적인 얘기를 먼저하자면 shared_ptr은 lock-free하지 못하기에 shared_ptr로 해결은 안된다.(shared_ptr자체에서 lock을 잡아버림)
* 다음걸 설명하기 위한 step이라 생각하자

```cpp
template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
		{

		}

        // data와 next Node를 shared_ptr로 관리
		shared_ptr<T> data;
		shared_ptr<Node> next;
	};

public:
	void Push(const T& value)
	{
        // Node를 넣을때 make_shared한다
		shared_ptr<Node> node = make_shared<Node>(value);

        // 그냥 head를 읽어올 경우 atomic하지 않기에 std::atomic_load통해 읽어온다
		node->next = std::atomic_load(&_head);

        // 일반 atomic_compare_exchange_weak와 동작이 같음.
        // atomic 하게 _head == node->next 인지 확인 후 _head = node 하라
		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
		{
		}
	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);

		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr)
			return shared_ptr<T>();

		return oldHead->data;
	}

    // 이렇게 하면 delete를 굳이 하지 않아도 된다.

    /*

        그런데 shared_ptr자체가 lock_free한가???
        -> 만약 shared_ptr자체에서 lock을하고 있다면 이게 무슨 의미인가???

        shared_ptr<int32> ptr;
        bool value = atomic_is_lock_free(&ptr);     // false가 나옴, share_ptr자체에서 lock을 걸게된다.  

    */

private:
	shared_ptr<Node> _head;
};
```

---

## 3차 해결책

* 세상 만사가 그리 쉽지않다... `shared_ptr` 자체가 lock_free하지 못하기에 결국 lock으로 동작하게 된다. ㅠㅠ.. 우리가 원하던 결과가 아님
* 다시 lock_free하게 구현해보자 (참로고, 아래 내용이 엄청어려운데 나는 왜 이런코드를 못 짜지 실망말자.)
* 아래코드는 논문에서 나온내용을 코드로 구현, 오래된 학술의 결과이다.(보통사람은 아래처럼 잘 못짠다는 말)

```cpp
template<typename T>
class LockFreeStack
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;    // 포인터 참조 횟수
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value))
		{

		}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};

public:
	// [][][][][][][]
	// [head]
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		// [!]
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{
		}
	}
	
	// [][][][][][][]
	// [head]
	shared_ptr<T> TryPop()
	{
		while (true)
		{
		    CountedNodePtr oldHead = _head; 
			// 참조권 획득 (externalCount를 현 시점 기준 +1 한 애가 이김)
            // externalCount이 기본 1이고 이함수를 지나면 기본 2가 된다.
            // 만약 여러 스레드에서 동시 진입이 되었다면 2이상이 되겠지?
			IncreaseHeadCount(oldHead);
			// 최소한 externalCount >= 2 일테니 삭제X (안전하게 접근할 수 있는)
            // 여기까지오면 externalCount을 증가 시켰기에 접근권(참조권)은 있는것.
			Node* ptr = oldHead.ptr;

			// 데이터 없음
			if (ptr == nullptr)
				return shared_ptr<T>();

			// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
                // 참조권을 획득한 첫 번째 Thread가 여기로 들어옴

				shared_ptr<T> res;
				res.swap(ptr->data);

                // 데이터를 누가 쓰고있나 확인용
				// external : 1 -> 2(나+1) -> 4(나+1 남+2)
				// internal : 1 -> 0
				const int32 countIncrease = oldHead.externalCount - 2;
                // externalCount - 2을 하면 몇 명(스레드)이 참조권을 갖고 있는지 나온다.

                // fetch_add에 얼마를 넣든 원래 값이 리턴됨
                // 예를들어 internalCount = 0이고 fetch_add(2)를하면 리턴값은 0이고 이후 2로 변경
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
                // 참조권은 있으나 소유권이 없는, 그러니깐 첫 번째 Thread이후의 Thread는 모두 여기로 온다.
				// 참조권은 얻었으나, 소유권은 실패 -> 뒷수습은 내가 한다
				delete ptr;
			}
		}
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
        CountedNodePtr newCounter = oldCounter;
        
		while (true)
		{
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;
};
```