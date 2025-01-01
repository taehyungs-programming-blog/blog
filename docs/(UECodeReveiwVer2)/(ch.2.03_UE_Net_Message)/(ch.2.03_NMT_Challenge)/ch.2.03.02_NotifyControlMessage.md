---
layout: default
title: "[MSG] NMT_Challenge (PendingNetGame::NotifyControlMessage)"
parent: "([Network] 03. UE Net의 주요 메시지 정리)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// PendingNetGame::NotifyControlMessage

virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch) override
{
    check(Connection == NetDriver->ServerConnection);

    switch (MessageType)
    {
        case NMT_Challenge:
        {
            // challenged by server
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
```
