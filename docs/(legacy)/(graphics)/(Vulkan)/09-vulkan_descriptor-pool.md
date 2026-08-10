---
layout: default
title: "09. descriptor pool and execute compute shader"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Vulkan Descriptor Pool

* Descriptor Pool은 Vulkan에서 Descriptor Set을 할당하기 위한 메모리 풀. 
* 쉽게 말해, 셰이더가 사용할 리소스(이미지, 버퍼, 샘플러 등)를 GPU에 바인딩하기 위해 필요한 "연결 정보"를 저장하는 공간.

* 왜 필요한가?
    * Vulkan은 리소스 관리를 애플리케이션에 맡기는 저수준 API. 
    * Descriptor Set을 할당하려면 먼저 Descriptor Pool을 생성해서 메모리를 미리 예약

```
┌─────────────────────────────────────────────────┐
│  Pipeline Layout (파이프라인 전체 리소스 구조)      │
│    └── Descriptor Set Layout (세트의 바인딩 정의)  │
│          └── Descriptor Set (실제 리소스 연결)     │
│                └── Descriptor Pool (메모리 관리)   │
└─────────────────────────────────────────────────┘
```

```cpp
VkDescriptorPoolSize poolSize{};
poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // Storage image descriptors
poolSize.descriptorCount = 2;                     // 2 storage images total

VkDescriptorPoolCreateInfo poolCI{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
poolCI.poolSizeCount = 1;      // One type of descriptor
poolCI.pPoolSizes = &poolSize; // Our storage image pool
poolCI.maxSets = 1;            // We'll allocate 1 descriptor set

VkDescriptorPool descriptorPool;
vkCreateDescriptorPool(device, &poolCI, nullptr, &descriptorPool);

VkDescriptorSetAllocateInfo allocInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
allocInfo.descriptorPool = descriptorPool;    // Pool to allocate from
allocInfo.descriptorSetCount = 1;             // Number of sets to allocate
allocInfo.pSetLayouts = &descriptorSetLayout; // Layout for the set

VkDescriptorSet descriptorSet;
vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
```

```cpp
// 리소스 연결

// 이미지 정보 설정
VkDescriptorImageInfo imageInfo{};
imageInfo.imageView = myImageView;
imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

// Write 구조체로 연결
VkWriteDescriptorSet write{};
write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
write.dstSet = descriptorSet;
write.dstBinding = 0;
write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
write.descriptorCount = 1;
write.pImageInfo = &imageInfo;

vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
```

```
┌──────────────────────────────────────────────────────────────┐
│                     Command Buffer 기록                       │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   ① Image Layout Transition (Barrier)                       │
│      └── UNDEFINED → GENERAL (Storage Image용)               │
│                          ↓                                   │
│   ② Pipeline 바인딩                                          │
│      └── vkCmdBindPipeline(COMPUTE, pipeline)               │
│                          ↓                                   │
│   ③ Descriptor Set 바인딩                                    │
│      └── vkCmdBindDescriptorSets(...)                       │
│                          ↓                                   │
│   ④ Dispatch (실행!)                                         │
│      └── vkCmdDispatch(groupX, groupY, groupZ)              │
│                          ↓                                   │
│   ⑤ 결과 동기화 (Barrier)                                    │
│      └── GENERAL → TRANSFER_SRC (결과 복사용)                 │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

```cpp
// 배리어 상세 설명

// ═══════════════════════════════════════════════════════════
// Pre-Dispatch: 이미지를 Compute Shader가 읽을 수 있게 준비
// ═══════════════════════════════════════════════════════════

VkImageMemoryBarrier2 inputBarrier{};
inputBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

// 언제 기다릴지 (Source)
inputBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
inputBarrier.srcAccessMask = 0;

// 언제 사용할지 (Destination)
inputBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
inputBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

// 레이아웃 전환
inputBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
inputBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;

// 큐 패밀리 전환 (같은 큐면 IGNORED)
inputBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
inputBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

// 대상 이미지
inputBarrier.image = inputImage;
inputBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
```

```cpp
// Dispatch 실행

// 파이프라인 바인딩
vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

// Descriptor Set 바인딩
vkCmdBindDescriptorSets(
    cmdBuffer,
    VK_PIPELINE_BIND_POINT_COMPUTE,  // Graphics가 아닌 Compute!
    pipelineLayout,
    0,                                // firstSet (set = 0)
    1,                                // setCount
    &descriptorSet,
    0, nullptr                        // dynamic offsets (사용 안 함)
);

// Work Group 수 계산 (local_size가 16x16인 경우)
uint32_t groupX = (width + 15) / 16;   // 올림 나눗셈
uint32_t groupY = (height + 15) / 16;

// 실행!
vkCmdDispatch(cmdBuffer, groupX, groupY, 1);
```

### 결과 복사 및 저장

```
GPU 이미지 (VRAM)  →  Staging Buffer  →  CPU 메모리  →  파일
     (복사)              (매핑)           (변환)
```

```
┌─────────────────────────────────────────────────────────────────┐
│                      결과 복사 프로세스                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐                                                │
│  │ Output      │  GPU 전용 메모리 (DEVICE_LOCAL)                 │
│  │ Image       │  - 빠르지만 CPU 접근 불가                        │
│  └──────┬──────┘                                                │
│         │                                                       │
│         │ vkCmdCopyImageToBuffer()                              │
│         ▼                                                       │
│  ┌─────────────┐                                                │
│  │ Staging     │  호스트 가시 메모리 (HOST_VISIBLE)               │
│  │ Buffer      │  - CPU 접근 가능                                │
│  └──────┬──────┘                                                │
│         │                                                       │
│         │ vkMapMemory()                                         │
│         ▼                                                       │
│  ┌─────────────┐                                                │
│  │ CPU 메모리   │  float[4] → uint8[4] 변환                      │
│  │ (매핑됨)     │  (RGBA32F → RGBA8)                             │
│  └──────┬──────┘                                                │
│         │                                                       │
│         │ stbi_write_jpg()                                      │
│         ▼                                                       │
│  ┌─────────────┐                                                │
│  │ output.jpg  │  최종 결과 파일                                 │
│  └─────────────┘                                                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

```cpp
// Staging Buffer 생성

// 버퍼 크기 계산 (RGBA32F = 픽셀당 16바이트)
VkDeviceSize bufferSize = width * height * 4 * sizeof(float);

// 버퍼 생성
VkBufferCreateInfo bufferInfo{};
bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
bufferInfo.size = bufferSize;
bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;  // 복사 대상
bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

// 메모리 할당 (CPU에서 접근 가능해야 함!)
VkMemoryAllocateInfo allocInfo{};
allocInfo.memoryTypeIndex = findMemoryType(
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |    // CPU 접근 가능
    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT     // 자동 동기화
);
```

```cpp
// 이미지 → 버퍼 복사

VkBufferImageCopy copyRegion{};
copyRegion.bufferOffset = 0;
copyRegion.bufferRowLength = 0;      // Tightly packed (빈틈없이)
copyRegion.bufferImageHeight = 0;    // Tightly packed

copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
copyRegion.imageSubresource.mipLevel = 0;
copyRegion.imageSubresource.baseArrayLayer = 0;
copyRegion.imageSubresource.layerCount = 1;

copyRegion.imageOffset = {0, 0, 0};
copyRegion.imageExtent = {width, height, 1};

vkCmdCopyImageToBuffer(
    cmdBuffer,
    outputImage,
    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,  // 반드시 이 레이아웃!
    stagingBuffer,
    1, &copyRegion
);
```

```cpp
// 데이터 변환 및 저장

// GPU → CPU 메모리 매핑
void* mappedData;
vkMapMemory(device, stagingMemory, 0, bufferSize, 0, &mappedData);

// float (0.0~1.0) → uint8 (0~255) 변환
float* floatData = static_cast<float*>(mappedData);
vector<uint8_t> pixels(width * height * 4);

for (uint32_t i = 0; i < width * height * 4; ++i) {
    float value = clamp(floatData[i], 0.0f, 1.0f);
    pixels[i] = static_cast<uint8_t>(value * 255.0f);
}

vkUnmapMemory(device, stagingMemory);

// JPEG 파일로 저장
stbi_write_jpg("output.jpg", width, height, 4, pixels.data(), 90);
```