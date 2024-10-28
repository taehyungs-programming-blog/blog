---
layout: default
title: "03-08. FNetConnectionIdHandler"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// see Allocate() and Free()
// - what is the benefit of using TBitArray<> over TArray<int32>?
//   - we can save x32 better memory usage:
//     - int32 -> 32 bits 
//     - int32 can hold 32 indices
// - see PostInitProperties (goto 015: ClientConnect)

// Allocate()와 Free()를 보세요
// - TBitArray<>를 TArray<int32> 대신 사용하는 이점은 무엇일까요?
//   - 32배 더 나은 메모리 사용이 가능합니다:
//     - int32는 32비트입니다
//     - int32는 32개의 인덱스를 저장할 수 있습니다
// - PostInitProperties를 보세요 (015: ClientConnect로 이동)

class FNetConnectionIdHandler
{
    FNetConnectionIdHandler()
        : IdHint(0)
    {
    }

    ~FNetConnectionIdHandler() {}

    // FNetConnectionIdHandler::Init() is called UNetDriver::PostInitProperties()

    // FNetConnectionIdHandler::Init()은 UNetDriver::PostInitProperties()에서 호출됩니다

    void Init(uint32 IdCount)
    {
        check(IdCount > 0);
        UsedIds.Init(false, IdCount + 1);
        // treat 0 as an invalid connection ID; it's used by NetConnection CDOs

        // 0을 유효하지 않은 연결 ID로 취급합니다; NetConnection CDO에서 사용됩니다

        UsedIds[0] = true;
    }

    uint32 Allocate()
    {
        uint32 Id = UsedIds.FindAndSetFirstZeroBit(IdHint);
        IdHint = Id + 1;
        return Id != INDEX_NONE ? Id : 0u;
    }

    void Free(uint32 Id)
    {
        UsedIds[Id] = (Id == 0u);
        IdHint = FMath::Min(IdHint, Id);
    }

    TBitArray<> UsedBits;
    uint32 IdHint;
};
``` 