---
layout: default
title: "[구현] JobQueue - 2(std::apply를 이용한 JobQueue 구현)"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-30)

---

🐶 현재 코드의 문제는 Job이 생성될때마다 클래스를 새로 생성해야 한다.

```cpp
// 매번 이런걸 만들어야 한다는 말...
class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer) : _room(room), _sendBuffer(sendBuffer)
	{
	}

	virtual void Execute() override
	{
		_room.Broadcast(_sendBuffer);
	}

public:
	Room& _room;
	SendBufferRef _sendBuffer;
};
```

🐶 우선 지금할 내용을 JobQueue Ver1.2 정도라 생각하고 위 문제의 해결방법은 공용클래스를 선언하고 JobQueue를 관리할 예정임을 알자

---

🐶 흠... 이렇게 하면 좋겠는걸 Job이라는 Fuctor를 만들고 마치 Fuctor를 호출하면 Queue에 쌓인 Job이 호출되게?

```cpp
// 마치 이런 함수를 호출하듯 fuctor를 만들고 싶다
void HealByValue(int64 target, int32 value)
{
    // ...
}

// 그럼 Job이라는 Fuctor만 만들면 될까?
template<typename Ret, typename T1, typename T2>
class Job
{
public:
    Ret operator()(T1 t1, T2 t2)
    {
        // ...
    }

    // or 이전방식대로 Execute를 이용해도 됨
    Ret Execute(T1 t1, T2 t2)
    {
        // ...
    }
};

int main()
{
    Job<void, int64, int32> job;
    job(100, 10);   
}
```

🐶 하지만... 위 코드는 예상하듯 문제가 많다<br>
🐶 우선은 매개변수가 2개만 들어올것이라는 확신이 없다(여러개를 받아야할 수도 있다.)

```cpp
template<typename Ret, typename... Args>
class Job
{
public:
    Ret operator()(Args... args)
    {
        // do something
    }

    Ret Execute(Args... args)
    {
        // do something
    }
};
```

🐶 그럼 매개변수 문제는 해결됐고<br>
🐶 `do something`이 들어갈 자리에 우리가 원하는 코드를 넣어야할텐데 그 문제는??<br>
🐶 **함수포인터**를 이용해보자


```cpp
template<typename Ret, typename... Args>
class FuncJob   // 클래스이름을 바꿈
{
    using FuncType = Ret(*)(Args...);

public:
    FuncJob(FuncType func) : _func(func) {}

    Ret operator()(Args... args)
    {
        _func(args...);
    }

    Ret Execute(Args... args)
    {
        _func(args...);
    }

private:
    FuncType _func;
};

// ...

void HealByValue(int64 target, int32 value)
{
    // ...
}

int main()
{
    Job<void, int64, int32> job(HealByValue);
    job(100, 10);   
}
```

🐶 좋다! 거의 해결된거 같은데 ... `job(100, 10)`를 호출시에 100, 10을 넘기는데 매개변수만 넣어두고 나중에 호출해야할 함수가 있을수 있다. 매개변수를 지역변수로 관리했으면 한는데 ... ?

```cpp
template<typename Ret, typename... Args>
class FuncJob
{
    using FuncType = Ret(*)(Args...);

public:
    FuncJob(FuncType func) : _func(func), _tuple(args...) {}

    Ret operator()()
    {
        // C++17 이후부터는 아래 표현이 가능
        std::apply(_func, _tuple);
    }

    Ret Execute()
    {
        // C++17 이후부터는 아래 표현이 가능
        std::apply(_func, _tuple);
    }

private:
    FuncType _func;
    // tuple에 Args를 보관할 예정
    std::tuple<Args...> _tuple;
};

// ...

int main()
{
    Job<void, int64, int32> job(HealByValue, 100, 10);

    // do others

    job.Execute();   
}
```

🐶 `std::apply(_func, _tuple);`가 C++17기준이라 했는데 C++17 이전에는 어떻게 썼을까?<br>
🐶 아래에 코드를 남기는데 ... 그냥 C++17쓰는게 마음 편할지도?

```cpp
// 사전에 알아야 할 부분이 있는데
    // tuple : 여러개의 자료형을 관리
auto tup = std::tuple<int32, int32>(1, 2);
    // 뭐 이런식으로 데이터를 빼올수 있음
auto val0 = std::get<0>(tup);
auto val1 = std::get<1>(tup);
```

```cpp
// C++11 apply
template<int... Remains>
struct seq
{};

template<int N, int... Remains>
struct gen_seq : gen_seq<N-1, N-1, Remains...>
{};

template<int... Remains>
struct gen_seq<0, Remains...> : seq<Remains...>
{};

/*
    gen_seq<3> 을 선언시
    : gen_seq<2, 2> 를 상속 받게 된다.
        N == 2가 되고 다시
    : gen_seq<1, 1, 2> 를 상속 받게 된다.
        N == 1이 되고 다시
    : gen_seq<0, 0, 1, 2> 를 상속 받게 된다.
        N == 0일시
    seq<0, 1, 2> 를 상속 받는다 (-> 이걸 만드려고 gen_seq를 호출함.)
*/

template<typename Ret, typename... Args>
void xapply(Ret(*func)(Args...), std::tuple<Args...>& tup)
{
	xapply_helper(func, gen_seq<sizeof...(Args)>(), tup);
    /*
        예를들어
        xapply(func), int, int, int)가 들어올 경우
        xapply_helper(func, gen_seq<3>(), tup); 이 되기에
        xapply_helper(func, seq<0, 1, 2>, tup); 가 된다
    */
}

template<typename F, typename... Args, int... ls>
void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
{
    // func(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup) ... )
	(func)(std::get<ls>(tup)...);
}

template<typename T, typename Ret, typename... Args>
void xapply(T* obj, Ret(T::*func)(Args...), std::tuple<Args...>& tup)
{
    // 멤버 함수를 대비
	xapply_helper(obj, func, gen_seq<sizeof...(Args)>(), tup);
}

template<typename T, typename F, typename... Args, int... ls>
void xapply_helper(T* obj, F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(obj->*func)(std::get<ls>(tup)...);
}
```

🐶 멤버 함수도 대비를 해야한다.

```cpp
template<typename T/*객체도 필요*/, typename Ret, typename... Args>
class MemberJob
{
    using FuncType = Ret(T::*)(Args...);

public:
    MemberJob(T* obj, FuncType func, Args... args) : _obj(obj), _func(func), _tuple(args...) {}

    Ret operator()()
    {
        std::apply(_func, _tuple);
    }

    Ret Execute()
    {
        std::apply(_func, _tuple);
    }

private:
    T*                  _obj;
    FuncType            _func;
    std::tuple<Args...> _tuple;
};

// ...

Kning k1;
MemberJob job(&k1, &Knight::HealMe, 10);

job.Execute();
```

사용은 이렇게 된다.

```cpp
class Room
{
    // ...
    template<typename T, typename Ret, typename... Args>
    void PushJob(Ret(T::*memFunc)(Args...), Args... args)
    {
        auto job = MakeShared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
        _jobs.Push(job);
    }
```

```cpp
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << pkt.msg() << endl;

	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);

	//GRoom.PushJob(MakeShared<BroadcastJob>(GRoom, sendBuffer));
	GRoom.PushJob(&Room::Broadcast, sendBuffer);

	return true;
}
```

🐶 이제 모든 문제가 해결됐을까?<br>
🐶 lambda capture를 이용해서 좀 더 효율적인 코드를 작성해 보고자 한다.
