---
layout: default
title: "02-09. PacketHandler"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/**
 * this class maintains an array of all PacketHandler Components and forwards incoming and outgoing packets the each component
 */
class PacketHandler
{
    // ...

    /** state of the handler */
    UE::Handler::State State;

    /** mode of the handler, client or server */
    Handler::Mode Mode;

    /** the maximum supported packet size (reflects UNetConnection::MaxPacket) */
    uint32 MaxPacketBits;

    /** maximum number of outgoing packet bits supported (automatically calculated to factor in other HandlerComponent reserved bits) */
    uint32 MaxOutgoingBits;

    /** the HandlerComponent pipeline, for processing incoming/outgoing packets */
    TArray<TSharedPtr<HandlerComponent>> HandlerComponents;

    /** packets that are buffered while HandlerComponents are being initialized */
    TArray<BufferedPacket*> BufferedPackets;

    /** packets that are queued to be sent when handler is ticked */
    TQueue<BufferedPacket*> QueuedPackets;

    /** packets that are buffered while HandlerComponents are being initialized */
    TArray<BufferedPacket*> BufferedConnectionlessPackets;

    /** packets that are queued to be sent when handler is ticked */
    TQueue<BufferedPacket*> QueuedConnectionlessPackets;
};
```

* 함수를 좀 더 살펴보자면

```cpp
/** handles initialization of manager */
void Initialize(UE::Handler::Mode InMode, uint32 InMaxPacketBits, bool bConnectionlessOnly=false,
    TSharedPtr<class IAnalyticsProvider> InProvider=nullptr, FDDoSDetection* InDDoS=nullptr, FName InDriverProfile=NAME_None)
{
    // attention to what Mode is (currently: Server)
    // - in PacketHandler, we set MaxPacketBits considering MTU(MSS)
    Mode = InMode;
    MaxPacketBits = InMaxPacketBits;
}
```

```cpp
TSharedPtr<HandlerComponent> AddHandler(const FString& ComponentStr, bool bDeferInitialize=false)
{
    TSharedPtr<HandlerComponent> ReturnVal = nullptr;
    if (!ComponentStr.IsEmpty())
    {
        // extract ComponentName and ComponentOptions from ComponentStr
        // ComponentStr에서 ComponentName과 ComponentOptions를 추출합니다
        //                                                                         
        //  Engine.EngineHandlerComponentFactory(StatelessConnectHandlerComponent) 
        //  ──────────────────▲───────────────── ────────────▲───────────────────  
        //                    │                              │                     
        //                    │                              │                     
        //                ComponentName                 ComponentOption            
        //                                                                         
        //
        FString ComponentName;
        FString ComponentOptions;
        for (int32 i = 0; i < ComponentStr.Len(); ++i)
        {
            TCHAR c = ComponentStr[i];

            // Parsing Options
            // 옵션 파싱
            if (c == '(')
            {
                // Skip '('
                // '(' 건너뛰기
                ++i;

                // Parse until end of options
                // 옵션의 끝까지 파싱
                for (; i<ComponentStr.Len(); i++)
                {
                    c = ComponentStr[i];

                    // End of options
                    // 옵션의 끝
                    if (c == ')')
                    {
                        break;
                    }
                    // Append char to options
                    // 옵션에 문자 추가
                    else
                    {
                        ComponentOptions.AppendChar(c);
                    }
                }
            }
            // Append char to component name if not whitespace
            // 공백이 아니면 컴포넌트 이름에 문자 추가
            else if (c != ' ')
            {
                ComponentName.AppendChar(c);
            }
        }

        // ComponentName is 'Engine.EngineHandlerComponentFactory'
        // ComponentName은 'Engine.EngineHandlerComponentFactory'입니다
        if (ComponentName != TEXT("ReliabilityHandlerComponent"))
        {
            int32 FactoryComponentDelim = ComponentName.Find(TEXT("."));
            if (FactoryComponentDelim != INDEX_NONE)
            {
                // every HandlerComponentFactory type has one instance, loaded as a named singleton
                // 모든 HandlerComponentFactory 타입은 하나의 인스턴스를 가지며, 이름이 지정된 싱글톤으로 로드됩니다
                // singleton name becomes "EngineHandlerComponentFactory_Singleton"
                // - by delimiter("."), we split left and right strings
                // 싱글톤 이름은 "EngineHandlerComponentFactory_Singleton"이 됩니다
                // - 구분자(".")로 왼쪽과 오른쪽 문자열을 분리합니다
                FString SingletonName = ComponentName.Mid(FactoryComponentDelim + 1) + TEXT("_Singleton");

                // Factory is exepcted to be the instance of UEngineHandlerComponentFactory
                // Factory는 UEngineHandlerComponentFactory의 인스턴스일 것으로 예상됩니다
                UHandlerComponentFactory* Factory = FindFirstObject<UHandlerComponentFactory>(*SingletonName, EFindFirstObjectOptions::NativeFirst | EFindFirstObjectOptions::EnsureIfAmbiguous);
                if (Factory == nullptr)
                {
                    UClass* FactoryClass = StaticLoadClass(UHandlerComponentFactory::StaticClass(), nullptr, *ComponentName);
                    if (FactoryClass)
                    {
                        Factory = NewObject<UHandlerComponentFactory>(GetTransientPackage(), FactoryClass, *SingletonName);
                    }
                }

                // EngineHandlerComponentFactory가 생성되어 Factory에 할당됨
                if (Factory != nullptr)
                {
                    ReturnVal = Factory->CreateComponentInstance(ComponentOptions);
                }
            }
            else
            {
                //...
            }

            // ReturnVal(HandlerComponent) should be valid component
            // - see PacketHandler::AddHandler (should be aware of bDeferInitialize as 'true')
            // ReturnVal(HandlerComponent)는 유효한 컴포넌트여야 합니다
            // - PacketHandler::AddHandler를 참조하세요 (bDeferInitialize가 'true'임을 주의하세요)
            if (ReturnVal.IsValid())
            {
                AddHandler(ReturnVal, bDeferInitialize);
            }
        }
    }

    return ReturnVal;
}
```