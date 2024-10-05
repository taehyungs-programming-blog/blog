---
layout: default
title: "03-05. InitConnect"
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
/** kick off NetDriver level address resolution for clients connecting to servers (with resolution passing on to FNetConnectionAddressResolution) */
void InitConnect(UNetConnection* ServerConnection, ISocketSubsystem* SocketSubsystem, const FSocket* ActiveSocket, const FURL& ConnectURL)
{
    //  we pass ServerConnection as 'UIpConnection'
    // - our NetConnectionAddressResolution is 'enabled' for AddressResolution
    UIpConnection* IPConnection = CastChecked<UIpConnection>(ServerConnection);
    int32 DestinationPort = ConnectURL.Port;
    if (IPConnection->Resolver->IsAddressResolutionEnabled())
    {
        //  IpNetDriver already created sockets, in our case, BoundSockets is same as UIpNetDriver::SocketPrivate
        // - we pass NetDriver's sockets to "NetConnection's AddressResolution.BindSockets"
        IPConnection->Resolver->BindSockets = MoveTemp(BoundSockets);

        // create a weakobj so that we can pass the Connection safely to the lambda for later
        //  'AsyncResolverHandler' lambda function is executed in arbitrary time, we pass IPConnection with TWeakObjectPtr safely
        // - what if UIpConnection is gc'ed, cuz networking error occurs: in this case, we need to handle it nicely
        // - AsyncResolverHandler is callback function, first see ISocketSubsystem::GetAddressInfoAsync(goto 029: ClientConnect)
        TWeakObjectPtr<UIpConnection> SafeConnectionPtr(IPConnection);
        auto AsyncResolverHandler = [SafeConnectionPtr, SocketSubsystem, DestinationPort](FAddressInfoResult Results)
        {
            // 032 - Networking - ClientConnect - FNetDirverAddressResolution::InitConnect::AsyncResolverHandler
            UIpConnection* Connection = SafeConnectionPtr.Get();
            if (Results.ReturnCode == SE_NO_ERROR)
            {
                TArray<TSharedRef<FInternetAddr>> AddressResults;
                for (auto& Result : Results.Results)
                {
                    AddressResults.Add(Result.Address);
                }

                //  forward AddressResults, normally only one address is returned
                // - we also update ResolutionState as "TryNextAddress"!
                // - REMEMBER we are in 'TryNextAddress'
                Connection->Resolver->ResolverResults = MoveTemp(AddressResults);
                Connection->Resolver->ResolutionState = EAddressResolutionState::TryNextAddress;
            }
            else
            {
                // ERROR!
            }
        };

        SocketSubsystem->GetAddressInfoAsync(AsyncResolverHandler, *ConnectURL.Host, *FString::Printf(TEXT("%d"), DestinationPort),
            EAddressInfoFlags::AllResultsWithMapping | EAddressInfoFlags::OnlyUsableAddresses, NAME_None, ESocketType::SOCKTYPE_Datagram);
    
        //  for simplicity, we assume ISocketSubsystem::GetAddressInfoAsync is finished
    }
}
```