---
layout: default
title: "03-03. InitLocalConnection"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** initialize address resolution for clients connecting to a server */
bool InitLocalConnection(ISocketSubsystem* SocketSubsystem, FSocket* InSocket, const FURL& InURL)
{
    bool bValidInit = true;

    // by default, (RenderState != EAddressResolutionState::Disabled)
    // - update the state as WaitingForResolves
    // - after resolving address, then change the state from 'WaitingForResolves' to 'TryNextAddress'
    if (!IsAddressResolutionEnabled())
    {
        //...
    }
    else
    {
        // 결국 여기만 탄다
        ResolutionState = EAddressResolutionState::WaitingForResolves;
    }

    return bValidInit;
}
```

```cpp
/** resets the FBitWriter to its default state */
virtual void InitSendBuffer()
{
    // FinalBufferSize is the data size which we can put in:
    // - MaxPacket is 1024
    // - MaxPacketHandlerBits is updated in InitHandler() by PacketHandler::GetTotalReservedPacketBits()
    // - FinalBufferSize is in unit 'Bits'
    int32 FinalBufferSize = (MaxPacket * 8) - MaxPacketHandlerBits;

    // initialize the one outgoing buffer
    if (FinalBufferSize == SendBuffer.GetMaxBits())
    {
        // reset all of our values to their initial state without a malloc/free
        // see FBitWriter::Rest() briefly (similar to its constructor)
        SendBuffer.Reset();
    }
    else
    {
        // first time initialization needs to allocate the buffer
        SendBuffer = FBitWriter(FinalBufferSize);
    }

    // see ResetPacketBitCounts briefly, we'll see these variables frequently
    ResetPacketBitCounts();
}
```

* 여기서 부터 SendBuffer준비