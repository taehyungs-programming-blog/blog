---
layout: default
title: "03-12. UChannel"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** represents a range of PacketIds, inclusive */
/** PacketId의 범위를 나타내며, 경계값을 포함합니다 */
struct FPacketIdRange
{
    int32 First;
    int32 Last;
};

/** base class of communication channels */
/** 통신 채널들의 기본 클래스 */
class UChannel : public UObject
{
    /** initialize this channel for the given connection and index */
    /** 주어진 연결과 인덱스에 대해 이 채널을 초기화합니다 */
    virtual void Init(UNetConnection* InConnection, int32 InChIndex, EChannelCreateFlags CreateFlags)
    {
        // let's see member variables briefly
        // 멤버 변수들을 간단히 살펴봅시다
        Connection      = InConnection;
        ChIndex			= InChIndex;
        OpenedLocally	= EnumHasAnyFlags(CreateFlags, EChannelCreateFlags::OpenedLocally);
        OpenPacketId	= FPacketIdRange();
    }

    /** owner connection */
    /** 소유자 연결 */
    TObjectPtr<UNetConnection> Connection;

    /** set when placed in the actor channel pool */
    /** 액터 채널 풀에 배치될 때 설정됩니다 */
    uint32 bPooled : 1;

    /** whether channel was opened by locally or by remote */
    /** 채널이 로컬에서 열렸는지 원격에서 열렸는지 여부 */
    uint32 OpenLocally : 1;

    /** index of this channel */
    /** 이 채널의 인덱스 */
    int32 ChIndex;

    /** 
     * if OpenLocally is true, this is the packet we sent the bOpen bunch 'on' 
     * otherwise, it's the packet we received the bOpen bunch 'on'
     */
    /** 
     * OpenLocally가 true이면, 이는 우리가 bOpen 번치를 보낸 패킷입니다
     * 그렇지 않으면, 이는 우리가 bOpen 번치를 받은 패킷입니다
     */
    FPacketIdRange OpenPacketId;
};
```