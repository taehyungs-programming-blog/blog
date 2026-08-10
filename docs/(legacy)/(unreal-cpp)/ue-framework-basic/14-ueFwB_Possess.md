---
layout: default
title: "14. Possess"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Possess 과정 정리

* 기존 Pawn 연결 해제
    * PlayerController는 현재 Pawn을 가지고 있을 수 있으므로 먼저 기존 연결을 해제
    * 보통 UnPossess() 호출
* 대상 Pawn 상태 확인
    * 새로 Possess할 Pawn에 이미 다른 Controller가 붙어 있는지 확인
    * 만약 있다면 기존 컨트롤러가 UnPossess()되거나 교체됩니다.
* Controller ↔ Pawn 연결
    * PlayerController::Possess(Pawn*)가 호출
        * Pawn->Controller에 this가 설정
    * Controller->Pawn도 해당 Pawn으로 업데이트
* Pawn 초기화
    * Pawn 쪽에서는 PossessedBy(AController*) 콜백이 호출되어 Controller를 참조
    * AI라면 RestartAI(), 플레이어라면 SetupPlayerInputComponent() 등의 초기화
* 카메라와 InputComponent 설정
    * PlayerCameraManager가 새로운 Pawn에 맞춰 업데이트
    * Pawn이 플레이어 조작 대상이면 InputComponent가 생성 및 연결

---

```cpp
void AController::Possess(APawn* InPawn)
{
	if (!bCanPossessWithoutAuthority && !HasAuthority())
	{
		FMessageLog("PIE").Warning(FText::Format(
			LOCTEXT("ControllerPossessAuthorityOnly", "Possess function should only be used by the network authority for {0}"),
			FText::FromName(GetFName())
			));
		UE_LOG(LogController, Warning, TEXT("Trying to possess %s without network authority! Request will be ignored."), *GetNameSafe(InPawn));
		return;
	}

	REDIRECT_OBJECT_TO_VLOG(InPawn, this);

	APawn* CurrentPawn = GetPawn();

	// A notification is required when the current assigned pawn is not possessed (i.e. pawn assigned before calling Possess)
	const bool bNotificationRequired = (CurrentPawn != nullptr) && (CurrentPawn->GetController() == nullptr);

	// Possess는 여기서 진행된다.
	OnPossess(InPawn);

	// Notify when pawn to possess (different than the assigned one) has been accepted by the native class or notification is explicitly required
	APawn* NewPawn = GetPawn();
	if ((NewPawn != CurrentPawn) || bNotificationRequired)
	{
		ReceivePossess(NewPawn);
		OnNewPawn.Broadcast(NewPawn);
		OnPossessedPawnChanged.Broadcast(bNotificationRequired ? nullptr : CurrentPawn, NewPawn);
	}
	
	TRACE_PAWN_POSSESS(this, InPawn); 
}
```

```cpp
void AController::OnPossess(APawn* InPawn)
{
	const bool bNewPawn = GetPawn() != InPawn;

	// Unpossess current pawn (if any) when current pawn changes
	if (bNewPawn && GetPawn() != nullptr)
	{
		UnPossess();
	}

	if (InPawn == nullptr)
	{
		return;
	}

	if (InPawn->Controller != nullptr)
	{
		UE_CLOG(InPawn->Controller == this, LogController, Warning, TEXT("Asking %s to possess pawn %s more than once; pawn will be restarted! Should call Unpossess first."), *GetNameSafe(this), *GetNameSafe(InPawn));
		InPawn->Controller->UnPossess();
	}

	InPawn->PossessedBy(this);
	SetPawn(InPawn);

	// update rotation to match possessed pawn's rotation
	SetControlRotation(Pawn->GetActorRotation());

    // 후 처리
	Pawn->DispatchRestart(false);
}
```

