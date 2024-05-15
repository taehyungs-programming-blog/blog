---
layout: default
title: "06. Camera"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/2)

---

## 변수를 통해 대략적 살펴보기

* Character에 붙어있는 LyraCameraComponent를 확인해 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-6-1.png"/>
</p>

```cpp
UCLASS()
class ULyraCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

    // ...

protected:

	// 여러 카메라 모드를 관리하기 위해 만들었구나?
        // 카메라 모드 - 1, 2, 3 인칭 등 ..
	UPROPERTY()
	TObjectPtr<ULyraCameraModeStack> CameraModeStack;

	// ...
};
```

```cpp
UCLASS()
class ULyraCameraModeStack : public UObject
{
	GENERATED_BODY()

    // ...

    // ULyraCameraModeStack은 말 그대로 CameraMode를 Stack으로 관리
	UPROPERTY()
	TArray<TObjectPtr<ULyraCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<ULyraCameraMode>> CameraModeStack;
};

```

---

## 핵심 함수는?

```cpp
void ULyraCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);

	UpdateCameraModes();

	FLyraCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	// Keep player controller in sync with the latest view.
	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// Apply any offset that was added to the field of view.
	CameraModeView.FieldOfView += FieldOfViewOffset;
	FieldOfViewOffset = 0.0f;

	// Keep camera component in sync with the latest view.
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
	FieldOfView = CameraModeView.FieldOfView;

	// Fill in desired view.
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;

	// See if the CameraActor wants to override the PostProcess settings used.
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}


	if (IsXRHeadTrackedCamera())
	{
		// In XR much of the camera behavior above is irrellevant, but the post process settings are not.
		Super::GetCameraView(DeltaTime, DesiredView);
	}
}
```