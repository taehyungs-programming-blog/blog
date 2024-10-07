---
layout: default
title: "02-10. StatelessConnectHandlerComponent"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/**
 * PacketHandler component for implementing a stateless (non-memory-consuming) connection handshake
 */
class StatelessConnectHandlerComponent : public HandlerComponent
{
    StatelessConnectHandlerComponent()
        : HandlerComponent(FName(TEXT("StatelessConnectHandlerComponent")))
        , ActiveSecret(255) // mark it as uninitialized by setting '255'
    {
        // - bActive means StatelessConnectHandlerComponent is going to involve incoming/outgoing packets
        // - bRequiresHandshake to initialize StatelessConnectHandlerComponent, the component will use the way of handshake
        bActive = true;
        bRequiresHandshake = true;
    }

    /** generate a new HandshakeSecret value */
    void UpdateSecret()
    {
        // on first update, update both secrets
        // as we saw, the meaning of ActiveSecret having '255' is need-to-initialize
        if (ActiveSecret == 255)
        {
            // NOTE: the size of this may be excessive:
            // see SECRET_BYTE_SIZE briefly (== 20)
            HandshakeSecret[0].AddUninitialized(SECRET_BYTE_SIZE);
            HandshakeSecret[1].AddUninitialized(SECRET_BYTE_SIZE);

            // generate total 20 secret values in HandshakeSecret token
            TArray<uint8>& CurArray = HandshakeSecret[1];
            for (int32 i=0; i<SECRET_BYTE_SIZE; i++)
            {
                CurArray[i] = FMath::Rand() % 255;
            }

            // set current active-secret index as 0
            ActiveSecret = 0;
        }
        else
        {
            // swap between 0 and 1
            ActiveSecret = (uint8)!ActiveSecret;
        }

        // When UpdateSecret() method is called, update HandshakeSecret token
        TArray<uint8>& CurArray = HandshakeSecret[ActiveSecret];
        for (int32 i=0; i<SECRET_BYTE_SIZE; i++)
        {
            CurArray[i] = FMath::Rand() % 255;
        }
    }

    /** sets the net driver this handler is associated with */
    void SetDriver(UNetDriver* InDriver)
    {
        // cache NetDriver to associate with
        Driver = InDriver;

        if (Handler->Mode == UE::Handler::Mode::Server)
        {
            // note that we're in StatelessComponent
            // - but making sure that Driver's StatelessConnectComponent is same with 'this'
            StatelessConnectHandlerComponent* StatelessComponent = Driver->StatelessConnectComponent.Pin().Get();
            if (StatelessComponent)
            {
                if (StatelessComponent == this)
                {
                    // to prepare handshake process, generate secret token
                    // - see UpdateSecret ***
                    UpdateSecret();
                }
            }
        }
        else // Handler->Mode == Handler::Mode::Client
        {
            // we'll cover this later
            //...
        }
    }

    virtual void Initialize() override
    {
        // on the server, initializes immediately:
        // - clientside doesn't initialize until handshake completes
        // server-side StatelessConnectHandlerComponent is initialized 
        // - But client-side StatelessConnectHandlerComponent's initialization completion is delayed until the Handshake process completes
        if (Handler->Mode == UE::Handler::Mode::Server)
        {
            Initialized();
        }
    }

    virtual int32 GetReservedPacketBits() const override
    {
        // count all base bits additions which affect NetConnection packets, regardless of handshake protocol version - as this is called upon construction
        // 
        // - MagicHeader.Num() == 0
        // - SessionIDSizeBits and ClientIDSizeBits are declared as static const below
        int32 ReturnVal = MagicHeader.Num() + SessionIDSizeBits + ClientIDSizeBits + 1 /* bHandshakePacket */;
        return ReturnVal;
    }

    /** the net driver associated with this handler - for performing connectionless sends */
    UNetDriver* Driver;

    /**
     * SERVER-SIDE variables
     */

    /** the serverside-only 'secret' value, used to help with generating cookies */
    TArray<uint8> HandshakeSecret[SECRET_COUNT];

    /** which of the two secret values above is active (values are changed frequently, to limit replay attacks) */
    uint8 ActiveSecret;

    /** the size of the session id in packets (WARNING: Adjusting this is a net compatibility break) */
    static constexpr uint32 SessionIDSizeBits = 2;

    /** the size of the connection id in packets (WARNING: Adjusting this is a net compatibility break) */
    static constexpr uint32 ClientIDSizeBits = 3;
};
```