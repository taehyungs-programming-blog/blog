---
layout: default
title: "06. resource barrier"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Barrier란?

* Vulkan에서 Barrier(배리어)는 GPU 작업 간의 동기화와 이미지 레이아웃 전환을 담당하는 핵심 개념

## 왜 Barrier가 필요할까?

* GPU는 병렬 처리를 한다
    * GPU는 명령어들을 순서대로 실행하지 않음. 
    * 성능을 위해 병렬로, 때로는 순서를 바꿔서 실행
* 의존성이 있는 작업은 순서가 중요하다
* Barrier = "기다려!" 명령

## Barrier의 세 가지 종류

| 종류 | 용도 |
|:---|:---|
| Memory Barrier | 전역 메모리 동기화 |
| Buffer Memory Barrier | 특정 버퍼 동기화 |
| Image Memory Barrier | 이미지 동기화 + 레이아웃 전환 |

* 이미지 작업에서는 주로 Image Memory Barrier를 사용

## 이미지 레이아웃 종류

* UNDEFINED : 초기 상태 (내용 무효)
* GENERAL : 범용 (모든 작업 가능)
* TRANSFER_SRC_OPTIMAL : 복사 원본에 최적화
* TRANSFER_DST_OPTIMAL : 복사 대상에 최적화
* SHADER_READ_ONLY_OPTIMAL : 셰이더 읽기에 최적화
* COLOR_ATTACHMENT_OPTIMAL : 렌더 타겟에 최적화
* DEPTH_STENCIL_ATTACHMENT : 뎁스 버퍼에 최적화
* OPTIMAL_PRESENT_SRC_KHR : 화면 출력용

### 왜 레이아웃이 다양한가?

* GPU는 용도에 따라 메모리를 다르게 최적화

```
SHADER_READ_ONLY_OPTIMAL          COLOR_ATTACHMENT_OPTIMAL
┌───┬───┬───┬───┐                 ┌───────────────────┐
│ T │ T │ T │ T │  타일 기반       │                   │
├───┼───┼───┼───┤  캐시 최적화     │   선형 메모리     │
│ T │ T │ T │ T │                 │   빠른 쓰기       │
└───┴───┴───┴───┘                 └───────────────────┘
     텍스처 읽기용                     렌더 타겟용
```

## `VkImageMemoryBarrier2` 구조체

```cpp
VkImageMemoryBarrier2 barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};

// 동기화 설정 (언제 기다릴지)
barrier.srcStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT;     // 이전 단계
barrier.dstStageMask  = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT; // 다음 단계
barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;       // 이전 접근 방식
barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;          // 다음 접근 방식

// 레이아웃 전환 설정 (어떻게 바꿀지)
barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

// 대상 이미지
barrier.image = myImage;
barrier.subresourceRange = {
    VK_IMAGE_ASPECT_COLOR_BIT,  // 컬러/뎁스/스텐실
    0, 1,   // 밉맵 범위
    0, 1    // 레이어 범위
};
```

## 파이프라인 스테이지 (Pipeline Stage)

```
┌──────────────────────────────────────────────────────────┐
│                   GPU 파이프라인                         │
├──────────────────────────────────────────────────────────┤
│                                                          │
│  TOP_OF_PIPE                                             │
│       ↓                                                  │
│  DRAW_INDIRECT (간접 드로우 읽기)                         │
│       ↓                                                  │
│  VERTEX_INPUT (버텍스 버퍼 읽기)                          │
│       ↓                                                  │
│  VERTEX_SHADER                                           │
│       ↓                                                  │
│  TESSELLATION / GEOMETRY                                 │
│       ↓                                                  │
│  FRAGMENT_SHADER (텍스처 샘플링)                          │
│       ↓                                                  │
│  COLOR_ATTACHMENT_OUTPUT (렌더 타겟 쓰기)                 │
│       ↓                                                  │
│  BOTTOM_OF_PIPE                                          │
│                                                          │
│  ─────── 별도 경로 ───────                               │
│  TRANSFER (복사 작업)                                    │
│  COMPUTE_SHADER (컴퓨트 작업)                            │
│                                                          │
└──────────────────────────────────────────────────────────┘
```

## Access Flags (접근 방식)

| Access Flag | 의미 |
|:---|:---|
| TRANSFER_READ | 복사 원본으로 읽기 |
| TRANSFER_WRITE | 복사 대상으로 쓰기 |
| SHADER_READ | 셰이더에서 읽기 (텍스처) |
| SHADER_WRITE | 셰이더에서 쓰기 (스토리지) |
| COLOR_ATTACHMENT_READ | 렌더 타겟 읽기 |
| COLOR_ATTACHMENT_WRITE | 렌더 타겟 쓰기 |
| DEPTH_STENCIL_ATTACHMENT_READ | 뎁스 버퍼 읽기 |
| DEPTH_STENCIL_ATTACHMENT_WRITE | 뎁스 버퍼 쓰기 |

## Example

```cpp
// 1. 이미지 생성 직후 상태: UNDEFINED

// 2. 복사 받을 준비
barrier.srcStageMask  = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
barrier.dstStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
barrier.srcAccessMask = VK_ACCESS_2_NONE;
barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
barrier.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
barrier.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
vkCmdPipelineBarrier2(cmd, &depInfo);

// 3. 버퍼 → 이미지 복사
vkCmdCopyBufferToImage(...);

// 4. 셰이더에서 읽을 준비
barrier.srcStageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
barrier.dstStageMask  = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
vkCmdPipelineBarrier2(cmd, &depInfo);
```