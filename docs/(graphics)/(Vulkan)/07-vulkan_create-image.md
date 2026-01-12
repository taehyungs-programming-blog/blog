---
layout: default
title: "07. create image"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 전체 흐름 개요

```
┌──────────────────────────────────────────────────────────────┐
│                      전체 흐름                               │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   [CPU 메모리]                                               │
│       │                                                      │
│       ▼ memcpy                                               │
│   [스테이징 버퍼] ─── HOST_VISIBLE 메모리                     │
│       │                                                      │
│       ▼ vkCmdCopyBufferToImage                               │
│   [GPU 이미지] ─── DEVICE_LOCAL 메모리                        │
│       │                                                      │
│       ▼                                                      │
│   [셰이더에서 텍스처로 사용]                                  │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

```
┌─────────────────────────────────────────────────────────────┐
│                    전체 과정 요약                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. vkCreateImage()         - 이미지 핸들 생성              │
│  2. vkAllocateMemory()      - GPU 메모리 할당               │
│  3. vkBindImageMemory()     - 이미지와 메모리 연결          │
│  4. vkCreateImageView()     - 셰이더용 뷰 생성              │
│                                                             │
│  5. 스테이징 버퍼 생성       - CPU→GPU 전송용               │
│  6. memcpy()                - CPU 데이터 → 스테이징 버퍼    │
│                                                             │
│  7. Barrier                 - UNDEFINED → TRANSFER_DST      │
│  8. vkCmdCopyBufferToImage()- 스테이징 → GPU 이미지         │
│  9. Barrier                 - TRANSFER_DST → SHADER_READ    │
│                                                             │
│  10. vkQueueSubmit()        - GPU에 명령 제출               │
│  11. 스테이징 버퍼 정리      - 임시 리소스 해제             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 1) VkImage 생성

* 이미지의 핸들(Handle)만 생. 
* 아직 메모리는 없음.

```cpp
VkImageCreateInfo imageInfo{};
imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
imageInfo.imageType = VK_IMAGE_TYPE_2D;           // 2D 이미지
imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;       // RGBA 8bit sRGB
imageInfo.extent.width = width;
imageInfo.extent.height = height;
imageInfo.extent.depth = 1;
imageInfo.mipLevels = 1;                          // 밉맵 레벨 수
imageInfo.arrayLayers = 1;                        // 배열 레이어 수
imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;        // MSAA 없음
imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;       // GPU 최적화 타일링
imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT // 복사 대상
                | VK_IMAGE_USAGE_SAMPLED_BIT;     // 텍스처로 사용
imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

VkImage image;
vkCreateImage(device, &imageInfo, nullptr, &image);
```

## 2) 메모리 할당 및 바인딩

* 이미지에 실제 GPU 메모리를 할당하고 연결.

```cpp
// 1. 이미지가 필요로 하는 메모리 요구사항 조회
VkMemoryRequirements memRequirements;
vkGetImageMemoryRequirements(device, image, &memRequirements);

// 2. 메모리 할당 정보 설정
VkMemoryAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
allocInfo.allocationSize = memRequirements.size;
allocInfo.memoryTypeIndex = findMemoryType(
    memRequirements.memoryTypeBits,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT  // GPU 전용 메모리
);

// 3. 메모리 할당
VkDeviceMemory imageMemory;
vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);

// 4. 이미지와 메모리 바인딩
vkBindImageMemory(device, image, imageMemory, 0);
```

```
┌─────────────────────────────────────────────────────────┐
│                    GPU 메모리 종류                       │
├─────────────────────┬───────────────────────────────────┤
│ DEVICE_LOCAL        │ GPU 전용, 가장 빠름               │
│                     │ CPU 직접 접근 불가                │
├─────────────────────┼───────────────────────────────────┤
│ HOST_VISIBLE        │ CPU가 직접 읽기/쓰기 가능         │
│                     │ 스테이징 버퍼용                   │
├─────────────────────┼───────────────────────────────────┤
│ HOST_COHERENT       │ CPU 쓰기가 자동으로 GPU에 반영    │
│                     │ flush 불필요                      │
└─────────────────────┴───────────────────────────────────┘
```

## 3) VkImageView 생성

* 셰이더에서 이미지에 접근하기 위한 뷰(View)를 생성

```cpp
VkImageViewCreateInfo viewInfo{};
viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
viewInfo.image = image;
viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
viewInfo.subresourceRange.baseMipLevel = 0;
viewInfo.subresourceRange.levelCount = 1;
viewInfo.subresourceRange.baseArrayLayer = 0;
viewInfo.subresourceRange.layerCount = 1;

// Swizzle (채널 재매핑, 보통 기본값 사용)
viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

VkImageView imageView;
vkCreateImageView(device, &viewInfo, nullptr, &imageView);
```

```
┌─────────────────────────────────────────────────────────┐
│  VkImage         = 실제 메모리에 있는 픽셀 데이터        │
│  VkImageView     = 이미지를 "어떻게 볼지" 정의          │
│                                                         │
│  예시:                                                  │
│  • 큐브맵 이미지 → 한 면만 보는 2D 뷰                   │
│  • 밉맵 이미지 → 특정 레벨만 보는 뷰                    │
│  • RGBA 이미지 → R 채널만 보는 뷰                       │
└─────────────────────────────────────────────────────────┘
```

## 4) 스테이징 버퍼 생성

* CPU 데이터를 GPU로 전송하기 위한 중간 버퍼를 만든다

```cpp
VkDeviceSize imageSize = width * height * 4; // RGBA = 4 bytes

// 1. 버퍼 생성
VkBufferCreateInfo bufferInfo{};
bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
bufferInfo.size = imageSize;
bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;  // 복사 원본
bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

VkBuffer stagingBuffer;
vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer);

// 2. 메모리 할당 (HOST_VISIBLE)
VkMemoryRequirements memRequirements;
vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

VkMemoryAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
allocInfo.allocationSize = memRequirements.size;
allocInfo.memoryTypeIndex = findMemoryType(
    memRequirements.memoryTypeBits,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |   // CPU 접근 가능
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT    // 자동 동기화
);

VkDeviceMemory stagingBufferMemory;
vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory);
vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

// 3. CPU 데이터를 스테이징 버퍼에 복사
void* data;
vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
memcpy(data, pixels, static_cast<size_t>(imageSize));
vkUnmapMemory(device, stagingBufferMemory);
```

### 5) 스테이징 버퍼 용도

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│   DEVICE_LOCAL 메모리: GPU만 접근 가능 (빠름!)          │
│                        CPU는 직접 쓸 수 없음            │
│                                                         │
│   HOST_VISIBLE 메모리: CPU가 접근 가능 (느림)           │
│                        데이터 업로드용                  │
│                                                         │
│   따라서:                                               │
│   CPU → HOST_VISIBLE(스테이징) → DEVICE_LOCAL(최종)     │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

## 6) 커맨드 버퍼 생성 및 기록

* GPU 명령을 기록할 커맨드 버퍼를 준비

```cpp
VkCommandBufferAllocateInfo allocInfo{};
allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
allocInfo.commandPool = commandPool;
allocInfo.commandBufferCount = 1;

VkCommandBuffer commandBuffer;
vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

// 커맨드 기록 시작
VkCommandBufferBeginInfo beginInfo{};
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

vkBeginCommandBuffer(commandBuffer, &beginInfo);
```

## 7) 레이아웃 전환 #1 (UNDEFINED → TRANSFER_DST)

* 이미지를 복사 받을 수 있는 상태로 전환

```cpp
VkImageMemoryBarrier barrier{};
barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
barrier.image = image;
barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
barrier.subresourceRange.baseMipLevel = 0;
barrier.subresourceRange.levelCount = 1;
barrier.subresourceRange.baseArrayLayer = 0;
barrier.subresourceRange.layerCount = 1;
barrier.srcAccessMask = 0;
barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

vkCmdPipelineBarrier(
    commandBuffer,
    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,      // 이전 단계
    VK_PIPELINE_STAGE_TRANSFER_BIT,          // 다음 단계
    0,
    0, nullptr,                              // 메모리 배리어
    0, nullptr,                              // 버퍼 배리어
    1, &barrier                              // 이미지 배리어
);
```

## 8) 버퍼 → 이미지 복사

* 스테이징 버퍼의 데이터를 GPU 이미지로 복사

```cpp
VkBufferImageCopy region{};
region.bufferOffset = 0;
region.bufferRowLength = 0;    // 0 = tightly packed
region.bufferImageHeight = 0;  // 0 = tightly packed
region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
region.imageSubresource.mipLevel = 0;
region.imageSubresource.baseArrayLayer = 0;
region.imageSubresource.layerCount = 1;
region.imageOffset = {0, 0, 0};
region.imageExtent = {width, height, 1};

vkCmdCopyBufferToImage(
    commandBuffer,
    stagingBuffer,
    image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    1,
    &region
);
```

## 9) 레이아웃 전환 #2 (TRANSFER_DST → SHADER_READ_ONLY)

* 복사가 끝났으니, 셰이더에서 읽을 수 있는 상태로 전환

```cpp
barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

vkCmdPipelineBarrier(
    commandBuffer,
    VK_PIPELINE_STAGE_TRANSFER_BIT,          // 전송 작업 완료 후
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,   // 프래그먼트 셰이더 전에
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
);
```

## 10) 커맨드 제출 및 대기

* 기록된 커맨드를 GPU에 제출하고 완료를 기다림

```cpp
vkEndCommandBuffer(commandBuffer);

VkSubmitInfo submitInfo{};
submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
submitInfo.commandBufferCount = 1;
submitInfo.pCommandBuffers = &commandBuffer;

vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
vkQueueWaitIdle(graphicsQueue);  // 완료 대기

// 정리
vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
vkDestroyBuffer(device, stagingBuffer, nullptr);
vkFreeMemory(device, stagingBufferMemory, nullptr);
```