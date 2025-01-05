---
layout: default
title: "02-05. PacketHandler"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## PacketHandler 설계 이념

* 모듈식 설계
    * 핸들러 컴포넌트 체인
    * 플러그인 방식의 기능 확장
    * 유연한 패킷 처리 파이프라인
* 보안과 최적화
    * 패킷 암호화/복호화
    * 데이터 압축/해제
    * 무결성 검증

```cpp
class FPacketHandler
{
private:
    // 핸들러 컴포넌트 체인
    TArray<TSharedPtr<HandlerComponent>> HandlerComponents;
    
    // 패킷 처리 상태
    bool bInitialized;
    bool bEnabled;

public:
    // 패킷 처리
    bool ProcessPacket(uint8* Data, int32& DataLen)
    {
        // 모든 핸들러 컴포넌트를 통해 패킷 처리
        for (auto& Component : HandlerComponents)
        {
            if (!Component->ProcessPacket(Data, DataLen))
            {
                return false;
            }
        }
        return true;
    }

    // 아웃바운드 패킷 처리
    void ProcessOutgoing(FBitWriter& Writer)
    {
        for (auto& Component : HandlerComponents)
        {
            Component->ProcessOutgoing(Writer);
        }
    }

    // 인바운드 패킷 처리
    bool ProcessIncoming(FBitReader& Reader)
    {
        for (auto& Component : HandlerComponents)
        {
            if (!Component->ProcessIncoming(Reader))
            {
                return false;
            }
        }
        return true;
    }
};
```

---

```cpp
class UNetConnection
{
private:
    TUniquePtr<FPacketHandler> PacketHandler;

public:
    void InitializeHandler()
    {
        PacketHandler = MakeUnique<FPacketHandler>();
        
        // 필요한 핸들러 컴포넌트 추가
        PacketHandler->AddHandler<FEncryptionHandlerComponent>();
        PacketHandler->AddHandler<FCompressionHandlerComponent>();
        PacketHandler->AddHandler<FAuthHandlerComponent>();
    }

    void SendPacket(FBitWriter& Writer)
    {
        // 아웃바운드 패킷 처리
        if (PacketHandler)
        {
            PacketHandler->ProcessOutgoing(Writer);
        }
        
        // 패킷 전송
        LowLevelSend(Writer.GetData(), Writer.GetNumBytes());
    }

    bool ReceivedPacket(FBitReader& Reader)
    {
        // 인바운드 패킷 처리
        if (PacketHandler && !PacketHandler->ProcessIncoming(Reader))
        {
            // 패킷 처리 실패
            return false;
        }
        
        // 패킷 처리 계속
        return ProcessReceivedPacket(Reader);
    }
};
```

---

## 그런데 왜? PacketHandler에서 BeginHandshaking을 관리할까?

* 중앙 집중식 핸드셰이크 관리
    * PacketHandler는 여러 HandlerComponent들을 관리하는 중앙 매니저 역할
    * 각 HandlerComponent들의 핸드셰이크 과정을 조율하고 순서를 제어
* 일관된 초기화 프로세스

```cpp
void BeginHandshaking(FPacketHandlerHandshakeComplete InHandshakeDel)
{
    bBeganHandshaking = true;
    HandshakeCompleteDel = InHandshakeDel;

    // HandlerComponent들을 순차적으로 초기화
    for (int32 i = HanderComponents.Num() - 1; i >= 0; --i)
    {
        HandleComponent& CurComponent = *HandlerComponents[i];
        if (CurComponent.RequiresHandshake() && !CurComponent.IsInitialized())
        {
            CurComponent.NotifyHandshakeBegin();
            break;
        }
    }
}
```