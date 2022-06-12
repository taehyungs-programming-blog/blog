---
layout: default
title: "[구현] Session 구현 - 1"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-17)

---

```cpp
int main()
{
	// ServerService를 만들고
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	// Start 실행
	ASSERT_CRASH(service->Start());

	// ...
```

```cpp
bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	// listener 생성하고
	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());

	// listener를 실행
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}
```

```cpp
bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	// 소켓연결에 관한 부분은 생략

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);

		// AcceptEvent를 등록
		RegisterAccept(acceptEvent);
	}

	return true;
}
```

```cpp
void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;

	// Accept
	if (false == SocketUtils::AcceptEx(_socket, 
                                        session->GetSocket(), 
                                        session->_recvBuffer, 
                                        0, 
                                        sizeof(SOCKADDR_IN) + 16, 
                                        sizeof(SOCKADDR_IN) + 16, 
                                        OUT & bytesReceived, 
                                        static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// 일단 다시 Accept 걸어준다
			RegisterAccept(acceptEvent);
		}
	}
}
```

```cpp
void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}
```

```cpp
void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->session;

	// ...

	session->SetNetAddress(NetAddress(sockAddress));

	// session의 ProcessConnect() 실행
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}
```

```cpp
void Session::ProcessConnect()
{
	_connected.store(true);

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 컨텐츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}
```

```cpp
void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RELEASE_REF
		}
	}
}
```

```cpp
void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}
```

---

## 헷갈리는 부분

* [참고 사이트 🌎](https://snowfleur.tistory.com/116)

<br>

🤷‍♂️ `SocketUtils::AcceptEx`과 `::WSARecv`의 차이점?

```cpp
void Session::RegisterRecv()
{
	// ...

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // ADD_REF

    // ...

	if (SOCKET_ERROR == ::WSARecv(_socket, 
                                    &wsaBuf, 
                                    1, 
                                    OUT &numOfBytes, 
                                    OUT &flags, 
                                    &_recvEvent, 
                                    nullptr))

    // ...
```

```cpp
void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	// ...

	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;

	// ...
```

* `WSASend()`와 `WSARecv()` 같은 비동기 입출력 함수의 송수신 완료 통지를 IOCP에서 받을 수 있다.
* 하지만 `accept()`와 `connect()` 함수는 IOCP를 이용해서 완료 통지를 받을 수 없었다.
* 이러한 문제를 해결하기 위해 MS(Micro Soft)에서 XP부터 확장 API를 제공했는데 그게 바로 `AcceptEx()`와 `ConnectEx()` 함수이다. 이 두 함수는 비동기 입출력 방식으로 작동하는 함수이며 `AcceptEx()`는 `accept`와 차이는 `AcceptEx()`는 미리 두개의 소켓, 리슨 소켓과 접속을 받을 소켓을 미리 준비해야 한다는 점이다. 

 