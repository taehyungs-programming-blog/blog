---
layout: default
title: "09. 게임이 초기화 과정 실제 코드로"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우선 기존에 설명한 순서를 기억하자

* (1) UGameInstance
* (2) UWorld
* (3) UGameViewportClient
* (4) ULocalPlayer
* (5) AGameModeBase / AGameMode
    * UWorld::BeginPlay() 시점에 생성
* (6) AGameStateBase / AGameState
    * GameMode 생성 직후 InitGameState() 안에서 스폰됨. (GameMode → GameState 생성자 직접 호출)
* (7) APlayerController
    * GameMode의 PostLogin() / HandleStartingNewPlayer() 경로에서 SpawnPlayerController → GameMode::RestartPlayer() 로 이어짐. 이 과정에서 Pawn도 스폰되고 Possess
* (8) APlayerState
    * PlayerController와 짝으로 자동 생성. 기본적으로 Controller 생성 시점에 InitPlayerState() 통해 붙는다
* (9) APlayerCameraManager
    * PlayerController 안에서 내부적으로 생성. (APlayerController::InitInputSystem(), APlayerController::SpawnPlayerCameraManager() 참고)
* (10) UPlayerInput
* (11) APawn / ACharacter

```cpp
UGameInstance* UEditorEngine::CreateInnerProcessPIEGameInstance(FRequestPlaySessionParams& InParams, const FGameInstancePIEParameters& InPIEParameters, int32 InPIEInstanceIndex)
{
	// (1) GameInstance를 읽어온다
	FSoftClassPath GameInstanceClassName = GetDefault<UGameMapsSettings>()->GameInstanceClass;
	UClass* GameInstanceClass = GameInstanceClassName.TryLoadClass<UGameInstance>();

	// If an invalid class type was specified we fall back to the default.
	if (!GameInstanceClass)
	{
		GameInstanceClass = UGameInstance::StaticClass();
	}

	UGameInstance* GameInstance = NewObject<UGameInstance>(this, GameInstanceClass);

	// Root로 지정하여 GC를 피해야겠지
	GameInstance->AddToRoot();

	// (2) Init 과정에서 Subsystem/World복사 등이 일어나는데 그 과정이 이후에 다룬다
	const bool bFirstWorld = !PlayWorld;
	const FGameInstancePIEResult InitializeResult = GameInstance->InitializeForPlayInEditor(InPIEInstanceIndex, InPIEParameters);
	if (!InitializeResult.IsSuccess())
	{
		FMessageDialog::Open(EAppMsgType::Ok, InitializeResult.FailureReason);
		FEditorDelegates::EndPIE.Broadcast(InPIEParameters.bSimulateInEditor);
		FNavigationSystem::OnPIEEnd(*EditorWorld);

		GameInstance->RemoveFromRoot();
		return nullptr;
	}

	// Our game instance was successfully created
	FWorldContext* const PieWorldContext = GameInstance->GetWorldContext();
	check(PieWorldContext);
	PlayWorld = PieWorldContext->World();

	// Temporarily set GWorld to our newly created world. This utility function
	// also sets GIsPlayInEditorWorld so that users can know if GWorld is actually
	// a PIE world or not.
	SetPlayInEditorWorld(PlayWorld);

	// Initialize a local player and viewport client for non-dedicated server instances.
    // (3)
	UGameViewportClient* ViewportClient = nullptr;
	ULocalPlayer *NewLocalPlayer = nullptr;
	TSharedPtr<SPIEViewport> PIEViewport = nullptr;

	if (!InPIEParameters.bRunAsDedicated)
	{
		// Create an instance of the Game Viewport Client, with the class specified by the Engine.
		ViewportClient = NewObject<UGameViewportClient>(this, GameViewportClientClass);
		ViewportClient->Init(*PieWorldContext, GameInstance);

		ULevelEditorPlaySettings* PlayInSettings = GetMutableDefault<ULevelEditorPlaySettings>();
		ViewportClient->EngineShowFlags.SetServerDrawDebug(PlayInSettings->ShowServerDebugDrawingByDefault());

		if (!InParams.EditorPlaySettings->EnableGameSound)
		{
			if (FAudioDeviceHandle GameInstanceAudioDevice = GameInstance->GetWorld()->GetAudioDevice())
			{
				GameInstanceAudioDevice->SetTransientPrimaryVolume(0.0f);
			}
		}
		if (InParams.EditorPlaySettings->SoloAudioInFirstPIEClient)
		{
			if (FAudioDeviceHandle GameInstanceAudioDevice = PlayWorld->GetAudioDevice())
			{
				if (GEngine)
				{
					if (FAudioDeviceManager* DeviceManager = GEngine->GetAudioDeviceManager())
					{
						EPlayNetMode NetMode; 
						InParams.EditorPlaySettings->GetPlayNetMode(NetMode);
						if ((NetMode == PIE_Client && InPIEInstanceIndex == 1) || 
							((NetMode == PIE_Standalone || NetMode == PIE_ListenServer) && InPIEInstanceIndex == 0))
						{
							DeviceManager->SetSoloDevice(GameInstanceAudioDevice->DeviceID);
						}
						else
						{
							GameInstanceAudioDevice->SetDeviceMuted(true);
						}
					}
				}
			}
		}

		GameViewport = ViewportClient;
		GameViewport->bIsPlayInEditorViewport = true;

		// Update our World Context to know which Viewport Client is associated.
		PieWorldContext->GameViewport = ViewportClient;

		// Add a callback for Game Input that isn't absorbed by the Game Viewport. This allows us to
		// make editor commands work (such as Shift F1, etc.) from within PIE.
		ViewportClient->OnGameViewportInputKey().BindUObject(this, &UEditorEngine::ProcessDebuggerCommands);

		// Listen for when the viewport is closed, so we can see about shutting down PIE.
		ViewportCloseRequestedDelegateHandle = ViewportClient->OnCloseRequested().AddUObject(this, &UEditorEngine::OnViewportCloseRequested);
		FSlatePlayInEditorInfo& SlatePlayInEditorSession = SlatePlayInEditorMap.Add(PieWorldContext->ContextHandle, FSlatePlayInEditorInfo());

		// Might be invalid depending how pie was launched. Code below handles this
		if (InParams.DestinationSlateViewport.Get(nullptr).IsValid())
		{
			SlatePlayInEditorSession.DestinationSlateViewport = InParams.DestinationSlateViewport.GetValue();
			
			// Only one PIE Instance can live in a given viewport, so we'll null it out so that we create
			// windows instead for the remaining clients.
			InParams.DestinationSlateViewport = nullptr;
		}

		// (4) Attempt to initialize a Local Player.
		FString Error;
		NewLocalPlayer = ViewportClient->SetupInitialLocalPlayer(Error);
		if (!NewLocalPlayer)
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "Error_CouldntSpawnPlayer", "Couldn't spawn player: {0}"), FText::FromString(Error)));
			// go back to using the real world as GWorld
			RestoreEditorWorld(EditorWorld);
			EndPlayMap();
			GameInstance->RemoveFromRoot();
			return nullptr;
		}

		// A Local Player gets created even in SIE (which is different than a Player Controller), but we only
		// store a reference if we're PIE for the UI to know where to restore our viewport location after PIE closes.
		if (!InPIEParameters.bSimulateInEditor)
		{
			SlatePlayInEditorSession.EditorPlayer = NewLocalPlayer;
		}

		// Note: For K2 debugging purposes this MUST be created before beginplay is called because beginplay can trigger breakpoints
		// and we need to be able to refocus the pie viewport afterwards so it must be created first in order for us to find it
		{
			// If the original request provided a Slate Viewport, we'll use that for our output.
			if (SlatePlayInEditorSession.DestinationSlateViewport.IsValid())
			{
				TSharedPtr<IAssetViewport> LevelViewportRef = SlatePlayInEditorSession.DestinationSlateViewport.Pin();
				LevelViewportRef->StartPlayInEditorSession(ViewportClient, InParams.WorldType == EPlaySessionWorldType::SimulateInEditor);

				// We count this as a viewport being created so that subsequent clients won't think they're the 'first' and use the wrong setting.
				PlayInEditorSessionInfo->NumViewportInstancesCreated++;
			}
			else
			{
				// Generate a new Window to put this instance in.
				PIEViewport = GeneratePIEViewportWindow(InParams, PlayInEditorSessionInfo->NumViewportInstancesCreated, *PieWorldContext, InPIEParameters.NetMode, ViewportClient, SlatePlayInEditorSession);

				// Increment for each viewport so that the window titles get correct numbers and it uses the right save/load setting. Non-visible
				// servers won't be bumping this number as it's used for saving/restoring window positions.
				PlayInEditorSessionInfo->NumViewportInstancesCreated++;
			}


			// Broadcast that the Viewport has been successfully created.
			UGameViewportClient::OnViewportCreated().Broadcast();
		}

		// Mark the Viewport as a PIE Viewport
		if (GameViewport && GameViewport->Viewport)
		{
			GameViewport->Viewport->SetPlayInEditorViewport(true);
		}

		if (InParams.EditorPlaySettings->bUseNonRealtimeAudioDevice && AudioDeviceManager)
		{
			UE_LOG(LogPlayLevel, Log, TEXT("Creating new non-realtime audio mixer"));
			FAudioDeviceParams DeviceParams = AudioDeviceManager->GetDefaultParamsForNewWorld();
			DeviceParams.Scope = EAudioDeviceScope::Unique;
			DeviceParams.AssociatedWorld = PlayWorld;
			DeviceParams.bIsNonRealtime = true;
			// For NRT rendering, don't need a large buffer and don't need to double buffer
			DeviceParams.BufferSizeOverride = 32;
			DeviceParams.NumBuffersOverride = 2;
			FAudioDeviceHandle AudioDevice = AudioDeviceManager->RequestAudioDevice(DeviceParams);
			check(AudioDevice.IsValid());
			if (PlayWorld)
			{
				PlayWorld->SetAudioDevice(AudioDevice);
			}
		}
	}

	// By this point it is safe to remove the GameInstance from the root and allow it to garbage collected as per usual
	GameInstance->RemoveFromRoot();

	// If the request wanted to override the game mode we have to do that here while we still have specifics about
	// the request. This will allow
	if (InParams.GameModeOverride)
	{
		GameInstance->GetWorld()->GetWorldSettings()->DefaultGameMode = InParams.GameModeOverride;
	}

	// Transfer the Blueprint Debug references to the first client world that is created. This needs to be called before 
	// GameInstance->StartPlayInEditorGameInstance so that references are transfered by the time BeginPlay is called.
	if (bFirstWorld && PlayWorld)
	{
		EditorWorld->TransferBlueprintDebugReferences(PlayWorld);
	}

	FGameInstancePIEResult StartResult = FGameInstancePIEResult::Success();
	{
		FTemporaryPlayInEditorIDOverride OverrideIDHelper(InPIEInstanceIndex);
		StartResult = GameInstance->StartPlayInEditorGameInstance(NewLocalPlayer, InPIEParameters);
	}

	if (!StartResult.IsSuccess())
	{
		FMessageDialog::Open(EAppMsgType::Ok, StartResult.FailureReason);
		RestoreEditorWorld(EditorWorld);
		EndPlayMap();
		return nullptr;
	}

	EnableWorldSwitchCallbacks(true);

	if (PIEViewport.IsValid())
	{
		// Register the new viewport widget with Slate for viewport specific message routing.
		FSlateApplication::Get().RegisterGameViewport(PIEViewport.ToSharedRef());
	}

	// Go back to using the editor world as GWorld.
	RestoreEditorWorld(EditorWorld);

	return GameInstance;
}
```

