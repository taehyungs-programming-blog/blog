---
layout: default
title: "10-04. NotifyControlMessage"
parent: "([Network] 10. ClientLoginControlMessage)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## FNetControlMessage의 설명이 먼저다

* `FNetControlMessage`는 언리얼 엔진의 네트워크 제어 메시지를 처리하기 위한 템플릿 클래스입니다. 주로 서버-클라이언트 간의 핸드셰이크, 로그인, 조인 등의 제어 메시지를 주고받는 데 사용됩니다.

```cpp
// 메시지 전송
FNetControlMessage<NMT_Hello>::Send(Connection, IsLittleEndian, NetworkVersion, Token, Features);

// 메시지 수신
FString Token;
if (FNetControlMessage<NMT_Hello>::Receive(Bunch, IsLittleEndian, NetworkVersion, Token, Features))
{
    // 메시지 처리
}
```

```cpp
// Hello 메시지 (클라이언트 -> 서버)
template<>
struct FNetControlMessage<NMT_Hello>
{
    static bool Receive(FInBunch& Bunch, uint8& IsLittleEndian, uint32& RemoteNetworkVersion,
        FString& EncryptionToken, EEngineNetworkRuntimeFeatures& RemoteNetworkFeatures)
    {
        bool bSuccess = Bunch.SerializeInt(IsLittleEndian, 1);
        bSuccess &= Bunch.SerializeInt(RemoteNetworkVersion);
        bSuccess &= Bunch.ReadString(EncryptionToken);
        
        uint32 NetworkFeatures = 0;
        bSuccess &= Bunch.SerializeInt(NetworkFeatures);
        RemoteNetworkFeatures = static_cast<EEngineNetworkRuntimeFeatures>(NetworkFeatures);
        
        return bSuccess;
    }

    static void Send(UNetConnection* Connection, uint8 IsLittleEndian, uint32 NetworkVersion,
        const FString& EncryptionToken, EEngineNetworkRuntimeFeatures NetworkFeatures)
    {
        FNetBitWriter Writer(Connection->PackageMap);
        
        Writer.SerializeInt(IsLittleEndian, 1);
        Writer.SerializeInt(NetworkVersion);
        Writer.WriteString(EncryptionToken);
        Writer.SerializeInt(static_cast<uint32>(NetworkFeatures));
        
        Connection->SendRawBunch(Writer, true);
    }
};

// Welcome 메시지 (서버 -> 클라이언트)
template<>
struct FNetControlMessage<NMT_Welcome>
{
    static bool Receive(FInBunch& Bunch, FString& Map, FString& GameName, FString& RedirectURL)
    {
        bool bSuccess = Bunch.ReadString(Map);
        bSuccess &= Bunch.ReadString(GameName);
        bSuccess &= Bunch.ReadString(RedirectURL);
        return bSuccess;
    }

    static void Send(UNetConnection* Connection, const FString& Map, 
        const FString& GameName, const FString& RedirectURL)
    {
        FNetBitWriter Writer(Connection->PackageMap);
        
        Writer.WriteString(Map);
        Writer.WriteString(GameName);
        Writer.WriteString(RedirectURL);
        
        Connection->SendRawBunch(Writer, true);
    }
};
```

---

```cpp
// PendingNetGame.h

virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch) override
{
    check(Connection == NetDriver->ServerConnection);

    switch (MessageType)
    {
        case NMT_Challenge:
        {
            if (FNetControlMessage<NMT_Challenge>::Receive(Bunch, Connection->Challenge))
            {
                FURL PartialURL(URL);
                PartialURL.Host = TEXT("");
                PartialURL.Port = PartialURL.UrlConfig.DefaultPort; 
                ParitalURL.Map = TEXT("");

                ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this);
                if (LocalPlayer)
                {
                    FString OverrideName = LocalPlayer->GetNickname();
                    if (OverrideName.Len() > 0)
                    {
                        PartialURL.AddOption(*FString::Printf(TEXT("Name=%s"), *OverrideName));
                    }

                    Connection->PlayerId = LocalPlayer->GetPreferredUniqueNetId();
                }

                Connection->ClientResponse = TEXT("0");

                FString URLString(ParitialURL.ToString());
                FString OnlinePlatformNameString = TEXT("NULL");

                FNetControlMessage<NMT_Login>::Send(Connection, Connection->ClientResponse, URLString, Connection->PlayerId, OnlinePlatformNameString);

                NetDriver->ServerConnection->FlushNet();
            }
            break;
        }

        
        case NMT_Welcome:
        {
            FString GameName;
            FString RedirectURL;

            if (FNetControlMessage<NMT_Welcome>::Receive(Bunch, URL.Map, GameName, RedirectURL))
            {
                if (GameName.Len() > 0)
                {
                    URL.AddOption(*FString::Printf(TEXT("game=%s"), *GameName));
                }

                FNetControlMessage<NMT_Netspeed>::Send(Connection, Connection->CurrentNetSpeed);

                bSuccessfullyConnected = true;
            }

            break;
        }
    }
}
```