---
layout: default
title: "02-11. UNetDriver와 UNetConnection의 관계"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_1_3.png"/>
</p>

```cpp
class UNetDriver 
{
private:
    // 모든 클라이언트 연결을 관리하는 배열
    TArray<UNetConnection*> ClientConnections;

public:
    // 새로운 클라이언트 연결 생성
    UNetConnection* CreateConnection(/*...*/)
    {
        UNetConnection* Connection = NewObject<UIpConnection>();
        Connection->InitBase(this, Socket, URL, /*...*/);
        return Connection;
    }

    // 클라이언트 연결 추가
    void AddClientConnection(UNetConnection* ClientConnection)
    {
        ClientConnections.Add(ClientConnection);
    }

    // 클라이언트 연결 제거
    void RemoveClientConnection(UNetConnection* ClientConnection)
    {
        ClientConnections.Remove(ClientConnection);
    }
};
```

```cpp
class UNetConnection
{
protected:
    // 이 연결을 관리하는 드라이버에 대한 참조
    UNetDriver* Driver;

public:
    virtual void InitBase(UNetDriver* InDriver, /*...*/)
    {
        Driver = InDriver;
        // 기타 초기화...
    }

    // 연결 종료
    virtual void Close()
    {
        if (Driver)
        {
            Driver->RemoveClientConnection(this);
        }
    }
};
```