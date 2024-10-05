---
layout: default
title: "03-04. FBitWriter"
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
/** optimized arbitrary bit range memory copy routine */
void appBitsCpy(uint8* Dest, int32 DestBit, uint8* Src, int32 SrcBit, int32 BitCount)
{
    // for calculation efficiency, it has several special case handing (for quicker calculation)
    if (BitCount == 0) return;

    // special case - always at least one bit to copy, a maximum of 2 bytes to read, 2 to write - only touch bytes that are actually used
    // 1. BitCount <= 8, handling bits[0,7] 
    // - BitCount <= 8 implies that it touches 1-byte memory twice to read and twice to write:
    //
    //   - when we copy bits from SRC two bits index[7] and index[8] to DEST                    
    //                                                                                          
    //     - NOTE that the below figures are based on little-endian (x86)                       
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
    if (BitCount <= 8)
    {
        // in this if-statement, (LastSrc - SrcIndex) and (LastDest - DestIndex) cannot exceed more than '1', cuz we are dealing with BitCount <= 8
        uint32 DestIndex    = DestBit / 8;
        uint32 SrcIndex     = SrcBit / 8;
        uint32 LastDest     = (DestBit + BitCount - 1) / 8;
        uint32 LastSrc      = (SrcBit + BitCount - 1) / 8;

        // maximum bit range[0, 7] can be inserted in same 1-byte
        // - this is because (SrcBit,DestBit) & 7!
        uint32 ShiftSrc     = SrcBit & 7;
        uint32 ShiftDest    = DestBit & 7;

        // on BitCount <= 8, we can access maximum 2-byte memory:
        // - 0xFF is 255 and 0xFE is 254
        //   - 0xFF is 11111111(2)
        //   - 0xFE is 11111110(2) : 0xFE is pre-shifted left by 1: 11111111(2) << 1 = 11111110(2)
        // - why do we pre-shifted? we'll understand the reason in soon!
        uint32 FirstMask    = 0xFF << ShiftDest;
        uint32 LastMask     = 0xFE << ((DestBit + BitCount - 1) & 7); // pre-shifted left by 1

        // Accu is accumlater of intermediate calculation results
        // - when SrcIndex and LastSrc is same, all bits are converged into 1-byte
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
            //
            //       - with ~operator, try to understand masking functionality:
            //       
            //              ┌───┬───┬───┬───┬───┬───┬───┬───┐        DATA & MASK          ┌───┬───┬───┬───┬───┬───┬───┬───┐                  
            //         DATA:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ ────────────┬─────────────► │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ : CLEARING***    
            //              └───┴───┴───┴───┴───┴───┴───┴───┘             │               └───┴───┴───┴───┴───┴───┴───┴───┘                  
            //              ┌───┬───┬───┬───┬───┬───┬───┬───┐             │               ┌───┬───┬───┬───┬───┬───┬───┬───┐                  
            //         MASK:│ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ ────────────┴─────────────► │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 0 │ : MASKING***     
            //              └───┴───┴───┴───┴───┴───┴───┴───┘        DATA & (~MASK)       └───┴───┴───┴───┴───┴───┴───┴───┘                  
            //                                                                                                                               
            //
            // - what is expected value for "FirstMask & ~LastMask" when "(DestBit + Count - 1) & 7" is 0?
            //   - unified mask of 2-bytes:
            //           
            //                ┌───┬───┬───┬───┬───┬───┬───┬───┐                                                                                                      
            //      FirstMask:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 ├───────┐ FirstMask & (~LastMask)    ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐ 
            //                └───┴───┴───┴───┴───┴───┴───┴───┘       ├──────────────────────────► │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 
            //                ┌───┬───┬───┬───┬───┬───┬───┬───┐       │                            └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘ 
            //       LastMask:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 ├───────┘                                                                                              
            //                └───┴───┴───┴───┴───┴───┴───┴───┘                                                                                                      
            //                                                                                                                                                                                                                                                                                                                                                                        
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
            Dest[DestIndex] = (uint8)((Dest[DestIndex] & ~MultiMask) | ((Accu << ShiftDest) & MultiMask));
        }
        else
        {
            Dest[DestIndex] = (uint8)((Dest[DestIndex] & ~FirstMask) | ((Accu << ShiftDest) & FirstMask));

            // (8 - ShiftDest) means dropping the bits applied for DestIndex
            Dest[LastDest] = (uint8)((Dest[LastDest] & LastMask) | ((Accu >> (8 - ShiftDest)) & ~LastMask));
        }

        return;
    }

    // main copier, use byte sized shifting; minimum size is 9 bits, so at least 2 reads and 2 writes
    // note that LastDest and LastSrc are not subtrack '1'
    uint32 DestIndex    = DestBit / 8;
    uint32 LastDest     = (DestBit + BitCount) / 8;
    uint32 SrcIndex     = SrcBit / 8;
    uint32 LastSrc      = (SrcBit + BitCount) / 8;

    uint32 FirstSrcMask = 0xFF << (DestBit & 7);
    uint32 LastSrcMask  = 0xFF << ((DestBit + BitCount) & 7);
    
    // what 'ShiftCount' means?
    // - DestBit: 7, SrcBit: 1 -> ShiftCount = 6
    // - DestBit: 1, SrcBit: 7 -> ShiftCount = -6
    // - what does it means when ShiftCoutn < 0? let's try to understand the below code
    int32 ShiftCount    = (DestBit & 7) - (SrcBit & 7);

    // note subtracking '1', we can calculate DestLoop/SrcLoop correctly
    int32 DestLoop      = LastDest - DestIndex;
    int32 SrcLoop       = LastSrc - SrcIndex;
    uint32 FullLoop;
    uint32 BitAccu;

    // lead-in needs to read 1 or 2 source bytes depending on alignment
    // 1. LEAD-IN
    // 
    // - When ShiftCount >= 0:                                                                                                                                   
    //                          arrow direction is 5,6,7 increasing order! (NOT OPPOSITE)                                                                        
    //                             │                                                                                                                             
    //                             │                                                                                                                             
    //                             │                                                                                                                             
    //                             │ (DestBit & 7)==5                                                                                                            
    //                           ◄─▼────── │                                                                                                                     
    //                           ┌───┬───┬─▼─┬───┬───┬───┬───┬───┐                                                                                               
    //                      DEST:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │                                                                                               
    //                           └───┴───┴───┴───┴───┴───┴───┴───┘                                                                                               
    //                           ┌───┬───┬───┬───┬───┬───┬───┬───┐                                                                                               
    //                       SRC:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │                                                                                               
    //                        │  └───┴───┴───┴───┴───┴─▲─┴───┴───┘                                                                                               
    //                        │  ◄───────────────────  │                                                                                                         
    //    - ShiftCount == 3   │                  (SrcBit & 7)==2 │                                                                                               
    //    - SRC << ShiftCount │                                  │                                                                                               
    //                        │                      ┌───────────┼───────────┐(BitAccu & FirstSrcMask)                                                           
    //                        ▼  ┌───┬───┬───┬───┬───┼───┬───┬───┼───┬───┬───┼───┬───┬───┬───┬───┐                                                               
    //                   BitAccu:│ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                               
    //                           └───┴───┴───┴───┴───┼───┴───┴───┼───┴───┴───┼───┴───┴───┴───┴───┘                                                               
    //                           ┌───┬───┬───┬───┬───┼───┬───┬───┼───┬───┬───┼───┬───┬───┬───┬───┐                                                               
    //              FirstSrcMask:│ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                               
    //      0xFF << (DestBit & 7)└───┴───┴───┴───┴───┼───┴───┴───┼───┴───┴───┼───┴───┴───┴───┴───┘                                                               
    //                                               └───────────┼───────────┘                                                                                   
    //                                                           │           ┌───────────────────┐                                                               
    //                           ┌───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┼───┬───┬───┬───┬───┤                                                               
    //             ~FirstSrcMask:│ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │                                                               
    //                           └───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┼───┴───┴───┴───┴───┤                 ─                                             
    //                                                           ├───┬───┬───┼───┬───┬───┬───┬───┤                                                               
    //                                                      DEST:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │                                                               
    //                                                           ├───┴───┴───┼───┴───┴───┴───┴───┤                                                               
    //                                                           │           └───────────────────┘ DEST & (~FirstSrcMask)                                        
    //                                                           │                                                                                               
    //                                                           │                                                                                               
    //                                                           │                                                                                               
    //                                                           │                                                                                               
    //                                                 ◄─────────┼─────────►                                                                                     
    //                2. restof BitAccum is appied in FAST-LOOP  │  1,LEAD-IN:                                                                                   
    //                                                           │     Dest[DestIndex] = (uint8)((BitAccu & FirstSrcMask) | (Dest[DestIndex] & ~FirstSrcMask));  
    //                                                                                   ───────                                                                 
    //                                                                                    *** masking out!                                                       
    //                                                                                                                                                           
    // - When ShiftCount < 0:                                                                                                                                                                                                                                                                                                                                                                                                            
    //                                                                                                                                                                                                                            
    //                                           (DestBit & 7)==2                                                                                                                                                                 
    //                                                   │                                                                                                                                                                        
    //                             ┌───┬───┬───┬───┬───┬─▼─┬───┬───┐                                                                                                                                                              
    //                        DEST:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │                                                                                                                                                              
    //                             └───┴───┴───┴───┴───┴───┴───┴───┘                                                                                                                                                              
    //                             ┌───┬───┬───┬───┬───┬───┬───┬───┐                                                                                                                                                              
    //                      SRC[0]:│ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                                                                                                                              
    //                          │  └───┴───┴─▲─┴───┴───┴───┴───┴───┘             *** == BitAccu1                                                                                                                                  
    //                          │            │                                 ────────────────────────────────────                                                                                                               
    //   - ShiftCount == -3+8=5 │    (SrcBit & 7)==5                           ((uint32)Src[SrcIndex] << ShiftCount) + (BitAccu)                                                                                                  
    //   - SRC[0] << ShiftCount │                                              ┌───────────────────────────────────────────┐                                                                                                      
    //                          │                                              │                                           │                                                                                                      
    //                          ▼  ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐                                                              
    //                     BitAccu:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                              
    //                             └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘                                                              
    //                                                                         │                                           │                                                                                                      
    //                             ┌───┬───┬───┬───┬ ──┬───┬───┬───┐           │                                           │                                                                                                      
    //                      SRC[1]:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │           │                                           │                                                                                                      
    //                          │  └───┴───┴───┴───┴───┴───┴───┴───┘           │                                           │                                                                                                      
    //  - ShiftCount == 5+8=13  │                                              │                                           │                                                                                                      
    //  - SRC[1] << ShiftCount  │                                  │           │                   │                       │       │                                                                                              
    //                          ▼  ┌───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┼───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┼───┬───┼───┬───┬───┬───┬───┬───┬───┬───┐                                                              
    //                    BitAccu1:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                              
    //                          │  └───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┼───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┼───┴───┼───┴───┴───┴───┴───┴───┴───┴───┘                                                              
    //                          │                                  │           │                   │                       │       │                                                                                              
    //                          │                                              │                                           │                                                                                                      
    //       BitAccu + BitAccu1 │                                              └───────────────────────────────────────────┘                                                                                                      
    //                          │                                                                                                                                                                                                 
    //                          │                                              ┌───────────────────────────────────────────┐                                                                                                      
    //                          ▼  ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐                                                              
    //                     BitAccu:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │                                                              
    //                          │  └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘                                                              
    //                          │                                              └───────────────────────────────────────────┘                                                                                                      
    //                          │                                                                                                                                                                                                 
    //             BitAccu >> 8 │                                                          right-shfit 8       │           │       │                                                                                              
    //                          │                                                ─────────────────────────────►│           ├───────┼───────────────────────┐                                                                      
    //                          ▼  ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┼───┬───┼───┬───┬───┬───┬───┬───┼───┬───┐                                                              
    //                     BitAccu:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │                                                              
    //                             └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┼───┴───┼───┴───┴───┴───┴───┴───┼───┴───┘                                                              
    //                                                                                                         │           │       │                       │                                                                      
    //                                                                                                         │           │       │                       │                                                                      
    //                                                                                                         │           │       │                       │                                                                      
    //                                                                                             ┌───┬───┬───┼───┬───┬───┼───┬───┼───┬───┬───┬───┬───┬───┼───┬───┐                                                              
    //                                                                                FirstSrcMask:│ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │                                                              
    //                                                                        0xFF << (DestBit & 7)└───┴───┴───┼───┴───┴───┼───┴───┼───┴───┴───┴───┴───┴───┼───┴───┘                                                              
    //                                                                                                         │           ├───────┼───────────────────────┘                                                                      
    //                                                                                                         │           │       │                       ┌───────┐                                                              
    //                                                                                             ┌───┬───┬───┼───┬───┬───┼───┬───┼───┬───┬───┬───┬───┬───┼───┬───┤                                                              
    //                                                                               ~FirstSrcMask:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │                                                              
    //                                                                                             └───┴───┴───┼───┴───┴───┼───┴───┼───┴───┴───┼───┴───┴───┼───┴───┤                 ─                                            
    //                                                                                                         │           │       ├───┬───┬───┼───┬───┬───┼───┬───┤                                                              
    //                                                                                                         │           │  DEST:│ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │                                                              
    //                                                                                                         │           │       ├───┴───┴───┼───┴───┴───┼───┴───┤                                                              
    //                                                                                                         │           │       │                       └───────┘                                                              
    //                                                                                                         │           │       │                                                                                              
    //                                                                                                         │           │       │                                                                                              
    //                                                                                                         ◄────▲──────►       │                                                                                              
    //                                                                                                         │    │      │       │                                                                                              
    //                                                                                                              │    ◄─┴───────┼─────────►                                                                                    
    //                                                                                  2. restof BitAccum is appied│in FAST-LOOP  │  1,LEAD-IN:                                                                                  
    //                                                                                             ▲                │              │     Dest[DestIndex] = (uint8)((BitAccu & FirstSrcMask) | (Dest[DestIndex] & ~FirstSrcMask)); 
    //                                                                                             │                │                                      ───────                                                                
    //                                                                                             │                │                                       *** masking out!                                                      
    //                                                                                             │                └────────┐                                                                                                    
    //                                                                                             │                         │                                                                                                    
    //                                                                                             │                         │(***)                                                                                               
    //                                                                                             │                         │                                                                                                    
    //                                                                                             │                    ─────┼────────────────────────                                                                            
    //                                                                                        when we loop first index, we need to copy remaining bits                                                                            
    //                                                                                                                                                                                                                            
    //                                                                                                         │                                                                                                                  
    //                                                                                                         ◄─────────────────[right-shfit─13]──────────────────                                                               
    //                                                                         ┌───────────────────────────────┼                                                                                                                  
    //                             ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┼───┬───┬───┼───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐                                                              
    //                             │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 1 │ 0 │ 0 │                                                              
    //                             └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┼───┴───┴───┼───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘                                                              
    //                                                                         └───────────────────────────────┘                                                                                                                  
    //                                                                          every single byte is shifted by 'ShiftCount(13)'                                                                                                  
    //                                                                                                                                                                                                                            
    if (ShiftCount >= 0)
    {
        FullLoop    = FMath::Max(DestLoop, SrcLoop);
        BitAccu     = Src[SrcIndex] << ShiftCount;
        ShiftCount  += 8; // prepare for the inner loop
    }
    else
    {
        ShiftCount += 8; // turn shift -7..-1 into +1..+7
        FullLoop    = FMath::Max(DestLoop, SrcLoop - 1);
        BitAccu     = Src[SrcIndex] << ShiftCount;
        SrcIndex++;
        ShiftCount += 8; // prepare for inner loop
        BitAccu = (((uint32)Src[SrcIndex] << ShiftCount) + (BitAccu)) >> 8;
    }

    // lead-in - first copy
    Dest[DestIndex] = (uint8)((BitAccu & FirstSrcMask) | (Dest[DestIndex] & ~FirstSrcMask));
    SrcIndex++;
    DestIndex++;

    // fast inner loop
    for (; FullLoop > 1; FullLoop--)
    {
        // shift-count ranges from 8 to 15 - all reads are relevant
        // note that BitAccu is shifted by 8 every loop count
        BitAccu = (((uint32)Src[SrcIndex] << ShiftCount) + (BitAccu)) >> 8; // copy in the new, discard the old
        SrcIndex++;
        Dest[DestIndex] = (uint8)BitAccu; // copy low 8 bits
        DestIndex++;
    }

    // lead out 
    // LEAD-OUT is for your homework~ :)
    if (LastSrcMask != 0xFF)
    {
        if ((uint32)(SrcBit + BitCount - 1) / 8 == SrcIndex) // last legal byte?
        {
            BitAccu = (((uint32)Src[SrcIndex] << ShiftCount) + (BitAccu)) >> 8;
        }
        else
        {
            BitAccu = BitAccu >> 8;
        }
        Dest[DestIndex] = (uint8)((Dest[DestIndex] & LastSrcMask) | (BitAccu & ~LastSrcMask));
    }
}
```