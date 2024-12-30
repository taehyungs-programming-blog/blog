---
layout: default
title: "02-03. UNetConnection"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UNetConnection 설계 이념

* 연결 상태 관리
    * **단일 클라이언트-서버 간 연결 표현** : 요게 핵심이겠지?
    * 패킷의 신뢰성 있는 전송 보장
    * 채널 시스템 관리
* 데이터 무결성
    * 패킷 순서 보장
    * 패킷 손실 복구
    * 중복 패킷 처리
* 성능 최적화
    * 버퍼링과 배치 처리
    * 대역폭 관리
    * 우선순위 기반 전송

```cpp
class UNetConnection : public UObject
{
private:
    // 연결 상태 관리
    EConnectionState ConnectionState;
    double LastReceiveTime;
    double LastSendTime;
    
    // 채널 관리
    TArray<UChannel*> Channels;
    TArray<UChannel*> OpenChannels;
    
    // 패킷 관리
    FBitWriter SendBuffer;
    int32 OutPacketId;
    int32 InPacketId;
    
    // 신뢰성 관리
    struct FUnackedPacket
    {
        double SentTime;
        TArray<uint8> Data;
        int32 RetryCount;
    };
    TMap<int32, FUnackedPacket> UnackedPackets;

public:
    // 초기화 및 정리
    virtual bool InitConnection(UNetDriver* InDriver, const FURL& InURL)
    {
        Driver = InDriver;
        ConnectionState = EConnectionState::Initialized;
        
        // 기본 채널 설정
        CreateChannel(CHTYPE_Control, true, 0);
        return true;
    }

    // 패킷 전송
    virtual void SendPacket()
    {
        if (SendBuffer.GetNumBits() > 0)
        {
            // 패킷 헤더 추가
            FPacketHeader Header;
            Header.PacketId = ++OutPacketId;
            Header.bReliable = HasReliableData();
            
            // 신뢰성 있는 전송 처리
            if (Header.bReliable)
            {
                FUnackedPacket& Unacked = UnackedPackets.Add(Header.PacketId);
                Unacked.SentTime = FPlatformTime::Seconds();
                Unacked.Data = SendBuffer;
            }
            
            // 실제 전송
            LowLevelSend(Header, SendBuffer);
            LastSendTime = FPlatformTime::Seconds();
        }
    }

    // 패킷 수신
    virtual void ReceivedPacket(FBitReader& Reader)
    {
        // 패킷 헤더 파싱
        FPacketHeader Header;
        Reader >> Header;
        
        // 순서 확인
        if (Header.PacketId <= InPacketId)
        {
            // 중복 패킷 처리
            return;
        }
        
        // 신뢰성 처리
        if (Header.bReliable)
        {
            // ACK 전송
            SendAck(Header.PacketId);
        }
        
        // 채널로 데이터 전달
        DispatchPacketToChannels(Reader);
        
        InPacketId = Header.PacketId;
        LastReceiveTime = FPlatformTime::Seconds();
    }

    // 채널 관리
    UChannel* CreateChannel(EChannelType ChType, bool bOpenedLocally, int32 ChIndex)
    {
        UChannel* Channel = NewObject<UChannel>();
        Channel->Init(this, ChType, bOpenedLocally, ChIndex);
        Channels.Add(Channel);
        
        if (Channel->IsOpen())
        {
            OpenChannels.Add(Channel);
        }
        
        return Channel;
    }

protected:
    // 신뢰성 관리
    void UpdateUnackedPackets()
    {
        const double CurrentTime = FPlatformTime::Seconds();
        const double TimeoutThreshold = 1.0; // 1초
        
        for (auto It = UnackedPackets.CreateIterator(); It; ++It)
        {
            FUnackedPacket& Packet = It.Value();
            
            if (CurrentTime - Packet.SentTime > TimeoutThreshold)
            {
                if (Packet.RetryCount < MaxRetryCount)
                {
                    // 재전송
                    LowLevelSend(It.Key(), Packet.Data);
                    Packet.SentTime = CurrentTime;
                    Packet.RetryCount++;
                }
                else
                {
                    // 최대 재시도 횟수 초과
                    HandleConnectionTimeout();
                }
            }
        }
    }

    // 대역폭 관리
    void UpdateBandwidthUsage()
    {
        // 현재 대역폭 사용량 계산
        float CurrentBandwidth = CalculateBandwidthUsage();
        
        if (CurrentBandwidth > MaxBandwidth)
        {
            // 대역폭 조절
            ThrottleConnection();
        }
    }
};
```