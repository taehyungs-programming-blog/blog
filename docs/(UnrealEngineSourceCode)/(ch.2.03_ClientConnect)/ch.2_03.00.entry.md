---
layout: default
title: "([Network] 03. ClientConnect)"
parent: "(UE SourceCode 분석 🤖)"
has_children: true
nav_order: 3
---

```cpp
// 역시 시작은 StartPlayInEditorGameInstance로 부터 시작된다.

/** called to actually start the game when doing play/simulate in editor */
/** 에디터에서 플레이/시뮬레이션을 할 때 실제로 게임을 시작하기 위해 호출됨 */
virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
    UEditorEngine* const EditorEngine = CastChecked<UEditorEngine>(GetEngine());
    {
        // for clients, just connect to the server
        // for networking, we add this:
        // - in networking, client-side and server(or standalone) game has different path

        // 클라이언트의 경우, 단순히 서버에 연결
        // 네트워킹을 위해 이것을 추가합니다:
        // - 네트워킹에서 클라이언트 측과 서버(또는 독립 실행형) 게임은 다른 경로를 가집니다
        if (Params.NetMode == PIE_Client)
        {
            // 이제 여기를 볼 차례

            // BaseURL has (127.0.0.1:1777)
            // BaseURL은 (127.0.0.1:1777)을 가집니다
            FURL BaseURL = WorldContext->LastURL;
            FString URLString(TEXT("127.0.0.1"));

            // we set our server port as 17777, which will be overriden
            // 서버 포트를 17777로 설정하지만, 이는 덮어쓰여질 것입니다
            uint16 ServerPort = 0;
            if (Params.EditorPlaySettings->GetServerPort(ServerPort))
            {
                URLString += FString::Printf(TEXT(":%hu"), ServerPort);
            }

            // before getting into UEngine::Browse, see rest of code below:
            // UEngine::Browse로 들어가기 전에, 아래 코드를 보세요:
            if (EditorEngine->Browse(*WorldContext, FURL(&BaseURL, *URLString, (ETravelType)TRAVEL_Absolute), Error) == EBrowseReturnVal::Pending)
            {
                // - we are waiting to complete connect process
                // - 연결 프로세스가 완료되기를 기다리고 있습니다
                EditorEngine->TransitionType = ETransitionType::WaitingToConnect;
            }
            else
            {
                return FGameInstancePIEResult::Failure();
            }
        }
```

```cpp
class UEngine : public UObject, public FExec
{
    //...

/** browse to a specified URL, relative to the current one */
/** 지정된 URL로 이동합니다. 현재 URL을 기준으로 상대적입니다 */

// we can think of 'Browse' as browsing session game provided by server, so first connect to the server
// 'Browse'를 서버가 제공하는 세션 게임을 탐색하는 것으로 생각할 수 있습니다. 따라서 먼저 서버에 연결합니다
virtual EBrowseReturnVal::Type Browse(FWorldContext& WorldContext, FURL URL, FString& Error)
{
    //...

    if (URL.IsLocalInternal())
    {
        //...
    }
    else if (URL.IsInternal() && GIsClient)
    {
        // we are create UPendingNetGame:
        // - server has infromation which map we have to load
        // - to get map information, first we need to connect the server
        // - before connecting to server, we create UPendingNetGame, temporary stage
        //   - to connect the server, we need a NetDriver to communicate
        //   - note that we are putting new instance of UPendingNetGame to WorldContext's PendingNetGame (*** see FWorldContext::PendingNetGame)

        // UPendingNetGame을 생성합니다:
        // - 서버는 우리가 로드해야 할 맵에 대한 정보를 가지고 있습니다
        // - 맵 정보를 얻기 위해서는 먼저 서버에 연결해야 합니다
        // - 서버에 연결하기 전에 UPendingNetGame을 생성합니다. 이는 임시 단계입니다
        //   - 서버에 연결하기 위해서는 통신을 위한 NetDriver가 필요합니다
        //   - 새로운 UPendingNetGame 인스턴스를 WorldContext의 PendingNetGame에 넣고 있음에 주목하세요 (*** FWorldContext::PendingNetGame 참조)
        
        //   - before looking into InitNetDriver(), see that we'll return EBrowseReturnVal::Pending:
        //   - this is because, we don't know which map do we have to load!

        //   - InitNetDriver()를 살펴보기 전에, EBrowseReturnVal::Pending을 반환할 것임을 확인하세요:
        //   - 이는 우리가 어떤 맵을 로드해야 할지 아직 모르기 때문입니다!
        WorldContext.PendingNetGame = NewObject<UPendingNetGame>();
        WorldContext.PendingNetGame->Initialize(URL);
        WorldContext.PendingNetGame->InitNetDriver();

        return EBrowseReturnVal::Pending;
    }
}
```
