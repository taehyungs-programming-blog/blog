---
layout: default
title: "02-04. PacketHandler"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review 2025 Ver. 🐍)"
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