---
layout: default
title: "06-05. [Func] DispatchPacket"
parent: "(06. Challenge과정에서 사용되는 주요 함수)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* DispatchPacket의 주요 목적은 수신된 패킷을 분해하여 여러 개의 Bunch로 나누고, 각각의 Bunch를 적절한 채널로 전달

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_6_3.png"/>
</p>


```cpp
virtual void DispatchPacket(FBitReader& Reader, int32 PacketId, bool& bOutSkipAck, bool& bOutHasBunchErrors)
{
    // 이런식으로 수신이 된다.
    //               Bunch0                             Bunch1                             Bunch2                 
    //  ┌──────────────────────────────────┬──────────��───────────────────────┬──────────────────────────────────┐
    //  ┼─────────────────┬────────────────┼─────────────────┬────────────────┼─────────────────┬────────────────┤
    //  │   BunchHeader0  │  BunchData0    │   BunchHeader0  │  BunchData0    │   BunchHeader0  │  BunchData0    │
    //  ┼─────────────────┴────────────────┼─────────────────┴────────────────┼─────────────────┴────────────────┤
    //  └──────────────────────────────────┴──────────────────────────────────┴──────────────────────────────────┘
    //
    while (!Reader.AtEnd())
    {
        {
        
            FInBunch Bunch(this);

            int32 IncomingStartPos = Reader.GetPosBits();


            // - 'NMT_Hello' 라 가정하면
            //   Bunch.bOpen = 1
            //   Bunch.bClose = 0
            //   Bunch.bCloseReason = Destroyed
            //   Bunch.bReliable = 1
            //   Bunch.ChIndex = 0
            //   Bunch.bHasPackagemapExports = 0
            //   Bunch.bHasMustBeMappedGUIDs = 0
            //   Bunch.bPartial = 0
            //   Bunch.ChSequence = 898 (*** this can be different on every execution because of Sequence Numbers are initialized randomly by SecretToken)
            //   Bunch.bPartialInitial = 0
            //   Bunch.bPartialFinal = 0
            //   Bunch.ChName = "Control"
            uint8 bControl      = Reader.ReadBit();
            Bunch.bOpen         = bControl ? Reader.ReadBit() : 0;
            Bunch.bClose        = bControl ? Reader.ReadBit() : 0;
            Bunch.bCloseReason = Bunch.bClose ? (EChannelCloseReason)Reader.ReadInt((uint32)EChannelCloseReason::MAX) : EChannelCloseReason::Destroyed;
            Bunch.bReliable     = Reader.ReadBit();
            {
                uint32 ChIndex;
                Reader.SerializeIntPacked(ChIndex);

                if (ChIndex >= (uint32)Channels.Num())
                {
                    Close(ENetCloseResult::BunchBadChannelIndex);
                    return;
                }

                Bunch.ChIndex = ChIndex;
            }

            Bunch.bHasPackageMapExports = Reader.ReadBit();
            Bunch.bHasMustBeMappedGUIDs = Reader.ReadBit();
            Bunch.bPartial = Reader.ReadBit();

            if (Bunch.bReliable)
            {                                                         
                Bunch.ChSequence = MakeRelative(Reader.ReadInt(MAX_CHSEQUENCE), InReliable[Bunch.ChIndex], MAX_CHSEQUENCE);
            }
            else if (Bunch.bPartial)
            {
            }
            else
            {
                Bunch.ChSequence = 0;
            }

            Bunch.bPartialInitial = Bunch.bPartial ? Reader.ReadBit() : 0;
            Bunch.bPartialFinal = Bunch.bPartial ? Reader.ReadBit() : 0;

            if (Bunch.bReliable || Bunch.bOpen)
            {
                UPackageMap::StaticSerializeName(Reader, Bunch.ChName);
            }


            UChannel* Channel = Channels[Bunch.ChIndex];
            int32 BunchDataBits = Reader.ReadInt(UNetConnection::MaxPacket*8);


            const int64 BunchDataBits32BitAligned = (BunchDataBits + 31) & ~31;
            Bunch.ResetData(Reader, BunchDataBits, BunchDataBits32BitAligned);

            if (Bunch.bHasPackageMapExports)
            {

            }

            if (Bunch.bReliable && Bunch.ChSequence <= InReliable[Bunch.ChIndex])
            {
                continue;
            }


            if (Channel == nullptr)
            {
                // 필요한 경우 새 채널 생성
                Channel = CreateChannelByName(Bunch.ChName, EChannelCreateFlags::None, Bunch.ChIndex);
            }

            Bunch.bIgnoreRPCs = bIgnoreRPCs;

            bool bLocalSkipAck = false;
            if (Channel != nullptr)
            {
                // Bunch 전달:
                Channel->ReceivedRawBunch(Bunch, bLocalSkipAck);
            }

            if (bLocalSkipAck)
            {
                bOutSkipAck = true;
            }
        }
    }
}
```