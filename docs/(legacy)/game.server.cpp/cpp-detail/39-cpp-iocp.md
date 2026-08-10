---
layout: default
title: "[구현] Server Service"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-16)

---

## 하고자 하는 것!

```cpp
class GameSession : public Session
{
    // 아직까진 GameSession을 Dispatch하지는 않음.(이후 강의에 넣을 예정)
};


int main()
{
    // 이제 소켓과 관련된 내용을 ServerServiceRef를 통해 진행
	// using ServerServiceRef	= std::shared_ptr<class ServerService>;
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // Session Factory를 전달
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
                    // service를 통해 Iocp접근 가능
					service->GetIocpCore()->Dispatch();
				}				
			});
	}	

	GThreadManager->Join();
}
```

🤷‍♂️ 간단하게 몇 가지만 정리하고 들어간다.<Br>
🤷‍♂️ `MakeShared<IocpCore>()` 를 넘기는 이유? 👉 말 그대로 IocpCore를 생성후 넘긴다.<br>
🤷‍♂️ `MakeShared<GameSession>` 를 넘기는 이유? 👉 

```cpp
Service::Service(ServiceType type,
 NetAddress address, 
 IocpCoreRef core, 
 SessionFactory factory, 
 int32 maxSessionCount)
	: _type(type), 
    _netAddress(address), 
    _iocpCore(core), 
    _sessionFactory(factory),           // GameSession을 넘기면 factory로 들어가게 되고
    _maxSessionCount(maxSessionCount)
{

}

// SessionFactory는 shared_ptr의 함수포인터이다.
using SessionRef		= std::shared_ptr<class Session>;
using SessionFactory = function<SessionRef(void)>;

// 따라서 아래와 같이 호출 시 Session을 생성하게 된다
SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();
    // ...
```

---

## Service

```cpp
#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

/*-------------
	Service
--------------*/

using SessionFactory = function<SessionRef(void)>;
						// MakeShare<class>가 들어온다면 SessionFactory() = MakeShare<class>() 가 된다

class Service : public enable_shared_from_this<Service>	/* shared_ptr의 this를 사용하겠다 */
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	SessionRef			CreateSession();
	void				AddSession(SessionRef session);
	void				ReleaseSession(SessionRef session);
	int32				GetCurrentSessionCount() { return _sessionCount; }
	int32				GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType			GetServiceType() { return _type; }
	NetAddress			GetNetAddress() { return _netAddress; }
	IocpCoreRef&		GetIocpCore() { return _iocpCore; }

protected:
	USE_LOCK;
	ServiceType			_type;
	NetAddress			_netAddress = {};
	IocpCoreRef			_iocpCore;

	// service에서 Session정보와 Listener 정보를 갖는다
	Set<SessionRef>		_sessions;
	int32				_sessionCount = 0;
	int32				_maxSessionCount = 0;
	SessionFactory		_sessionFactory;
};

/*-----------------
	ClientService
------------------*/

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};


/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	ListenerRef		_listener = nullptr;
};
```

```cpp
#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

/*-------------
	Service
--------------*/

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
}

void Service::CloseService()
{
	// TODO
}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

/*-----------------
	ClientService
------------------*/

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool ClientService::Start()
{
	// TODO
	return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Server, address, core, factory, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	// TODO

	Service::CloseService();
}
```

```cpp
// ...

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	IocpObjectRef	owner;
	// IocpEvent에서 IocpObject정보를 갖고있는다
		// IocpEvent를 생성시에 owner를 항상연결해줘야 한다

	/*
	
	bool Listener::StartAccept(ServerServiceRef service)
	{
		// ...

		const int32 acceptCount = _service->GetMaxSessionCount();
		for (int32 i = 0; i < acceptCount; i++)
		{
			AcceptEvent* acceptEvent = xnew<AcceptEvent>();

			// owner 정보는 StartAccept시에 넣음
			acceptEvent->owner = shared_from_this();
			_acceptEvents.push_back(acceptEvent);
			RegisterAccept(acceptEvent);
		}

	*/

	/*
	
	// owner 정보의 사용은
	bool IocpCore::Dispatch(uint32 timeoutMs)
	{
		DWORD numOfBytes = 0;
		ULONG_PTR key = 0;	
		IocpEvent* iocpEvent = nullptr;

		if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
		{
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
		}
	
	*/
};

// ...
```
