---
layout: default
title: "02-02. FSocket"
parent: "([Network] 02. UE Net의 주요 Class)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* FSocket은 Unreal Engine의 네트워크 소켓 추상화 클래스입니다. 플랫폼 독립적인 소켓 인터페이스를 제공하며, 주로 TCP와 UDP 통신에 사용

```cpp
// 게임 서버와 통신하는 매니저 클래스 예시
class FGameNetworkManager
{
public:
    FGameNetworkManager()
        : Socket(nullptr)
        , SocketSubsystem(nullptr)
        , ReceiverThread(nullptr)
        , bIsRunning(false)
    {
    }

    // 서버 연결 초기화
    bool Initialize(const FString& ServerIP, int32 ServerPort)
    {
        SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
        if (!SocketSubsystem)
            return false;

        // TCP 소켓 생성
        Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("GameServer Connection"), true);
        if (!Socket)
            return false;

        // 서버 주소 설정
        TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
        bool bIsValid = false;
        Addr->SetIp(*ServerIP, bIsValid);
        if (!bIsValid)
            return false;
        Addr->SetPort(ServerPort);

        // 서버 연결
        if (!Socket->Connect(*Addr))
        {
            SocketSubsystem->DestroySocket(Socket);
            Socket = nullptr;
            return false;
        }

        // 수신 스레드 시작
        bIsRunning = true;
        ReceiverThread = FRunnableThread::Create(this, TEXT("GameNetwork Receiver"));
        
        return true;
    }

    // 게임 데이터 전송 (예: 플레이어 위치)
    bool SendPlayerUpdate(const FVector& Position, const FRotator& Rotation)
    {
        if (!Socket || !Socket->IsValid())
            return false;

        // 데이터 직렬화
        FPlayerUpdatePacket Packet;
        Packet.Position = Position;
        Packet.Rotation = Rotation;
        Packet.Timestamp = FDateTime::UtcNow().ToUnixTimestamp();

        int32 BytesSent = 0;
        return Socket->Send((uint8*)&Packet, sizeof(Packet), BytesSent);
    }

    // 게임 이벤트 수신 (스레드에서 실행)
    virtual uint32 Run() override
    {
        while (bIsRunning)
        {
            uint8 Buffer[1024];
            int32 BytesRead = 0;

            if (Socket->Recv(Buffer, sizeof(Buffer), BytesRead))
            {
                // 게임 이벤트 처리
                ProcessGameEvent(Buffer, BytesRead);
            }

            // 적절한 간격으로 대기
            FPlatformProcess::Sleep(0.016f); // ~60Hz
        }
        return 0;
    }

    void Shutdown()
    {
        bIsRunning = false;
        
        if (ReceiverThread)
        {
            ReceiverThread->Kill(true);
            delete ReceiverThread;
            ReceiverThread = nullptr;
        }

        if (Socket)
        {
            Socket->Close();
            SocketSubsystem->DestroySocket(Socket);
            Socket = nullptr;
        }
    }

private:
    FSocket* Socket;
    ISocketSubsystem* SocketSubsystem;
    FRunnableThread* ReceiverThread;
    FThreadSafeBool bIsRunning;

    // 게임 이벤트 처리
    void ProcessGameEvent(uint8* Data, int32 Size)
    {
        // 이벤트 타입에 따른 처리
        EGameEventType EventType = *(EGameEventType*)Data;
        switch (EventType)
        {
            case EGameEventType::PlayerJoined:
                HandlePlayerJoined(Data, Size);
                break;
            case EGameEventType::PlayerLeft:
                HandlePlayerLeft(Data, Size);
                break;
            case EGameEventType::GameStateUpdate:
                HandleGameStateUpdate(Data, Size);
                break;
        }
    }
};

// 게임 월드에서 사용 예시
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    NetworkManager = MakeShared<FGameNetworkManager>();
    if (NetworkManager->Initialize(TEXT("127.0.0.1"), 7777))
    {
        // 주기적으로 플레이어 상태 업데이트 전송
        GetWorld()->GetTimerManager().SetTimer(
            UpdateTimerHandle,
            [this]()
            {
                if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
                {
                    if (APawn* Pawn = PC->GetPawn())
                    {
                        NetworkManager->SendPlayerUpdate(
                            Pawn->GetActorLocation(),
                            Pawn->GetActorRotation()
                        );
                    }
                }
            },
            0.1f, // 100ms 간격
            true
        );
    }
}
```