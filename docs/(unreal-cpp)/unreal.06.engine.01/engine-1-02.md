---
layout: default
title: "02. Unreal Main Function ~ Tick"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Unreal의 Main함수는 어디일까?

* 모든 코드를 설명하지 않고 중요하다 느껴지는 부분만 설명.

```cpp
// LaunchWindows.cpp

int32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int32 nCmdShow)
{
	int32 Result = LaunchWindowsStartup(hInInstance, hPrevInstance, pCmdLine, nCmdShow, nullptr);
	LaunchWindowsShutdown();
    /*
    LAUNCH_API void LaunchWindowsShutdown()
    {
        // Final shut down.
        FEngineLoop::AppExit();

        // pause if we should
        if (GShouldPauseBeforeExit)
        {
            Sleep(INFINITE);
        }
    }
    */
	return Result;
}
```

```cpp
LAUNCH_API int32 LaunchWindowsStartup( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char*, int32 nCmdShow, const TCHAR* CmdLine )
{
    // ... arg를 처리

	else
	{
		// Try ~ Exception으로 잡아둠
            // SEH (structured exception handling)라한다. (아래참고)
#if !PLATFORM_SEH_EXCEPTIONS_DISABLED
		__try
#endif
 		{
			GIsGuarded = 1;
			// SEH로 예외처리를 해 두고 GuardedMainWrapper로 들어가게 된다.
			ErrorLevel = GuardedMainWrapper( CmdLine );
			GIsGuarded = 0;
		}
#if !PLATFORM_SEH_EXCEPTIONS_DISABLED
		__except( FPlatformMisc::GetCrashHandlingType() == ECrashHandlingType::Default
				? ( GEnableInnerException ? EXCEPTION_EXECUTE_HANDLER : ReportCrash(GetExceptionInformation()) )
				: EXCEPTION_CONTINUE_SEARCH )	
		{
			// Crashed.
			ErrorLevel = 1;
			if(GError)
			{
				GError->HandleError();
			}
			LaunchStaticShutdownAfterError();
			FPlatformMallocCrash::Get().PrintPoolsUsage();
			FPlatformMisc::RequestExit( true, TEXT("LaunchWindowsStartup.ExceptionHandler"));
		}
#endif
	}

	TRACE_BOOKMARK(TEXT("WinMain.Exit"));

	return ErrorLevel;
}
```

### SEH (structured exception handling)

* Structured Exception Handling (SEH)은 Microsoft Windows 운영 체제에서 제공하는 **오류 처리 메커니즘**입니다. 
* 이는 소프트웨어 개발자들이 프로그램 내에서 예외 상황을 감지하고, 이를 안정적으로 처리할 수 있도록 설계된 시스템입니다. 
* SEH는 프로그램 실행 중 발생할 수 있는 다양한 오류나 예외 상황을 효과적으로 관리하고 대응하기 위한 방법을 제공합니다.

* SEH의 핵심 개념은 **'try'와 'except' 블록을 사용**하는 것입니다. 이를 통해 개발자는 특정 코드 섹션에서 발생할 수 있는 예외를 명시적으로 처리할 수 있습니다.
    * Try Block: 예외가 발생할 가능성이 있는 코드를 포함합니다. 시스템은 이 블록 내의 코드를 실행하다가 예외를 감지하면 즉시 실행을 중지하고 관련된 'except' 블록으로 제어를 전달합니다.
    * Except Block: 예외 처리 로직을 포함합니다. 이 블록은 특정 예외 타입에 대응하여 실행될 코드를 정의하며, 예외 상황에 대한 복구 작업이나 로깅 등의 처리를 수행할 수 있습니다.

* SEH는 또한 예외 필터링(exception filtering)을 지원하여, 'except' 블록이 특정 조건에 따라 실행되도록 할 수 있습니다. 이를 통해 더 세밀한 예외 처리가 가능해집니다.

* SEH의 중요한 특징 중 하나는 **운영 체제 수준에서 지원**된다는 것입니다. 이는 프로그램이 운영 체제와 긴밀하게 통합되어 예외 상황을 보다 효율적으로 처리할 수 있음을 의미합니다. 예를 들어, 하드웨어 예외(예: 접근 위반)나 시스템 예외가 발생했을 때, SEH를 통해 이러한 예외를 적절히 처리하고 프로그램의 안정성을 유지할 수 있습니다.

* 개발자는 SEH를 사용하여 프로그램의 오류 처리 능력을 강화하고, 예외 상홍 발생 시 적절한 조치를 취할 수 있는 코드를 작성할 수 있습니다. 이를 통해 사용자 경험을 개선하고, 프로그램의 안정성과 신뢰성을 높일 수 있습니다.

---

## GuardedMainWrapper

```cpp
LAUNCH_API int32 GuardedMainWrapper( const TCHAR* CmdLine )
{
    // ...
    ErrorLevel = GuardedMain( CmdLine );
}
```

```cpp
int32 GuardedMain( const TCHAR* CmdLine )
{
    // ...

    // Don't tick if we're running an embedded engine - we rely on the outer
	// application ticking us instead.
	if (!GUELibraryOverrideSettings.bIsEmbedded)
	{
		while( !IsEngineExitRequested() )
		{
            // 여기서 Tick을 돌리게 된다.
			EngineTick();
            /*
            LAUNCH_API void EngineTick( void )
            {
                GEngineLoop.Tick();
            }
            */
		}
	}
}
```

---

## Tick

```cpp
// 이 함수에서 하는 일이 엄청나게 많은데
void FEngineLoop::Tick()
{
    // ...

    // 결국은 Main Engine의 Tick을 돌리는게 핵심이긴하다.
    // main game engine tick (world, game objects, etc.)
    GEngine->Tick(FApp::GetDeltaTime(), bIdleMode);
```

* 그럼 GEngine는 뭘까? -> 이후강의 진행