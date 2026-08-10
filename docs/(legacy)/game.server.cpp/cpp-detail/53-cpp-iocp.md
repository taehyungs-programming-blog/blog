---
layout: default
title: "[구현] JobQueue - 3(lambda를 이용한 JobQueue 구현)"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-31)

---

🐨 람다에는 어떤 장점이 있을까?

```cpp
// Fuctor에서는 매개변수 a, b를 임시로 담아 두기위해서 tuple을 사용했었다.
void HelloWorld(int32 a, int32 b)
{
    // ...
}

int main()
{
    std::function<void()> func = [=]()
    {
        // 람다를 사용할 경우 매개변수를 임시로 담아두지 않아도 된다.
            // 람다가 알아서 해준다.
        HelloWorld(1, 2);
    }

    // ...

    func(); // 정상적으로 HelloWorld(1, 2); 호출됨.

    // 좀 더 어렵게 설명하면 closure object를 람다에서 생성해 준다.
}
```

🐨 단, 람다도 역시 단점이 있는데

```cpp
// 문제1) 참조한 객체의 생존보장

PlayerRef player = make_shared<Player>();

std::function<void()> func = [&player]()
{
    // 람다에서 player를 사용하려면 player의 생존이 보장되어야 한다.
}
```

```cpp
// 문제2) 자기 자신의 생존보장

class Knight
{
public:
    void HealMe(int32 value)
    {
        _hp += value;
    }

    void Test()
    {
        // 외부에서 이 람다를 호출시 Knight라는 객체의 생존을 보장하지 않으면 crash가 날수 있음
        auto job = [=]()
        {
            HealMe(_hp);
        }
        /* 
            // 위와 동일한 표현이다. 결국 this를 생략한 형태이다.
            auto job = [this]()
            {
                HealMe(this->_hp);
            }
        */
    }

private:
    int32 _hp = 100;
};
```

```cpp
// 사실 문제2는 이렇게 해결이 가능하긴 함

class Knight : public enable_shared_from_this<Knight>
{
    // ...

    void Test()
    {
        auto job = [self = shared_from_this()]()
        {
            // job이 유지될때는 Knight도 무조건 유지된다.
            HealMe(self->_hp);
        }
```

---

🐨 위 문제를 조심하며 구현을 해보자면<br>
🐨 우선은 결론부터... 이렇게 사용하고자 한다.

```cpp
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

    // 사실상 사용법은 이전과 동일
	GRoom->PushJob(&Room::Broadcast, sendBuffer);
    /*
        template<typename T, typename Ret, typename... Args>
        void PushJob(Ret(T::*memFunc)(Args...), Args... args)
        {
            shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
            auto job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
            _jobQueue.Push(job);
        }
    */

	return true;
}
```

```cpp
class JobSerializer : public enable_shared_from_this<JobSerializer>
{
public:
	void PushJob(CallbackType&& callback)
	{
		auto job = ObjectPool<Job>::MakeShared(std::move(callback));
		_jobQueue.Push(job);
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		auto job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		_jobQueue.Push(job);
	}

	virtual void FlushJob() abstract;

protected:
	JobQueue _jobQueue;
};
```

```cpp
using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};
```