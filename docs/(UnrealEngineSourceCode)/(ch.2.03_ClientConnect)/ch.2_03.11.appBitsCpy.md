---
layout: default
title: "03-11. appBitsCpy"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
void appBitsCpy(uint8* Dest, int32 DestBit, uint8* Src, int32 SrcBit, int32 BitCount)
{
    // for calculation efficiency, it has several special case handing (for quicker calculation)

    // 계산 효율성을 위해 몇 가지 특별한 경우를 처리합니다 (빠른 계산을 위해)
    if (BitCount == 0) return;

    // special case - always at least one bit to copy, a maximum of 2 bytes to read, 2 to write - only touch bytes that are actually used
    // 1. BitCount <= 8, handling bits[0,7] 
    // - BitCount <= 8 implies that it touches 1-byte memory twice to read and twice to write:

    // 특별한 경우 - 항상 최소한 한 비트를 복사하며, 최대 2바이트를 읽고 2바이트를 씁니다 - 실제로 사용된 바이트만 만집니다
    // 1. BitCount <= 8, 비트[0,7] 처리
    // - BitCount <= 8은 읽기와 쓰기에 각각 1바이트 메모리를 두 번 사용함을 의미합니다

    //
    //   - when we copy bits from SRC two bits index[7] and index[8] to DEST                    
    //                                                                                          
    //     - NOTE that the below figures are based on little-endian (x86)                       

    //   - SRC에서 인덱스[7]과 인덱스[8]의 비트를 DEST로 복사할 때
    // 
    //     - 아래 그림은 리틀 엔디언(x86)을 기반으로 합니다
    //                                                                                          
    //       INDEX:   7   6   5   4   3   2   1   0     15  14  13  12  11  10  9   8           
    //              ┌───┬───┬───┬───┬───┬───┬───┬───┐ ┌───┬───┬───┬───┬───┬───┬───┬───┐         
    //        DEST: │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │         
    //              └─▲─┴───┴───┴───┴───┴───┴───┴───┘ └───┴───┴───┴───┴───┴───┴───┴─▲─┘         
    //                │                                                             │           
    //                │                                                             │           
    //                │                                                             │           
    //              ┌─┴─┬───┬───┬───┬───┬───┬───┬───┐ ┌───┬───┬───┬───┬───┬───┬───┬─┴─┐         
    //         SRC: │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │         
    //              └───┴───┴───┴───┴───┴───┴───┴───┘ └───┴───┴───┴───┴───┴───┴───┴───┘         
    //                                                                                          
    //     *** in this case, we touch 2-bytes from SRC to read and 2-bytes from DEST to write   
    //                                ────────────────────────     ──────────────────────────  

    //     *** 이 경우, 읽기 위해 SRC에서 2바이트를 읽고 쓰기 위해 DEST에서 2바이트를 씁니다
    //                                ────────────────────────     ──────────────────────────  
    if (BitCount <= 8)
    {
        // in this if-statement, (LastSrc - SrcIndex) and (LastDest - DestIndex) cannot exceed more than '1', cuz we are dealing with BitCount <= 8

        // 이 if문에서, (LastSrc - SrcIndex)와 (LastDest - DestIndex)는 '1'을 초과할 수 없습니다, 왜냐하면 BitCount <= 8을 처리하고 있기 때문입니다
        uint32 DestIndex    = DestBit / 8;
        uint32 SrcIndex     = SrcBit / 8;
        uint32 LastDest     = (DestBit + BitCount - 1) / 8;
        uint32 LastSrc      = (SrcBit + BitCount - 1) / 8;

        // maximum bit range[0, 7] can be inserted in same 1-byte
        // - this is because (SrcBit,DestBit) & 7!

        // 최대 비트 범위[0, 7]은 동일한 1바이트에 삽입될 수 있습니다
        // - 이는 (SrcBit, DestBit) & 7 때문입니다!
        uint32 ShiftSrc     = SrcBit & 7;
        uint32 ShiftDest    = DestBit & 7;

        // on BitCount <= 8, we can access maximum 2-byte memory:
        // - 0xFF is 255 and 0xFE is 254
        //   - 0xFF is 11111111(2)
        //   - 0xFE is 11111110(2) : 0xFE is pre-shifted left by 1: 11111111(2) << 1 = 11111110(2)
        // - why do we pre-shifted? we'll understand the reason in soon!

        // BitCount <= 8에서는 최대 2바이트의 메모리에 접근할 수 있습니다:
        // - 0xFF는 255이고 0xFE는 254입니다
        //   - 0xFF는 11111111(2)입니다
        //   - 0xFE는 11111110(2)입니다 : 0xFE는 왼쪽으로 1비트 프리시프트된 값입니다: 11111111(2) << 1 = 11111110(2)
        // - 왜 프리시프트를 했는지? 곧 그 이유를 알게 될 것입니다!
        uint32 FirstMask    = 0xFF << ShiftDest;
        uint32 LastMask     = 0xFE << ((DestBit + BitCount - 1) & 7); // pre-shifted left by 1

        // Accu is accumlator of intermediate calculation results
        // - when SrcIndex and LastSrc is same, all bits are converged into 1-byte

        // Accu는 중간 계산 결과의 누산기입니다
        // - SrcIndex와 LastSrc가 같을 때, 모든 비트는 1바이트로 모입니다
        uint32 Accu;
        if (SrcIndex == LastSrc)
            Accu = (Src[SrcIndex] >> ShiftSrc);
        else
            // how two bytes(uint8) are combined into single int-value(uint32) [feat. little-endian]
            //
            // - how two bytes (with little-endian) are combined into one integer (4 bytes)?       
            //                                                                                     
            //       ┌───┬───┬───┬───┬───┬───┬───┬───┐     ┌───┬───┬───┬───┬───┬───┬───┬───┐       
            //   [0]:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ [1]:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │       
            //       └───┴───┴───┴───┴───┴───┴───┴───┘     └───┴───┴───┴───┴───┴───┴───┴───┘       
            //       ─────────────┬───────────────────     ──────────────┬──────────────────       
            //                    │                                      │                         
            //                    └───────────────────┬──────────────────┘                         
            //                                        │                                            
            //                                        │ How to order them to make complete value?  
            //                                        │   - we expect 255([0]) + 256[1]            
            //                                        │                                            
            //                                        ▼                                            
            //           ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐         
            //           │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │         
            //           └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘         
            //                                                                                     

            // 두 바이트(uint8)를 하나의 정수 값(uint32)으로 어떻게 결합하는가 [feat. 리틀 엔디안]
            //
            // - 두 바이트를 (리틀 엔디안에서) 어떻게 하나의 정수(4바이트)로 결합하는가?
            // 
            //       (생략)
            // 
            // - 그들을 어떻게 정렬하여 완전한 값을 만들까?
            //   - 우리는 255([0]) + 256[1]을 기대합니다
            // 
            //       (결과 값 그림 생략)
            Accu = ((Src[SrcIndex] >> ShiftSrc) | (Src[LastSrc] << (8 - ShiftSrc)));
        
        if (DestIndex == LastDest)
        {
            // focus on how MultiMask(for uint32) is constructed
            // - "~LastMask" apply inverse-operator!
            //   - now we can understand why we try to pre-shifted on LastMask:
            //     - what the statement, "uint32 LastMask = 0xFE << ((DestBit + Count - 1) & 7)" means?
            //       - let's suppose (DestBit + Count - 1) & 7 as '1, 2, 3':
            //                                                                                                              
            //                                                     left-shift 1 (<< 1)    ┌───┬───┬───┬───┬───┬───┬───┬───┐ 
            //                                                   ───────────────────────► │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 
            //              ┌───┬───┬───┬───┬───┬───┬───┬───┐                             └───┴───┴───┴───┴───┴───┴───┴───┘ 
            //         0xFE:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │                                                               
            //              └───┴───┴───┴───┴───┴───┴───┴───┘      left-shift 2 (<< 2)    ┌───┬───┬───┬───┬───┬───┬───┬───┐ 
            //                                                   ───────────────────────► │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 
            //                                                                            └───┴───┴───┴───┴───┴───┴───┴───┘ 

            // MultiMask(uint32)이 어떻게 구성되는지에 초점을 맞춥니다
            // - "~LastMask"는 반전 연산자를 적용합니다!
            //   - 이제 왜 LastMask에 프리시프트를 했는지 이해할 수 있습니다:
            //     - "uint32 LastMask = 0xFE << ((DestBit + Count - 1) & 7)" 문장은 무엇을 의미합니까?
            //       - (DestBit + Count - 1) & 7을 '1, 2, 3'으로 가정해 봅시다:
            // 
            //       (그림 생략)
            //
            //       - ~연산자를 사용하여 마스킹 기능을 이해해 봅시다:
            //       
            //       (생략)
            //
            // - "(DestBit + Count - 1) & 7"이 0일 때 "FirstMask & ~LastMask"의 예상 값은 무엇입니까?
            //   - 2바이트의 통합 마스크입니다:
            //           
            //       (그림 생략)
            uint32 MultiMask = FirstMask & ~LastMask;

            // based on what we covered, it is easy to understand:
            // - Dest[DestIndex] & ~MultiMask --> clearing the value
            // - (Accu << ShiftDest) & MultiMask --> masking the value
            // - (Dest[DestIndex] & ~MultiMask) | ((Accu << ShiftDest) & MultiMask) --> applying(copying) 'Accu' value to destination
            //
            // - lastly, what does it mean force-casting by "(uint8)"?:
            //                                                                                                                                           
            //                               (uint8) │ (uint8)                                                                                           
            //    (dropped)                          │                            (retained)                                                             
            //        ◄──────────────────────────────┼──────────────────────────────►                                                                    
            //       ┌───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┐     force-casting by uint8    ┌───┬───┬───┬───┬───┬───┬───┬───┐   
            //       │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │  ──────────────────────────►  │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │   
            //       └───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┘                               └───┴───┴───┴───┴───┴───┴───┴───┘   
            //                                       │                                                                                                   
            //                                       │                                                                                                   
            //                                       │                                                                                                   
            //                                                                                                                                           

            // 다룬 내용을 기반으로 하면 이해하기 쉽습니다:
            // - Dest[DestIndex] & ~MultiMask --> 값을 지웁니다
            // - (Accu << ShiftDest) & MultiMask --> 값을 마스킹합니다
            // - (Dest[DestIndex] & ~MultiMask) | ((Accu << ShiftDest) & MultiMask) --> 'Accu' 값을 대상에 적용(복사)합니다
            //
            // - 마지막으로 "(uint8)"로 강제 캐스팅한다는 것은 무엇을 의미합니까?
            // 
            // (그림 생략)
            Dest[DestIndex] = (uint8)((Dest[DestIndex] & ~MultiMask) | ((Accu << ShiftDest) & MultiMask));
        }
        else
        {
            Dest[DestIndex] = (uint8)((Dest[DestIndex] & ~FirstMask) | ((Accu << ShiftDest) & FirstMask));

            // (8 - ShiftDest) means dropping the bits applied for DestIndex

            // (8 - ShiftDest)는 DestIndex에 적용된 비트를 버린다는 것을 의미합니다
            Dest[LastDest] = (uint8)((Dest[LastDest] & LastMask) | ((Accu >> (8 - ShiftDest)) & ~LastMask));
        }

        return;
    }
}
```