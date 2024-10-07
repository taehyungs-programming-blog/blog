---
layout: default
title: "02-01. PlaySession"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* ServerListen에 들어가기 전 ServerListen이 어떻게 호출되는지 확인이 필요해 PlaySession을 먼저 다룹니다.

---

* `RequestPlaySession`는 어디서 호출되었나? 궁금하겠지만 큰 의미가 없는게 PIE에서 Play 누르면 거의 바로 호출이 됨. 
* 즉 `RequestPlaySession`이전은 크게 볼 의미는 없다.

* 잠깐 복습, **Unreal CDO(Class Default Object)**
    * Unreal Engine에서 **CDO(Class Default Object)**는 클래스의 기본 객체를 의미합니다. 
    * CDO는 특정 클래스의 초기 기본값들을 저장하고 있으며, 새로운 인스턴스가 생성될 때 이 CDO를 기반으로 초기화됩니다. 
    * 이는 객체 생성 시 일관된 초기 상태를 보장하며, 메모리 사용과 성능 측면에서 효율성을 제공합니다.
    * **주요 특징:**
        * 초기값 저장: CDO는 클래스의 모든 프로퍼티에 대한 기본값을 저장합니다. 이를 통해 객체를 생성할 때마다 프로퍼티를 개별적으로 설정할 필요가 없습니다.
        * 메모리 효율성: 동일한 값들은 메모리에 한 번만 저장되며, 각 인스턴스는 이 값을 참조합니다.
        * Reflection 시스템과의 연동: CDO는 Unreal Engine의 Reflection 시스템과 밀접하게 연관되어 있어, 에디터 내에서의 프로퍼티 편집, 시리얼라이제이션, 복제 등 다양한 기능에 활용됩니다.
        * Garbage Collection: CDO는 Garbage Collector가 추적하는 루트 객체 중 하나로, 메모리 관리에 중요한 역할을 합니다.

```cpp
// when you press the PIE button, this method is called to queue the PIE request
// PIE 버튼을 누르면 이 메서드가 호출되어 PIE 요청을 큐에 넣습니다
void RequestPlaySession(const FRequestPlaySessionParams& InParams)
{
    // store our request to be operated on next Tick
    // 다음 Tick에서 처리될 요청을 저장합니다
    PlaySessionRequest = InParams;

    // usually this EditorPlaySettings are nullptr, so use the CDO of ULevelEditorPlaySettings:
    // 일반적으로 이 EditorPlaySettings는 nullptr이므로, ULevelEditorPlaySettings의 CDO를 사용합니다:
    //                                                                                                            
    //                                                                                                            
    //                                                                                                            
    //              C++ Compiled Version     │    Serializable C++                                                
    //              C++ 컴파일된 버전         │    직렬화 가능한 C++                                               
    //                                       │                                                                    
    //                                       │                                                                    
    //         class (C++)◄──────────────────┼───────UClass(Unreal)                                               
    //          │                            │        │                                                           
    //          └──Initialization-list       │        └──CDO (Class Default Object)                               
    //             초기화 리스트              │           CDO (클래스 기본 객체)                                   
    //                    ▲                  │                    │                                               
    //                    └──────────────────┼────────────────────┘                                               
    //                                       │     *** CDO is a format of serializable c++ initialization-list    
    //                                       │     *** CDO는 직렬화 가능한 C++ 초기화 리스트의 형식입니다         
    //                                       │                            ─────────────────────────────────────   
    //                                       │                                                                    
    //                                                                                                            
    if (!PlaySessionRequest->EditorPlaySettings)
    {
        PlaySessionRequest->EditorPlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
    }

    // now we duplicate their EditorPlaySettings so that we can mutate it as part of startup to help rule out invalid configuration combinations
    // 이제 EditorPlaySettings를 복제하여 시작 과정의 일부로 변경할 수 있게 합니다. 이는 잘못된 구성 조합을 배제하는 데 도움이 됩니다
    // we cannot use the CDO directly, so duplicate the CDO and use the duplicated instance instead:
    // - FObjectDuplicationParameters + StaticDuplicateObjectEx = duplication of UObject
    // CDO를 직접 사용할 수 없으므로, CDO를 복제하고 복제된 인스턴스를 대신 사용합니다:
    // - FObjectDuplicationParameters + StaticDuplicateObjectEx = UObject의 복제
    FObjectDuplicationParameters DuplicationParams(PlaySessionRequest->EditorPlaySettings, GetTransientPackage());
    PlaySessionRequest->EditorPlaySettings = CastChecked<ULevelEditorPlaySettings>(StaticDuplicateObjectEx(DuplicationParams));
}
```

```cpp
// UEditorEngine
virtual void Tick(float DeltaSeconds, bool bIdleMode) override
{
    // ...

    // kick off a play session request if one was queued up during the last frame
    // - RequestPlaySession() sets 'PlaySessionRequest'
    // when play-session-request is set(added), it processes in the UEditorEngine::Tick
    if (PlaySessionRequest.IsSet())
    {
        // PlaySessionRequest가 세팅되었다면 여기로 들어오게 된다.
        StartQueuedPlaySessionRequest();
    }

    // ...
```

```cpp
void StartQueuedPlaySessionRequest()
{
    // 여기서 뭔가를 처리하고 ...
    StartQueuedPlaySessionRequestImpl();

    // ensure the request is always reset after an attempt (which may fail), so that we don't get stuck in an infinite loop of start attempts
    // to accept another session request, it is natural to clear up right?
    // 시도 후(실패할 수 있음) 항상 요청을 리셋하여, 시작 시도의 무한 루프에 빠지지 않도록 합니다
    // 다른 세션 요청을 받아들이기 위해 정리하는 것이 자연스럽죠?
    PlaySessionRequest.Reset();
}
```

```cpp
/** start the queued PlaySessionRequest; after this is called the queued play session request will be cleared */
// 대기 중인 PlaySessionRequest를 시작합니다; 이 함수가 호출된 후에는 대기 중인 play session 요청이 지워집니다
virtual void StartQueuedPlaySessionRequestImpl()
{
    const ULevelEditorPlaySettings* EditorPlaySettings = PlaySessionRequest->EditorPlaySettings;
    check(EditorPlaySettings);

    // FPlayInEditorSessionInfo 참고 (1)
    PlayInEditorSessionInfo = FPlayInEditorSessionInfo();

    // we preserve PlaySessionRequest in PlayInEditorSessionInfo
    // PlaySessionRequest를 PlayInEditorSessionInfo에 보존합니다
    PlayInEditorSessionInfo->OriginalRequestParams = PlaySessionRequest.GetValue();

    // we'll branch primarily based on the Session Destination, because it affects which settings we apply and how
    // 세션 목적지에 따라 주로 분기합니다, 이는 어떤 설정을 적용할지와 어떻게 적용할지에 영향을 미치기 때문입니다
    // what we are interested in is PIE, whose SessionDestination is "InProcess"
    // 우리가 관심 있는 것은 PIE입니다, 이것의 SessionDestination은 "InProcess"입니다
    switch (PlaySessionRequest->SessionDestination)
    {
    case EPlaySessionDestinationType::InProcess:
        // create one-or-more PIE/SIE sessions inside of the current process
        // 현재 프로세스 내에서 하나 이상의 PIE/SIE 세션을 생성합니다
        // now we start to PIE!
        // 이제 PIE를 시작합니다!
        StartPlayInEditorSession(PlaySessionRequest.GetValue());
        break;
    //...
    }
}
```

### (1) FPlayInEditorSessionInfo

```cpp
/**
 * this stores transient information about the current Play Session for the duration of the session:
 * - this allows us to cache information across async processes and hold useful information for clients who want to Late Join
 */
/**
 * 이것은 현재 플레이 세션의 일시적인 정보를 세션 기간 동안 저장합니다:
 * - 이를 통해 비동기 프로세스 간에 정보를 캐시하고 나중에 참여하려는 클라이언트를 위한 유용한 정보를 보관할 수 있습니다
 */
struct FPlayInEditorSessionInfo
{
    FRequestPlaySessionParams OriginalRequestParams;

    /** how many PIE instances have we opened; external dedicated servers don't count (but internal ones do) */
    /** 몇 개의 PIE 인스턴스를 열었는지 나타냅니다; 외부 전용 서버는 포함되지 않지만 내부 서버는 포함됩니다 */
    int32 PIEInstanceCount;
};
```

* `StartPlayInEditorSession`내용이 길어져 다음장으로 대신합니다.

