---
layout: default
title: "[구현] IOCP Core"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-15)

---

## 하고자 하는 것.

😺 `IocpCore` 클래스를 두고 iocp를 관리하고자 한다

```
[ IocpObject ] ---> [ Listener ]
                |
                --> [ Session ]

[ IocpCore ] -> Dispatch Loop를 돌며 IocpObject를 Dispatch한다.
```

* 눈 여겨 볼 부분
    * (1) IocpCore에 IocpObject를 등록하는 부분(Listner, Session을 어떻게 등록하는지)
    * (2) IocpCore를 어떻게 Dispatch하는지
    * (3) IocpCore에 등록된 IocpObject를 어떻게 파싱하는지

```cpp
// ...

int main()
{
    // 변경1)
	// SockUtil을 직접사용하지 않고 Listener를 통해 사용할 예정
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
                    // 변경2)
                    // 전역으로 IocpCore를 두고
                    // 여러 쓰레드로 Dispatch하며 Queue된 데이터 처리
					GIocpCore.Dispatch();
				}				
			});
	}	

	GThreadManager->Join();
}
```

```cpp
// ...

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	
	IocpEvent* iocpEvent = nullptr;

    // Dispatch내부는 Completion Port Queue를 읽어오는 정도
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
        // queue에 있던 iocpobject의 dispatch를 재 호출
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}

    // ...
```

😺 (???) `Listener`를 언제 IocpCore에 등록한 거지 ???

```cpp
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    // 더미 세션을 만들고
	Session* session = xnew<Session>();

    // 이벤트에 담에서
	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD bytesReceived = 0;

    // Accept Event가 발생시 알려달라고 등록한다.
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
```

```cpp
int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
                    // 실제 Accept Event 발생시 IocpCore에서 Dispatch가 알려주며
					GIocpCore.Dispatch();
				}				
			});
	}	
```

```cpp
// Listner에게 들어오게 된다.
void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}
```

---

## IOCPCore 내부

```cpp
#pragma once

/*----------------
	IocpObject
-----------------*/

class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(class IocpObject* iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE		_iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;
```

```cpp
#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

// TEMP
IocpCore GIocpCore;

/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObject* iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle() /*socket*/, 
								_iocpHandle, 
								reinterpret_cast<ULONG_PTR>(iocpObject), /*key*/
								0);

	/*
	
	// 대략 IocpObject는 아래와 같이 사용됨

	bool Listener::StartAccept(NetAddress netAddress)
	{
		_socket = SocketUtils::CreateSocket();
		if (_socket == INVALID_SOCKET)
			return false;

		// 등록
		if (GIocpCore.Register(this) == false)
			return false;

	// ...

	// Dispatch
	void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
	{
		// IocpEvent를 통해 필요한 정보를 넘긴다
		ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
		AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
		ProcessAccept(acceptEvent);
	}

	*/
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, 
								OUT &numOfBytes, 
								OUT reinterpret_cast<PULONG_PTR>(&iocpObject), 	// CreateIoCompletionPort할때 key 정보
								OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), 
								timeoutMs))
	{
		// Dispatch를 하면서 iocpEvent를 매개변수로 넣는다
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}

	/*
		참고로 현재 queue된 데이터는 accept밖에없고 아래처럼 등록
		void Listener::RegisterAccept(AcceptEvent* acceptEvent)
		{
			Session* session = xnew<Session>();

			acceptEvent->Init();
			acceptEvent->SetSession(session);

			DWORD bytesReceived = 0;
			if (false == SocketUtils::AcceptEx(_socket, 
										session->GetSocket(), 
										session->_recvBuffer, 
										0, 
										sizeof(SOCKADDR_IN) + 16, 
										sizeof(SOCKADDR_IN) + 16, 
										OUT & bytesReceived, 
										static_cast<LPOVERLAPPED>(acceptEvent)))
	*/


	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : 로그 찍기
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
```

```cpp
#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void		Init();
	EventType	GetType() { return _type; }

protected:
	EventType	_type;
};

/*----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

	void		SetSession(Session* session) { _session = session; }
	Session*	GetSession() { return _session; }

private:
	// session 정보와 같은 필요한 정보를 IocpEvent를 통해 넘긴다
	Session*	_session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }
};
```

```cpp
// ...

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	// Event에서 관리중인 Session주소를 빼내온다
	Session* session = acceptEvent->GetSession();

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		// 실패 시 다시 Register
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));

	cout << "Client Connected!" << endl;

	// TODO

	RegisterAccept(acceptEvent);
}

// ...
```