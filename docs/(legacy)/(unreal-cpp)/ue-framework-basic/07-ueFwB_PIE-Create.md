---
layout: default
title: "07. PIE(PlayInEditor) Create 과정"
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

* Editor에서 Play버튼을 누르면 시작하게 되는 동작의 정리이다.

```cpp
int32 GuardedMain( const TCHAR* CmdLine )
{
	FTrackedActivity::GetEngineActivity().Update(TEXT("Starting"), FTrackedActivity::ELight::Yellow);

	FTaskTagScope Scope(ETaskTag::EGameThread);

    // ...

#if WITH_EDITOR
		if (GIsEditor)
		{
			ErrorLevel = EditorInit(GEngineLoop);
		}
		else
#endif
		{
			ErrorLevel = EngineInit();
		}
	}

    // ...

	if (!GUELibraryOverrideSettings.bIsEmbedded)
	{
        while( !IsEngineExitRequested() )
		{
            // 여기부터겠지?
			EngineTick();
		}
	}

    // ...
```

```cpp
/** 
 * Ticks the engine loop 
 */
LAUNCH_API void EngineTick( void )
{
	GEngineLoop.Tick();
}
```

```cpp
void FEngineLoop::Tick()
{
    // 각 World에 Sync를 맞주고 Tick을 전달
}
```

```cpp
void UEditorEngine::Tick( float DeltaSeconds, bool bIdleMode )
{
    // 여기서 Tick을 각 Component 및 World에 전달
        // 자세한건 이후에 정리 예정.
    // ..

    // Kick off a Play Session request if one was queued up during the last frame.
	if (PlaySessionRequest.IsSet())
	{
        // 이렇게 Play버튼이 눌러졌는지 체크한다.
		StartQueuedPlaySessionRequest();
	}

    // ...
}
```

```cpp
void UEditorEngine::StartQueuedPlaySessionRequest()
{
	if (!PlaySessionRequest.IsSet())
	{
		UE_LOG(LogPlayLevel, Warning, TEXT("StartQueuedPlaySessionRequest() called whith no request queued. Ignoring..."));
		return;
	}

	StartQueuedPlaySessionRequestImpl();

	// Ensure the request is always reset after an attempt (which may fail)
	// so that we don't get stuck in an infinite loop of start attempts.
	PlaySessionRequest.Reset();
}
```

```cpp
void UEditorEngine::StartQueuedPlaySessionRequestImpl()
{
    // 기존 PIE로직을 정리
	if (!ensureAlwaysMsgf(PlaySessionRequest.IsSet(), TEXT("StartQueuedPlaySessionRequest should not be called without a request set!")))
	{
		return;
	}

	// End any previous sessions running in separate processes.
	EndPlayOnLocalPc();

	// If there's level already being played, close it. (This may change GWorld). 
	if (PlayWorld && PlaySessionRequest->SessionDestination == EPlaySessionDestinationType::InProcess)
	{
		// Cache our Play Session Request, as EndPlayMap will clear it. When this function exits the request will be reset anyways.
		FRequestPlaySessionParams OriginalRequest = PlaySessionRequest.GetValue();
		// Immediately end the current play world.
		EndPlayMap(); 
		// Restore the request as we're now processing it.
		PlaySessionRequest = OriginalRequest;
	}

	// We want to use the ULevelEditorPlaySettings that come from the Play Session Request.
	// By the time this function gets called, these settings are a copy of either the CDO, 
	// or a user provided instance. The settings may have been modified by the game instance
	// after the request was made, to allow game instances to pre-validate settings.
	const ULevelEditorPlaySettings* EditorPlaySettings = PlaySessionRequest->EditorPlaySettings;
	check(EditorPlaySettings);

	PlayInEditorSessionInfo = FPlayInEditorSessionInfo();
	PlayInEditorSessionInfo->PlayRequestStartTime = FPlatformTime::Seconds();
	PlayInEditorSessionInfo->PlayRequestStartTime_StudioAnalytics = FPlatformTime::Seconds();;

	// Keep a copy of their original request settings for any late
	// joiners or async processes that need access to the settings after launch.
	PlayInEditorSessionInfo->OriginalRequestParams = PlaySessionRequest.GetValue();

	// Load the saved window positions from the EditorPlaySettings object.
	for (const FIntPoint& Position : EditorPlaySettings->MultipleInstancePositions)
	{
		FPlayInEditorSessionInfo::FWindowSizeAndPos& NewPos = PlayInEditorSessionInfo->CachedWindowInfo.Add_GetRef(FPlayInEditorSessionInfo::FWindowSizeAndPos());
		NewPos.Position = Position;
	}

	// If our settings require us to launch a separate process in any form, we require the user to save
	// their content so that when the new process reads the data from disk it will match what we have in-editor.
	bool bUserWantsInProcess;
	EditorPlaySettings->GetRunUnderOneProcess(bUserWantsInProcess);

	bool bIsSeparateProcess = PlaySessionRequest->SessionDestination != EPlaySessionDestinationType::InProcess;
	if (!bUserWantsInProcess)
	{
		int32 NumClients;
		EditorPlaySettings->GetPlayNumberOfClients(NumClients);

		EPlayNetMode NetMode;
		EditorPlaySettings->GetPlayNetMode(NetMode);

		// More than one client will spawn a second process.		
		bIsSeparateProcess |= NumClients > 1;

		// If they want to run anyone as a client, a dedicated server is started in a separate process.
		bIsSeparateProcess |= NetMode == EPlayNetMode::PIE_Client;
	}

	if (bIsSeparateProcess && !SaveMapsForPlaySession())
	{
		// Maps did not save, print a warning
		FText ErrorMsg = LOCTEXT("PIEWorldSaveFail", "PIE failed because map save was canceled");
		UE_LOG(LogPlayLevel, Warning, TEXT("%s"), *ErrorMsg.ToString());
		FMessageLog(NAME_CategoryPIE).Warning(ErrorMsg);
		FMessageLog(NAME_CategoryPIE).Open();
		
		CancelRequestPlaySession();
		return;
	}

	FEditorDelegates::StartPIE.Broadcast(PlayInEditorSessionInfo->OriginalRequestParams.WorldType == EPlaySessionWorldType::SimulateInEditor);

    // 어떤 모드의 PIE로 실행되었는지 확인 후 실행
	// We'll branch primarily based on the Session Destination, because it affects which settings we apply and how.
	switch (PlaySessionRequest->SessionDestination)
	{
	case EPlaySessionDestinationType::InProcess:
		// Create one-or-more PIE/SIE sessions inside of the current process.
        // 여기만 봐도 좋다       // 여기만 봐도 좋다
		StartPlayInEditorSession(PlaySessionRequest.GetValue());
		break;
	case EPlaySessionDestinationType::NewProcess:
		// Create one-or-more PIE session by launching a new process on the local machine.
		StartPlayInNewProcessSession(PlaySessionRequest.GetValue());
		break;
	case EPlaySessionDestinationType::Launcher:
		// Create a Play Session via the Launcher which may be on a local or remote device.
		StartPlayUsingLauncherSession(PlaySessionRequest.GetValue());
		break;
	default:
		check(false);
	}
}
```

* 여기까지하면 PIE Create까진 완료!