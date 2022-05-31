---
layout: default
title: "[이론] 내가 만드는 smart pointer"
parent: "(C++ IOCP)"
grand_parent: "Game Server 👾"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-05)

---

## 근본적 질문

🍒 STL 표준에 Smart Pointer가 이미 다 구현되어 있는데 굳이 왜 **다시 구현을 해야할까?**<br>
🍒 다시 구현 자체가 목적이 아니다. 필요성에 대해 느끼는 것이 목적

---

## smart pointer의 필요성

```cpp
class Missile
{
public:
	void SetTarget(Wraight* target)
	{
		_target = target;
	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;

		return true;
	}

	Wraight* _target = nullptr;
};

int main()
{	
	Wraight* wraight = new Wraight();
	Missile* missile = new Missile();

	missile->SetTarget(wraight);

	// 레이스가 피격 당함
	delete wraight;
    // 여기서 이렇게 마음대로 메모리를 날려도 될까???
    // missile에서 wraight를 어떻게 쓰고 있는지도 모르는데??
	
    // ...
}
```

---

## 구현 1차

```cpp
#pragma once

/*---------------
   RefCountable
----------------*/

class RefCountable
{
public:
	RefCountable() : _refCount(1) { }
	// _refCount를 1로 초기화 한거는 정책에 따라 달라질 수 있음, 여기서는 1로 초기화 한다고 가정한다
	virtual ~RefCountable() { }

	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}

protected:
	atomic<int32> _refCount;    // 나를 참조하고 있는애들이 몇명이냐?
};
```

```cpp
// 이제 카운팅이 필요한 객체는 RefCountable를 상속하자
class Wraight : public RefCountable
{
public:
	// ...
```

```cpp
class Missile : public RefCountable
{
public:
	void SetTarget(Wraight* target)
	{
		_target = target;
		// RefCount를 추가해준다 
            // -> 딱 봐도 문제가 보인다 set 하는 와중 wraight가 delete될수 있음
		_target->AddRef();
	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;

		// TODO : 쫓아간다

		if (_target->_hp == 0)
		{
			// Release처리도 RefCount를 감소시킨다
			_target->ReleaseRef();
			_target->nullptr;
			return true;
		}

		return false;
	}

	Wraight* _target = nullptr;
};
```

🍒 이제 직접 delete하지말고 release한다

```cpp
int main()
{	
	Wraight* wraight = new Wraight();
	Missile* missile = new Missile();
	missile->SetTarget(wraight);

	// 레이스가 피격 당함
	wraight->ReleaseRef();
	wraight->nullptr;

    // ...
}
```

---

## 구현 2차

🍒 하지만 멀티쓰레드에서 동작하게 하려면... 또 다른 문제가 발생한다

```cpp
class Missile : public RefCountable
{
public:
	void SetTarget(Wraight* target)
	{
		_target = target;
		_target->AddRef();

		/* (문제)
			target을 복사하는 행위와
			AddRef를 하는 행위가 별개로 동작하다보니
			멀티쓰레드에 세이프하지 못하다
		*/
	}

	// ...
```

* 어떤문제가 있을까?
    * 예를 들어보자면 SetTarget을 하던도중 다른 쓰레드에 의해서 Release가 발생한다면?
    * `_target = target;`의 복사가 끝나고 AddRef를 호출하는데 _target의 복사만 끝나고 Release가 호출되어버려 메모리가 파괴된다면…
* 여기서 드는 의문 SetTarget자체가 쓰레드 세이프한가?
    * 만약 여러개의 쓰레드에서 동시에 SetTarget해버리면 안전할까?
    * 안전하지 않고 그렇게 사용못하도록 막아야함. 
    * 그러나 막지않은 이유는 여기서 설명하고자하는 것은 스마트포인터의 필요성임. Set, Release할때 카운팅을 어떻게 할건지 그 설명을 하고싶은 것이다.

🍒 그럼 해결해보자

```cpp
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() { }
	TSharedPtr(T* ptr) { Set(ptr); }

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	bool		operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	T*			operator*() { return _ptr; }
	const T*	operator*() const { return _ptr; }
				operator T* () const { return _ptr; }
	T*			operator->() { return _ptr; }
	const T*	operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};
```

🍒 설명을 좀 추가하자면

```cpp
class TSharedPtr
{
    // ...

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();		// 내가 들고있던 포인터를 날리고

				// 여기서 메모리가 해지(delete)되고 
				// Set을 진행한다면??

			Set(rhs._ptr);	// 새롭게 들어온 포인터를 셋
		}
		return *this;
	}

    // ...

    private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)// 여기서 메모리 체크한다 걱정말자 ㅎ
			ptr->AddRef();
	}
```

사용은 이렇게 하자

```cpp
// using이렇게 쓸수 있음을 기억(자주까먹음)
using WraightRef = TSharedPtr<Wraight>;
using MissileRef = TSharedPtr<Missile>;

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
    // ...

int main()
{	
	WraightRef wraight(new Wraight());	// 생성하면서 Set이 한번 호출됨
	wraight->ReleaseRef();	// RefCount를 하나 제거해야함
	MissileRef missile(new Missile());
	missile->ReleaseRef();

    // ...

	// 피격
	wraight->nullptr;
	// 이게 되나?
	// wraight->WraightRef(nullptr);	// 컴파일러가 이렇게 고쳐줌
	// 내부에서 Release -> Set호출된다

```

* 단, 우리가 만든 스마트 포인터는 단점이 있다.
    * **이미 만들어진 클래스를 대상으로 사용불가** (`RefCountable`를 상속해야함) 👉 해결해야함.(그냥 STL 스마트포인터를 쓰자..)
    * 순환(Cycle) 문제가 발생한다.(표준 스마트 포인터도 동일함) 👉 weak_ptr이나 참조(&)로 갖고 있는 방법으로 해결해야한다.