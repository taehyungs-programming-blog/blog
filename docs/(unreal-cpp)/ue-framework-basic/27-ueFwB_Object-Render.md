---
layout: default
title: "27. Render"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

# CameraUpdate

## 주요 개념

### APlayerController
- **역할**: 플레이어의 입력과 게임 로직을 연결하는 컨트롤러 클래스
- **특징**:
  - 플레이어의 Pawn(캐릭터)을 제어
  - 입력 처리 담당
  - 카메라 매니저(`PlayerCameraManager`)를 소유하고 관리
  - HUD, UI 등의 플레이어 관련 요소들을 총괄
- **주요 책임**:
  - 사용자 입력을 게임 액션으로 변환
  - 네트워크 통신 (클라이언트-서버)
  - 카메라 업데이트 트리거

### FTViewTarget
- **정의**: "View Target" - 카메라가 바라보는 대상을 나타내는 구조체
- **구성 요소**:
  ```cpp
  struct FTViewTarget
  {
      AActor* Target;          // 실제 타겟 액터 (일반적으로 Player의 Pawn)
      FMinimalViewInfo POV;    // 카메라의 시점 정보
      // ... 기타 블렌딩 관련 정보
  };
  ```
- **용도**: 
  - 현재 카메라가 어떤 액터를 따라가는지 추적
  - 카메라 전환 시 부드러운 블렌딩을 위한 정보 저장

### POV (Point of View)
- **정의**: 카메라의 시점 정보를 담는 구조체 (`FMinimalViewInfo`)
- **구성 요소**:
  ```cpp
  struct FMinimalViewInfo
  {
      FVector Location;      // 카메라 위치
      FRotator Rotation;     // 카메라 회전
      float FOV;             // Field of View (시야각)
      // ... AspectRatio, PostProcessSettings 등
  };
  ```
- **용도**: 렌더링 시스템에 전달되어 최종 화면을 그리는 데 사용

---

## 카메라 업데이트 흐름

```
Game Tick
    ↓
APlayerController::UpdateCameraManager
    ↓
APlayerCameraManager::UpdateCamera
    ↓
APlayerCameraManager::DoUpdateCamera
    ↓
APlayerCameraManager::UpdateViewTarget
    ↓
APlayerCameraManager::UpdateViewTargetInternal
    ↓
    ├─→ BlueprintUpdateCamera (BP에서 커스텀 로직)
    │
    └─→ AActor::CalcCamera
            ↓
        UCameraComponent::GetCameraView (최종 카메라 정보 계산)
    ↓
FillCameraCache (결과를 캐시에 저장)
```

---

## 상세 설명

### 1. APlayerController::UpdateCameraManager
```cpp
void APlayerController::UpdateCameraManager(float DeltaSeconds)
{
    if (PlayerCameraManager != NULL)
    {
        // 매 틱(tick)마다 카메라 업데이트를 수행
        // 다른 모든 액터들의 틱이 끝난 후, 매 틱마다 한 번 호출!
        PlayerCameraManager->UpdateCamera(DeltaSeconds);
    }
}
```

**호출 시점**: 
- 게임 틱 사이클의 후반부에 실행
- 모든 액터의 틱이 완료된 후 호출되어, 최신 게임 상태를 반영

**목적**: 
- `PlayerCameraManager`에게 카메라 업데이트를 위임

---

### 2. APlayerCameraManager::UpdateCamera vs DoUpdateCamera

#### UpdateCamera
```cpp
void APlayerCameraManager::UpdateCamera(float DeltaTime)
{
    // 실제 카메라 업데이트 로직 함수
    DoUpdateCamera(DeltaTime);
}
```

#### 왜 DoUpdateCamera를 따로 두었나?

**설계 이유**:
1. **네트워크 로직 추가 공간**: 
   - `UpdateCamera`는 네트워크 관련 로직(복제, 동기화)을 추가할 수 있는 진입점
   - 클라이언트/서버 환경에서 카메라 정보를 동기화해야 할 때 유용
   
2. **확장성**:
   - 상속받은 클래스에서 `UpdateCamera`를 오버라이드하여 전처리/후처리 로직 추가 가능
   - `DoUpdateCamera`는 핵심 로직만 담당하도록 분리

3. **Template Method 패턴**:
   - `UpdateCamera`: 템플릿 메서드 (프레임워크가 제공하는 확장 포인트)
   - `DoUpdateCamera`: 실제 구현 (핵심 알고리즘)

---

### 3. APlayerCameraManager::DoUpdateCamera
```cpp
void APlayerCameraManager::DoUpdateCamera(float DeltaTime)
{
    // 1. 지정된 ViewTarget(일반적으로 Player의 Actor의 카메라)를 통해 POV를 계산
    UpdateViewTarget(ViewTarget, DeltaTime);

    // 2. Update된 ViewTarget의 POV로 업데이트
    FMinimalViewInfo NewPOV = ViewTarget.POV;
    NewPOV.BlendViewInfo(PendingViewTarget.POV, BlendPct);

    // 3. 최종 카메라 시점 정보를 받아서 내부적으로 캐시 
    //    -> 이 정보를 다른 게임 시스템에서 빠르게 접근할 수 있도록
    FillCameraCache(NewPOV);
}
```

**주요 단계**:
1. **ViewTarget 업데이트**: 현재 타겟의 카메라 정보 계산
2. **블렌딩**: 카메라 전환 시 부드러운 전환을 위해 이전/다음 뷰를 보간
3. **캐싱**: 계산된 카메라 정보를 캐시에 저장하여 렌더링 및 다른 시스템에서 사용

---

### 4. APlayerCameraManager::UpdateViewTarget
```cpp
void APlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    if (ACameraActor* CamActor = Cast<ACameraActor>(OutVT.Target))
    {
        // CameraActor를 직접 사용하는 경우
        CamActor->GetCameraComponent()->GetCameraView(DeltaTime, OutVT.POV);
    }
    else
    {
        // 플레이어의 경우 일반적으로 해당 함수를 실행
        UpdateViewTargetInternal(OutVT, DeltaTime);
    }
}
```

**분기 처리**:
- **CameraActor**: 배치된 고정 카메라나 시네마틱 카메라 사용 시
- **일반 액터**: 플레이어 Pawn 등 일반적인 경우 → `UpdateViewTargetInternal` 호출

---

### 5. APlayerCameraManager::UpdateViewTargetInternal
```cpp
void APlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
    if (OutVT.Target)
    {
        FVector OutLocation;
        FRotator OutRotation;
        float OutFOV;

        // 블루프린트에서도 Camera Update 가능하도록 제공
        if (BlueprintUpdateCamera(OutVT.Target, OutLocation, OutRotation, OutFOV))
        {
            OutVT.POV.Location = OutLocation;
            OutVT.POV.Rotation = OutRotation;
            OutVT.POV.FOV = OutFOV;
        }
        else
        {
            // Actor를 통해 Camera 접근 계산
            OutVT.Target->CalcCamera(DeltaTime, OutVT.POV);
        }
    }
}
```

**우선순위**:
1. **Blueprint 커스텀 로직**: BP에서 오버라이드한 경우 해당 로직 사용
2. **Actor의 CalcCamera**: 기본 동작 - 액터가 소유한 카메라 컴포넌트 사용

---

### 6. AActor::CalcCamera
```cpp
void AActor::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
    TInlineComponentArray<UCameraComponent*> Cameras;
    GetComponents(/*out*/ Cameras);
    
    // Actor가 소유하고 있는 Camera 중 활성화되어 있는 첫 번째 카메라의 View 정보 가져옴
    for (UCameraComponent* CameraComponent : Cameras)
    {
        if (CameraComponent->IsActive())
        {
            CameraComponent->GetCameraView(DeltaTime, OutResult);
            return;
        }
    }
}
```

**동작**:
- 액터에 부착된 모든 `UCameraComponent`를 검색
- **활성화된 첫 번째 카메라**의 뷰 정보를 반환
- 여러 카메라가 있을 경우 우선순위는 컴포넌트 순서에 따름

---

## Q&A

### Q1: APlayerController는 무엇인가?

**답변**: 
`APlayerController`는 플레이어와 게임 세계를 연결하는 핵심 컨트롤러 클래스입니다.

**주요 역할**:
- **입력 처리**: 키보드, 마우스, 게임패드 등의 입력을 받아 게임 로직으로 변환
- **Pawn 제어**: 플레이어가 조종하는 캐릭터(Pawn)를 소유하고 제어
- **카메라 관리**: `APlayerCameraManager`를 소유하고 매 프레임 업데이트
- **네트워크**: 클라이언트-서버 아키텍처에서 플레이어 연결 관리
- **UI/HUD**: 플레이어 관련 UI 요소 관리

**비유**: 
실제 게임 플레이어(사람)와 게임 내 캐릭터 사이의 "중개자" 역할

---

### Q2: UpdateCamera에서 하지 않고 DoUpdateCamera를 둔 이유는?

**답변**: 
함수를 분리한 이유는 **확장성**과 **네트워크 로직 추가**를 위함입니다.

**주요 이유**:

1. **네트워크 관련 로직 추가 공간**:
   - 멀티플레이어 게임에서 카메라 정보를 서버와 동기화해야 할 때
   - `UpdateCamera`에서 네트워크 복제(Replication) 로직을 추가할 수 있음

2. **전/후처리 로직**:
   - `UpdateCamera`를 오버라이드하여 실제 업데이트 전후에 커스텀 로직 삽입 가능
   ```cpp
   void AMyPlayerCameraManager::UpdateCamera(float DeltaTime)
   {
       // 전처리
       PreCameraUpdate();
       
       // 핵심 로직
       DoUpdateCamera(DeltaTime);
       
       // 후처리
       PostCameraUpdate();
   }
   ```

3. **디자인 패턴** (Template Method Pattern):
   - `UpdateCamera`: 확장 가능한 템플릿 메서드
   - `DoUpdateCamera`: 변경되지 않는 핵심 알고리즘

---

### Q3: FTViewTarget는 무엇인가?

**답변**: 
`FTViewTarget`는 **카메라가 바라보는 대상**을 나타내는 구조체입니다.

**구성**:
```cpp
struct FTViewTarget
{
    AActor* Target;          // 타겟 액터 (예: 플레이어의 Pawn)
    FMinimalViewInfo POV;    // 해당 타겟의 카메라 시점 정보
    APawn* TargetPawn;       // 타겟이 Pawn인 경우
    APlayerState* PlayerState; // 관련 플레이어 상태
    // 블렌딩 관련 정보들...
};
```

**용도**:
- **현재 뷰 추적**: 카메라가 현재 어떤 액터를 따라가는지 저장
- **카메라 전환**: 다른 액터로 시점을 전환할 때 부드러운 블렌딩 지원
- **예시**:
  - 일반적으로: 플레이어 캐릭터 (Pawn)
  - 특수한 경우: 컷신용 CameraActor, 다른 플레이어 관전 등

---

### Q4: POV는 무엇인가?

**답변**: 
`POV`는 **Point of View**의 약자로, 카메라의 시점 정보를 담는 구조체입니다.

**정식 타입**: `FMinimalViewInfo`

**구성 요소**:
```cpp
struct FMinimalViewInfo
{
    FVector Location;           // 카메라 위치 (3D 공간)
    FRotator Rotation;          // 카메라 회전 (Pitch, Yaw, Roll)
    float FOV;                  // Field of View (시야각, 도 단위)
    float AspectRatio;          // 화면 비율
    float OrthoWidth;           // 직교 투영 너비
    float OrthoNearClipPlane;   // 근접 클리핑 평면
    float OrthoFarClipPlane;    // 원거리 클리핑 평면
    FPostProcessSettings PostProcessSettings;  // 후처리 효과
    // ... 기타
};
```

**용도**:
- **렌더링 입력값**: 최종적으로 게임 화면을 렌더링하는 데 필요한 모든 정보
- **카메라 블렌딩**: 카메라 전환 시 두 POV를 보간하여 부드러운 전환 구현
- **시각 효과**: 후처리, DOF(Depth of Field) 등의 시각 효과 설정

**흐름에서의 역할**:
1. `CalcCamera` 또는 `GetCameraView`에서 POV 계산
2. `DoUpdateCamera`에서 블렌딩 적용
3. `FillCameraCache`로 최종 POV를 캐시에 저장
4. 렌더링 시스템이 이 POV를 사용하여 화면 출력

---

# Render

Draw 프로세스는 Tick에서 호출되며 `GameViewport->Viewport->Draw();`를 통해 시작됩니다.

## 1. FViewport::Draw

### 역할
- 뷰포트의 렌더링 프로세스를 시작하는 진입점
- Canvas를 생성하고 ViewportClient의 Draw를 호출

### 주요 코드
```cpp
void FViewport::Draw( bool bShouldPresent /*= true */)
{
	UWorld* ViewportWorld = ViewportClient->GetWorld();
	
	// Canvas 생성
	// 캔버스는 뷰포트를 수행하는 FViewport의 하위 영역
	FCanvas Canvas(this, nullptr, ViewportWorld, 
		ViewportWorld ? ViewportWorld->GetFeatureLevel() : GMaxRHIFeatureLevel, 
		FCanvas::CDM_DeferDrawing, 
		ViewportClient->ShouldDPIScaleSceneCanvas() ? ViewportClient->GetDPIScale() : 1.0f);
	
	// 화면에 그릴 때 사용할 사각형 영역을 설정
	// 이 값이 설정되면 전체 화면 대신 그 사각형 안에만 렌더링을 하게 되고,
	// 설정하지 않으면 전체에 렌더링을 진행
	Canvas.SetRenderTargetRect(FIntRect(0, 0, SizeX, SizeY));
	{
		ViewportClient->Draw(this, &Canvas);
	}
}
```

---

## 2. UGameViewportClient::Draw

### 역할
- 씬 렌더링을 위한 ViewFamily 생성
- 각 로컬 플레이어에 대한 View 생성
- 렌더링 스레드로 렌더링 작업 전달

### 주요 코드
```cpp
void UGameViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{
	BeginDrawDelegate.Broadcast();

	UWorld* MyWorld = GetWorld();
	if (MyWorld == nullptr)
	{
		return;
	}

	// 매 프레임마다 씬을 렌더링하기 위해 생성되는 View에 대한 RAII인 ViewFamily 생성
	FSceneViewFamilyContext ViewFamily(
		FSceneViewFamily::ConstructionValues(InViewport, MyWorld->Scene, EngineShowFlags)
		.SetRealtimeUpdate(true)
		.SetRequireMobileMultiView(bRequireMultiView));

	// 각 로컬 플레이어에 대해 반복
	for (FLocalPlayerIterator Iterator(GEngine, MyWorld); Iterator; ++Iterator)
	{
		ULocalPlayer* LocalPlayer = *Iterator;
		if (LocalPlayer)
		{
			const int32 NumViews = bStereoRendering ? 
				GEngine->StereoRenderingDevice->GetDesiredNumberOfViews(bStereoRendering) : 1;
				
			for (int32 ViewIndex = 0; ViewIndex < NumViews; ++ViewIndex)
			{
				FVector	ViewLocation;
				FRotator ViewRotation;

				// 플레이어의 정보를 통하여 최종적인 View 생성(카메라 정보)
				FSceneView* View = LocalPlayer->CalcSceneView(
					&ViewFamily, ViewLocation, ViewRotation, InViewport, 
					nullptr, bStereoRendering ? ViewIndex : INDEX_NONE);
			}
		}
	}

	// 뷰에 대한 마지막 조정 기회 제공 함수
	FinalizeViews(&ViewFamily, PlayerViewMap);

	// ViewFamily에 있는 SceneView 정보(카메라 위치, 렌더링 설정 등)를
	// 렌더링 스레드로 전달하여 실제 렌더링 과정을 시작
	GetRendererModule().BeginRenderingViewFamily(SceneCanvas, &ViewFamily);

	EndDrawDelegate.Broadcast();
}
```

---

## 3. ULocalPlayer::CalcSceneView

### 역할
- SceneView 생성을 위한 초기화 옵션 계산
- 최종 FSceneView 객체 생성 및 ViewFamily에 추가

### 주요 코드
```cpp
FSceneView* ULocalPlayer::CalcSceneView(
	class FSceneViewFamily* ViewFamily,
	FVector& OutViewLocation,
	FRotator& OutViewRotation,
	FViewport* Viewport,
	class FViewElementDrawer* ViewDrawer,
	int32 StereoViewIndex)
{
	// View에 대한 종합적인 정보를 계산해서 저장
	FSceneViewInitOptions ViewInitOptions;
	if (!CalcSceneViewInitOptions(ViewInitOptions, Viewport, ViewDrawer, StereoViewIndex))
	{
		return nullptr;
	}

	// ViewInitOptions 정보를 이용하여 최종적인 View 생성
	FSceneView* const View = new FSceneView(ViewInitOptions);
	OutViewLocation = View->ViewLocation;
	OutViewRotation = View->ViewRotation;

	// Family에 저장
	ViewFamily->Views.Add(View);

	return View;
}
```

---

## 4. ULocalPlayer::CalcSceneViewInitOptions

### 역할
- View 초기화에 필요한 옵션 계산
- 투영 데이터(Projection Data) 계산
- View 유효성 검증

### 주요 코드
```cpp
bool ULocalPlayer::CalcSceneViewInitOptions(
	struct FSceneViewInitOptions& ViewInitOptions,
	FViewport* Viewport,
	class FViewElementDrawer* ViewDrawer,
	int32 StereoViewIndex)
{
	// 비정상적인 상황이면 실패
	if ((PlayerController == NULL) || (Size.X <= 0.f) || (Size.Y <= 0.f) || (Viewport == NULL))
	{
		return false;
	}

	// 투영 데이터(3차원 공간의 객체를 2차원 화면에 어떻게 표시할지) 계산해서 저장
	if (GetProjectionData(Viewport, /*inout*/ ViewInitOptions, StereoViewIndex) == false)
	{
		return false;
	}

	// 잘못된 View 데이터가 있으면 실패
	if (!ViewInitOptions.IsValidViewRectangle())
	{
		return false;
	}

	// ViewInitOptions에 필수 정보 저장
	ViewInitOptions.ViewActor = PlayerController->GetViewTarget();

	return true;
}
```

---

## 5. ULocalPlayer::GetProjectionData

### 역할
- 3D 공간을 2D 화면으로 투영하기 위한 데이터 계산
- Viewport의 크기와 비율을 기반으로 View Rectangle 설정
- View Matrix와 Projection Matrix 생성

### 주요 코드
```cpp
bool ULocalPlayer::GetProjectionData(
	FViewport* Viewport, 
	FSceneViewProjectionData& ProjectionData, 
	int32 StereoViewIndex) const
{
	// 비정상적인 상황이면 실패
	if ((Viewport == NULL) || (PlayerController == NULL) || 
	    (Viewport->GetSizeXY().X == 0) || (Viewport->GetSizeXY().Y == 0) || 
	    (Size.X == 0) || (Size.Y == 0))
	{
		return false;
	}
	
	// 0 ~ 1 비율을 Viewport의 Size에 맞게 계산해서 설정하는 로직
	int32 X = FMath::TruncToInt(Origin.X * Viewport->GetSizeXY().X);
	int32 Y = FMath::TruncToInt(Origin.Y * Viewport->GetSizeXY().Y);

	X += Viewport->GetInitialPositionXY().X;
	Y += Viewport->GetInitialPositionXY().Y;

	uint32 SizeX = FMath::TruncToInt(Size.X * Viewport->GetSizeXY().X);
	uint32 SizeY = FMath::TruncToInt(Size.Y * Viewport->GetSizeXY().Y);
	
	FIntRect UnconstrainedRectangle = FIntRect(X, Y, X+SizeX, Y+SizeY);
	ProjectionData.SetViewRectangle(UnconstrainedRectangle);
	
	// 카메라의 촬영 정보를 가져옴
	FMinimalViewInfo ViewInfo;
	GetViewPoint(/*out*/ ViewInfo);
	
	// View Matrix 생성
	ProjectionData.ViewOrigin = StereoViewLocation;
	ProjectionData.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
		FPlane(0,	0,	1,	0),
		FPlane(1,	0,	0,	0),
		FPlane(0,	1,	0,	0),
		FPlane(0,	0,	0,	1));
		
	// FMinimalViewInfo와 추가적인 데이터(Viewport 크기)를 기반으로 투영 행렬 계산
	FMinimalViewInfo::CalculateProjectionMatrixGivenView(
		ViewInfo, AspectRatioAxisConstraint, Viewport, /*inout*/ ProjectionData);
	
	return true;
}
```

## 6. ULocalPlayer::GetViewPoint

### 역할
- PlayerCameraManager로부터 카메라 정보 가져오기
- View 위치, 회전, FOV 등의 정보 수집

### 주요 코드
```cpp
void ULocalPlayer::GetViewPoint(FMinimalViewInfo& OutViewInfo) const
{
	if(PlayerController != NULL)
	{
		if (PlayerController->PlayerCameraManager != NULL)
		{
			// 카메라에 저장된 View, FOV 값 저장
			OutViewInfo = PlayerController->PlayerCameraManager->GetCameraCacheView();
			OutViewInfo.FOV = PlayerController->PlayerCameraManager->GetFOVAngle();

			// 카메라의 Location, Rotation 정보 저장
			PlayerController->GetPlayerViewPoint(
				/*out*/ OutViewInfo.Location, 
				/*out*/ OutViewInfo.Rotation);
		}
	}

	OutViewInfo.DesiredFOV = OutViewInfo.FOV;
}
```

---

## 전체 호출 흐름

```
Tick
 └─► GameViewport->Viewport->Draw()
      └─► FViewport::Draw()
           ├─► Canvas 생성
           └─► UGameViewportClient::Draw()
                ├─► ViewFamily 생성
                ├─► For each LocalPlayer:
                │    └─► ULocalPlayer::CalcSceneView()
                │         ├─► ULocalPlayer::CalcSceneViewInitOptions()
                │         │    └─► ULocalPlayer::GetProjectionData()
                │         │         └─► ULocalPlayer::GetViewPoint()
                │         └─► new FSceneView()
                ├─► FinalizeViews()
                └─► GetRendererModule().BeginRenderingViewFamily()
```

---

## 핵심 개념 정리

### Canvas
- 뷰포트의 하위 영역
- 실제 렌더링이 이루어지는 2D 영역
- 멀티플레이어 분할 화면 등에서 여러 Canvas 사용 가능

### ViewFamily
- 매 프레임마다 생성되는 View들의 컬렉션
- 씬 렌더링에 필요한 공통 설정 포함
- RAII 패턴으로 관리

### SceneView
- 특정 카메라 시점에서의 렌더링 정보
- View Matrix와 Projection Matrix 포함
- 각 LocalPlayer마다 생성

### Projection Data
- 3D 공간을 2D 화면으로 변환하는 수학적 정보
- View Rectangle, View Matrix, Projection Matrix 포함
- 카메라의 위치, 회전, FOV 등을 기반으로 계산

### View Matrix
- 카메라의 위치와 회전을 기반으로 생성
- 월드 좌표를 카메라 좌표계로 변환

### Projection Matrix
- 카메라 좌표계를 화면 좌표계로 변환
- FOV, 종횡비, Near/Far Plane 등의 정보 사용

