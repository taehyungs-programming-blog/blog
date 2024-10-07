---
layout: default
title: "02-07. InitListen"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* NetDriver까지 할당 및 Set완료! 이제 진짜 Listen준비

```cpp                                                                                  
virtual bool InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error) override
{
    // 
    // - NetDriver is world's network driver
    // - in the low-level perspective, how to communicate in network?
    // - NetDriver needs socket!!
    //   - in InitBase(), the socket is created for the NetDriver :)

    // - NetDriver는 월드의 네트워크 드라이버입니다
    // - 저수준 관점에서, 네트워크에서 어떻게 통신할까요?
    // - NetDriver는 소켓이 필요합니다!!
    //   - InitBase()에서 NetDriver를 위한 소켓이 생성됩니다 :)
    if (!InitBase(false, InNotify, LocalURL, bReuseAddressAndPort, Error))
    {
        return false;
    }

    // InitConnectionlessHandler는 다음장 참조
    InitConnectionlessHandler();

    // update port!
    // 포트를 업데이트합니다!
    LocalURL.Port = LocalAddr->GetPort();

    return true;
}
```

```cpp
virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override
{
    // UNetDriver::InitBase 참조 (1)
    if (!UNetDriver::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
    {
        return false;
    }

    ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
    if (SocketSubsystem == nullptr)
    {
        return false;
    }

    // BindPort is assigned URL.Port (ServerPort) for server, GetClientPort() is called for client side
    // BindPort는 서버의 경우 URL.Port(ServerPort)가 할당되고, 클라이언트 측에서는 GetClientPort()가 호출됩니다
    const int32 BindPort = bInitAsClient ? GetClientPort() : URL.Port;

    // increase socket queue size, because we are polling rather than threading and thus we rely on the OS socket to buffer a lot of data
    // by default, Desired[Recv|Send]Size are determined like this:
    // - ClientDesiredSocketReceiveBufferBytes(ClientDesiredSocketSendBufferBytes): 32768(32KB)
    // - ServerDesiredSocketReceiveBufferBytes(ServerDesiredSocketSendBufferBytes): 131072(128KB)
    // - Send/Recv is same!

    // 소켓 큐 크기를 증가시킵니다. 스레딩 대신 폴링을 사용하므로 OS 소켓이 많은 데이터를 버퍼링하는 데 의존합니다
    // 기본적으로 Desired[Recv|Send]Size는 다음과 같이 결정됩니다:
    // - ClientDesiredSocketReceiveBufferBytes(ClientDesiredSocketSendBufferBytes): 32768(32KB)
    // - ServerDesiredSocketReceiveBufferBytes(ServerDesiredSocketSendBufferBytes): 131072(128KB)
    // - 송신/수신 크기는 동일합니다!

    const int32 DesiredRecvSize = bInitAsClient ? ClientDesiredSocketReceiveBufferBytes : ServerDesiredSocketReceiveBufferBytes;
    const int32 DesiredSendSize = bInitAsClient ? ClientDesiredSocketSendBufferBytes : ServerDesiredSocketSendBufferBytes;

    // first see CreateAndBindSocketsFunc lambda definition before getting into Resolver->InitBindSockets:
    // - actually, this lambda is called inside InitBindSockets()
    // - see what variables are captured in the lambda:
    //   1. BindPort
    //   2. bResueAddressAndPort == false
    //   3. DesiredRecvSize/DesiredSendSize

    // Resolver->InitBindSockets로 들어가기 전에 먼저 CreateAndBindSocketsFunc 람다 정의를 보세요:
    // - 실제로 이 람다는 InitBindSockets() 내부에서 호출됩니다
    // - 람다에서 캡처되는 변수들을 보세요:
    //   1. BindPort
    //   2. bReuseAddressAndPort == false
    //   3. DesiredRecvSize/DesiredSendSize
    const EInitBindSocketsFlags InitBindFlags = bInitAsClient ? EInitBindSocketsFlags::Client : EInitBindSocketsFlags::Server;
    FCreateAndBindSocketFunc CreateAndBindSocketsFunc = [this, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize]
        (TSharedRef<FInternetAddr> BindAddr, FString& Error) -> FUniqueSocket
        {
            return this->CreateAndBindSocket(BindAddr, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize, Error);
        };
    
    bool bInitBindSocketsSuccess = Resolver->InitBindSockets(MoveTemp(CreateAndBindSocketsFunc), InitBindFlags, SocketSubsystem, Error);
    if (!bInitBindSocketsSuccess)
    {
        return false;
    }

    SetSocketAndLocalAddress(Resolver->GetFirstSocket());

    return true;
}
```

---

## (1) UNetDriver::InitBase

```cpp
virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
    // read any timeout overrides from the URL
    // URL에서 타임아웃 오버라이드를 읽습니다
    // InitialConnectTimeout and ConnectionTimeout are useful properties to see a glimpse of the reliable UDP
    // InitialConnectTimeout과 ConnectionTimeout은 신뢰할 수 있는 UDP의 개요를 볼 수 있는 유용한 속성입니다
    // - see InitialConnectTimeout and ConnectionTimeout variables briefly
    // - InitialConnectTimeout과 ConnectionTimeout 변수를 간단히 살펴보세요
    // - try to understand some portions of reliable UDP with these two variables:
    // - 이 두 변수를 통해 신뢰할 수 있는 UDP의 일부를 이해해 보세요:
    //
    //   *** Reliable UDP:                                                                            
    //   *** 신뢰할 수 있는 UDP:                                                                            
    //                                                                                                
    //       1. the UDP doesn't have connection: RELIABLE UDP have "connection"                       
    //       1. UDP는 연결이 없습니다: 신뢰할 수 있는 UDP는 "연결"이 있습니다                       
    //           - the reliable UDP make a connection in the software manner (not hardware)           
    //           - 신뢰할 수 있는 UDP는 소프트웨어 방식으로 연결을 만듭니다 (하드웨어가 아님)           
    //             ┌────────┐                               ┌────────┐                                
    //             │        ├───────────────────────────────┤        │                                
    //             │ Client ◄───────────Connection──────────► Server │                                
    //             │ 클라이언트 ◄───────────연결──────────► 서버 │                                
    //             │        ├───────────────────────────────┤        │                                
    //             └────────┘                               └────────┘                                
    //                                                                                                
    //                 *** when ConnectionTimeout is 1.0, it means for 1 sec:                         
    //                 *** ConnectionTimeout이 1.0일 때, 이는 1초를 의미합니다:                         
    //                     when there is no packets to communicate for 1 sec, destroy the connection  
    //                     1초 동안 통신할 패킷이 없으면 연결을 파괴합니다  
    //                                                                        ──────────────────────  
    //                                                                                                
    //       2. the reliable UDP is based on the UDP: the UDP could happen "packet-loss"              
    //       2. 신뢰할 수 있는 UDP는 UDP를 기반으로 합니다: UDP에서는 "패킷 손실"이 발생할 수 있습니다              
    //           - if it allow endless time to make a connection, it could result in "Conjestion"     
    //           - 연결을 만드는 데 무한한 시간을 허용하면 "혼잡"이 발생할 수 있습니다     
    //                                                                    ───────────────────────     
    //           - how the reliable UDP constructs a connection?: "Handshake" manner similar to TCP   
    //           - 신뢰할 수 있는 UDP는 어떻게 연결을 구성합니까?: TCP와 유사한 "핸드셰이크" 방식   
    //             e.g. 3-way hand-shake in TCP:                   ────────────────────────────────   
    //             예: TCP의 3-웨이 핸드셰이크:                   ────────────────────────────────   
    //                                                                                                
    //             ┌────────┐                                         ┌────────┐                      
    //             │ Client │                                         │ Server │                      
    //             │ 클라이언트 │                                         │ 서버 │                      
    //             └────┬───┘                                         └────┬───┘                      
    //                  │                                                  │                          
    //                  │                                       Prepare ┌──┤                          
    //                  │                                        LISTEN │  │                          
    //                  │                                       준비 ┌──┤                          
    //                  │                                        리스닝 │  │                          
    //                  │                                               └──►                          
    //                  │                                                  │                          
    //                  │             Send SYN (connect request)           │                          
    //                  │             SYN 전송 (연결 요청)           │                          
    //        SYN_SENT  ├─────────────────────────────────────────────────►│ LISTEN                   
    //        SYN_전송  ├─────────────────────────────────────────────────►│ 리스닝                   
    //                  │                                                  │                          
    //                  │             Send SYN+ACK                         │                          
    //                  │             SYN+ACK 전송                         │                          
    //     ESTABLISHED  │◄─────────────────────────────────────────────────┤ SYN_RECV'ED              
    //     연결 수립  │◄─────────────────────────────────────────────────┤ SYN_수신됨              
    //     (connection) │                                                  │                          
    //     (연결) │                                                  │                          
    //                  │             Send ACK                             │                          
    //                  │             ACK 전송                             │                          
    //                  ├─────────────────────────────────────────────────►│ ESTABLISHED              
    //                  ├─────────────────────────────────────────────────►│ 연결 수립              
    //                  │                                                  │ (connection)             
    //                  │                                                  │ (연결)             
    //                  │                                                  │                          
    //                  │       *** TCP's 3-way HandShaking                │                          
    //                  │       *** TCP의 3-웨이 핸드셰이킹                │                          
    //                  │                                                  │                          
    //                  │                                                  │                          
    //                  │                                                  │                          
    //                                                                                                

    if (const TCHAR* InitialConnectTimeoutOverride = URL.GetOption(TEXT("InitialConnectTimeout="), nullptr))
    {
        float ParsedValue;
        LexFromString(ParsedValue, InitialConnectTimeoutOverride);
        if (ParsedValue != 0.f)
        {
            InitialConnectTimeout = ParsedValue;
        }
    }
    if (const TCHAR* ConnectionTimeoutOverride = URL.GetOption(TEXT("ConnectionTimeout="), nullptr))
    {
        float ParsedValue;
        LexFromString(ParsedValue, ConnectionTimeoutOverride);
        if (ParsedValue != 0.0f)
        {
            ConnectionTimeout = ParsedValue;
        }
    }

    // the timeout is very annoying when you debugging, "-NoTimeouts" is very useful cmdarg to remember!
    // 디버깅 시 타임아웃은 매우 귀찮습니다. "-NoTimeouts"는 기억해둘 만한 유용한 명령줄 인수입니다!
    // - there are multiple ways to prevent the time-outs while you are debugging:
    // - 디버깅 중 타임아웃을 방지하는 여러 가지 방법이 있습니다:
    //   1. use '-NoTimeouts" option
    //   1. '-NoTimeouts" 옵션 사용
    //   2. set break points on both client-app and server-app
    //   2. 클라이언트 앱과 서버 앱 모두에 중단점 설정
    //
    // - in editor PIE build, when you use the option to run server in same process, you don't have to worry about this
    // - 에디터 PIE 빌드에서 서버를 동일한 프로세스에서 실행하는 옵션을 사용하면 이에 대해 걱정할 필요가 없습니다
    //   - when you are in debugging mode, the server also in debugging mode!
    //   - 디버깅 모드일 때 서버도 디버깅 모드입니다!
    if (URL.HasOption(TEXT("NoTimeouts")))
    {
        bNoTimeouts = true;
    }

    // see InitConnectionClass() ***
    // InitConnectionClass()를 참조하세요 ***
    bool bSuccess = InitConnectionClass();
    if (!bInitAsClient)
    {
        // "!bInitAsClient" is 'true' when we are in server-code
        // "!bInitAsClient"는 서버 코드에 있을 때 'true'입니다
        // - we'll cover this ConnectionlessHandler soon!
        // - 곧 이 ConnectionlessHandler를 다룰 것입니다!
        ConnectionlessHandler.Reset();
    }

    Notify = InNotify;

    InitNetTraceId();

    return bSuccess;
}
```