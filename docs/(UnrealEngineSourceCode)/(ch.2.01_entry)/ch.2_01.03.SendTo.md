---
layout: default
title: "01-03. SendTo"
parent: "([Network] 01.SocketSubsystemModule)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// the wrapper of UDP's sendto 
virtual bool SendTo(const uint8* Data, int32 Count, int32& BytesSent, const FInternetAddr& Destination) override
{
    // as we covered, the UDP send protocol is very simple just set address and send it and DONE!
    const FInternetAddrBSD& BSDAddr = static_cast<const FInternetAddrBSD&>(Destination);

    // write the data and see how much was written
    // call sendto()
    // - note that Count's unit is 'BYTE'
    // - SendFlags is usually 'None(==0)'
    BytesSent = sendto(Socket, (const char*)Data, Count, SendFlags, (const sockaddr*)&(BSDAddr.Addr), BSDAddr.GetStorageSize());

    return BytesSent >= 0;
}
```

```cpp
virtual bool RecvFrom(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddr& Source, ESocketReceiveFlags::Type Flags = ESocketReceiveFlags::None) override
{
    bool bSuccess = false;
    const bool bStreamSocket = (SocketType == SOCKTYPE_Streaming);

    // usually Flags is '0: None', it is useful to know some flags used in recvfrom()
    const int32 TranslatedFlags = TranslateFlags(Flags);

    FInternetAddrBSD& BSDAddr = static_cast<FInternetAddrBSD&>(Source);
    SOCKLEN Size = sizeof(sockaddr_storage);
    sockaddr* Addr = (sockaddr*)&(BSDAddr.Addr);

    // read into the buffer and set the source address
    // recvfrom call: UDP recv
    BytesRead = recvfrom(Socket, (char*)Data, BufferSize, TranslatedFlags, Addr, &Size);
    if (BytesRead >= 0)
    {
        bSuccess = !bStreamSocket || (BytesRead > 0);
    }

    return bSuccess;
}
```