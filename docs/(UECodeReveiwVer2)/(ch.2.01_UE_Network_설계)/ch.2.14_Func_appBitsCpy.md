---
layout: default
title: "02-14. [Func] appBitsCpy"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* BitWriter에서 사용되는 함수
* 함수의 목적
    * 비트 단위로 메모리를 복사하는 최적화된 함수입니다
    * 출발지(Src)에서 목적지(Dest)로 지정된 비트 수(BitCount)만큼 복사합니다

```cpp
void appBitsCpy(
    uint8* Dest,     // 목적지 버퍼
    int32 DestBit,   // 목적지에서 시작할 비트 위치
    uint8* Src,      // 출발지 버퍼
    int32 SrcBit,    // 출발지에서 시작할 비트 위치
    int32 BitCount   // 복사할 비트 수
)
```

```cpp
// 8비트 이하 복사 (BitCount <= 8)
if (BitCount <= 8) {
    // 1. 바이트 인덱스 계산
    uint32 DestIndex = DestBit / 8;  // 목적지 바이트 위치
    uint32 SrcIndex = SrcBit / 8;    // 출발지 바이트 위치
    
    // 2. 비트 시프트 값 계산 (0-7 사이)
    uint32 ShiftSrc = SrcBit & 7;    // 출발지 비트 오프셋
    uint32 ShiftDest = DestBit & 7;  // 목적지 비트 오프셋
    
    // 3. 마스크 생성
    uint32 FirstMask = 0xFF << ShiftDest;  // 첫 바이트 마스크
    
    // 4. 데이터 복사 및 마스킹
    Dest[DestIndex] = (
        (Dest[DestIndex] & ~MultiMask) |  // 기존 데이터 보존
        ((Accu << ShiftDest) & MultiMask)  // 새 데이터 삽입
    );
}
```

```cpp
// 8비트 초과 복사 (BitCount > 8)
else {
    // 1. 초기 설정
    uint32 DestIndex = DestBit / 8;
    uint32 SrcIndex = SrcBit / 8;
    
    // 2. 시프트 카운트 계산
    int32 ShiftCount = (DestBit & 7) - (SrcBit & 7);
    
    // 3. 리드인(Lead-in) - 첫 바이트 복사
    BitAccu = Src[SrcIndex] << ShiftCount;
    Dest[DestIndex] = (BitAccu & FirstSrcMask) | 
                      (Dest[DestIndex] & ~FirstSrcMask);
    
    // 4. 메인 루프 - 전체 바이트 복사
    for (; FullLoop > 1; FullLoop--) {
        BitAccu = (Src[SrcIndex] << ShiftCount) + BitAccu) >> 8;
        Dest[DestIndex++] = BitAccu;
        SrcIndex++;
    }
    
    // 5. 리드아웃(Lead-out) - 마지막 바이트 처리
    // 남은 비트들 처리
}
```

---

