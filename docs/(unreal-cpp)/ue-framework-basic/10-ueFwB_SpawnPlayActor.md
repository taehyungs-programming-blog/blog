---
layout: default
title: "10. SpawnPlayActor"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
FGameInstancePIEResult UGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	// ...

		// @todo, just use WorldContext.GamePlayer[0]?
		if (LocalPlayer)
		{
			FString Error;
            // StartPlayInEditorGameInstance에서 SpawnPlayActor가 호출됨
			if (!LocalPlayer->SpawnPlayActor(URL.ToString(1), Error, PlayWorld))
			{
				return FGameInstancePIEResult::Failure(FText::Format(NSLOCTEXT("UnrealEd", "Error_CouldntSpawnPlayer", "Couldn't spawn player: {0}"), FText::FromString(Error)));
			}

			if (GameViewport != NULL && GameViewport->Viewport != NULL)
			{
				SlowTask.EnterProgressFrame(25, NSLOCTEXT("UnrealEd", "PIEWaitingForLevelStreaming", "Starting PIE (Waiting for level streaming)..."));
				// Stream any levels now that need to be loaded before the game starts as a result of spawning the local player
				GEngine->BlockTillLevelStreamingCompleted(PlayWorld);
			}
		}

    // ...
```

```cpp
bool ULocalPlayer::SpawnPlayActor(const FString& URL,FString& OutError, UWorld* InWorld)
{
	check(InWorld);
	if (!InWorld->IsNetMode(NM_Client))
	{
		FURL PlayerURL(NULL, *URL, TRAVEL_Absolute);

		// Get player nickname
		FString PlayerName = GetNickname();
		if (PlayerName.Len() > 0)
		{
			PlayerURL.AddOption(*FString::Printf(TEXT("Name=%s"), *PlayerName));
		}

		// Send any game-specific url options for this player
		FString GameUrlOptions = GetGameLoginOptions();
		if (GameUrlOptions.Len() > 0)
		{
			PlayerURL.AddOption(*FString::Printf(TEXT("%s"), *GameUrlOptions));
		}

		// Get player unique id
		FUniqueNetIdRepl UniqueId(GetPreferredUniqueNetId());

        // 오... 역시 Actor는 World에 속하기에 World에서 Spawn하는구나?
		PlayerController = InWorld->SpawnPlayActor(this, ROLE_SimulatedProxy, PlayerURL, UniqueId, OutError, GEngine->GetGamePlayers(InWorld).Find(this));
	}

    // ...
```

```cpp
APlayerController* UWorld::SpawnPlayActor(UPlayer* NewPlayer, ENetRole RemoteRole, const FURL& InURL, const FUniqueNetIdRepl& UniqueId, FString& Error, uint8 InNetPlayerIndex)
{
	Error = TEXT("");

	// Make the option string.
	FString Options;
	for (int32 i = 0; i < InURL.Op.Num(); i++)
	{
		Options += TEXT('?');
		Options += InURL.Op[i];
	}

	if (AGameModeBase* const GameMode = GetAuthGameMode())
	{
		// Give the GameMode a chance to accept the login
        // LogIn이란 처리가 Spawn처리와 동일한 말임을 기억하자.
		APlayerController* const NewPlayerController = GameMode->Login(NewPlayer, RemoteRole, *InURL.Portal, Options, UniqueId, Error);
		if (NewPlayerController == NULL)
		{
			UE_LOG(LogSpawn, Warning, TEXT("Login failed: %s"), *Error);
			return NULL;
		}

		UE_LOG(LogSpawn, Log, TEXT("%s got player %s [%s]"), *NewPlayerController->GetName(), *NewPlayer->GetName(), UniqueId.IsValid() ? *UniqueId->ToString() : TEXT("Invalid"));

		// Possess the newly-spawned player.
		NewPlayerController->NetPlayerIndex = InNetPlayerIndex;
		NewPlayerController->SetRole(ROLE_Authority);
		NewPlayerController->SetReplicates(RemoteRole != ROLE_None);
		if (RemoteRole == ROLE_AutonomousProxy)
		{
			NewPlayerController->SetAutonomousProxy(true);
		}
		NewPlayerController->SetPlayer(NewPlayer);
		GameMode->PostLogin(NewPlayerController);
		return NewPlayerController;
	}

	UE_LOG(LogSpawn, Warning, TEXT("Login failed: No game mode set."));
	return nullptr;
}
```

