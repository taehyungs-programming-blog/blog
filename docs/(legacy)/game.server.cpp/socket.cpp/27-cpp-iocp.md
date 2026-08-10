---
layout: default
title: "02. TCP란?"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* Client와 Server가 연결되면 아래와 같이 Kernel 영역에 Recv, Send Buffer가 생성된다.

```
|   Client   |           |   Server   |
|            |           |            |
|            |           |            |
|            |           |            |
--------------           -------------- 
|   Kernel   |           |   Kernel   |
|            |           |            |
|  Recv Buf  |           |  Recv Buf  |
|  Send Buf  |           |  Send Buf  |
|            |           |            |
--------------           --------------
```

```
// 만약 Send를 한다고 하면 Client가 직접 Server로 보내는 것이 아니라
// Send Buf에 데이터를 채우게 된다.
// 따라서 아래에 나오겠지만 Send를 한다고해서 코드가 블럭이 되지않는다
    // (단순 Send Buf에 데이터를 옮기기만 했기에)
|   Client             |           |   Server   |
|                      |           |            |
|                      |           |            |
|                      |           |            |
------------------------           --------------
|   Kernel             |           |   Kernel   |
|                      |           |            |
|  Recv Buf            |           |  Recv Buf  |
|  Send Buf[SendData]  |           |  Send Buf  |
|                      |           |            |
------------------------           --------------
```

```
// 반대로 Recv를 했다면?
// Recv Buffer에서 데이터를 긁어온다.
// 없다면 무한대기를 하게된다.
|   Client   |           |   Server             |
|            |           |                      |
|            |           |                      |
|            |           |                      |
--------------           ------------------------
|   Kernel   |           |   Kernel             |
|            |           |                      |
|  Recv Buf  |           |  Recv Buf[RecvData]  |
|  Send Buf  |           |  Send Buf            |
|            |           |                      |
--------------           ------------------------
```

```
// 만약 Send Buf가 다 차버린 상황이라면?
// Send Buf에 자리가 날때까지 코드가 대기하게된다.
|   Client                 |           |   Server   |
|                          |           |            |
|   [SendData]             |           |            |
|                          |           |            |
----------------------------           --------------
|   Kernel                 |           |   Kernel   |
|                          |           |            |
|  Recv Buf                |           |  Recv Buf  |
|  Send Buf[SendDataFull]  |           |  Send Buf  |
|                          |           |            |
----------------------------           --------------
```

* 그런데 이런식으로 코드가 블럭되는 형태로 게임을 구현하는게 좋은방법일까?? 
    * 👉 **그에 대한 설명은 다음강좌에서 진행**

---

## 구현

### Client

```cpp
// ...
int main()
{
    // ... [Socket연결 관련해선 앞강 참조]
    // ---------------------------
    // 연결 성공! 이제부터 데이터 송수신 가능!
    cout << "Connected To Server!" << endl;

    while (true)
    {
        // TODO
        char sendBuffer[100] = "Hello World!";
        // 100바이트 * 10 = 1000바이트를 거의 한 순간에 보내게 된다.
        for (int32 i = 0; i < 10; i++)
        {
            int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
            // 서버에서 recv를 하지 않아도 아래로 내려간다(블락이 안됨)
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }
        }
        
        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
        /*
            // echo server를 만들고 싶다면?
            char recvBuffer[1000];
            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            // 여기서 데이터를 받지 않으면 무한대기
            if (recvLen <= 0)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Recv ErrorCode : " << errCode << endl;
                return 0;
            }
            cout << "Recv Data! Data = " << recvBuffer << endl;
            cout << "Recv Data! Len = " << recvLen << endl;
        */
        this_thread::sleep_for(1s);
    }
    // ...
}
```

### Server

```cpp
// ...
int main()
{
    // ... [Socket연결 앞강 참조]
    // -----------------------------
    while (true)
    {
        // ... [Client Connect 앞강 참조]
        // TODO
        while (true)
        {
            char recvBuffer[1000];
            this_thread::sleep_for(1s);
            // 100바이트 * 10 = 1000바이트를 거의 한 순간에 보냈는데
            // 100바이트씩 10번 들어올까??
            // Nope! = 1000바이트로 들어온다.
            // RecvBuffer를 이용하기 때문!
            // 여기서 또 생기는 문제는 1000바이트로 들어온 데이터를 어떻게 송신자가 보낸 형태로 잘 자르냐이다(다음에 설명)
            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (recvLen <= 0)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Recv ErrorCode : " << errCode << endl;
                return 0;
            }
            cout << "Recv Data! Data = " << recvBuffer << endl;
            cout << "Recv Data! Len = " << recvLen << endl;
            /* 에코서버
            int32 resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }*/
        }
    }
    // -----------------------------
    // 윈속 종료
    ::WSACleanup();
}
```

---

## kernel 영역의 buffer가 다 차면 어떻게 될까?

* 블로킹 모드: 
    * 기본적으로 소켓은 블로킹 모드로 설정되어 있습니다. 
    * 이 경우 send 함수는 버퍼에 공간이 생길 때까지 대기하게 됩니다. 즉, 함수 호출이 반환되지 않고 대기 상태에 머무르게 됩니다.
* 논블로킹 모드: 
    * 소켓을 논블로킹 모드로 설정한 경우, 버퍼가 가득 찼을 때 send 함수는 즉시 반환되고 특정 오류 코드 (예: WSAEWOULDBLOCK)를 반환합니다.

---

## kernel 영역의 buffer 사이즈를 확인할 수 있나?

```cpp
// 확인
int sendBufferSize;
int size = sizeof(sendBufferSize);
getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&sendBufferSize, &size);

// 변경
int newBufferSize = 2048; // 예를 들어 버퍼 크기를 2048 바이트로 설정
setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&newBufferSize, sizeof(newBufferSize));
```