---
layout: default
title: "08. descriptor set"
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

## Descriptor Set이란?

* Descriptor Set은 Vulkan에서 셰이더가 GPU 리소스(이미지, 버퍼, 샘플러 등)에 접근할 수 있도록 연결해주는 테이블

```
┌─────────────────────────────────────────────────────────────┐
│  Descriptor Set = "리소스 연결 테이블"                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   Binding 0  ──────────►  입력 이미지 (텍스처)                │
│   Binding 1  ──────────►  출력 이미지                        │
│   Binding 2  ──────────►  Uniform Buffer (행렬, 상수 등)      │
│   Binding 3  ──────────►  샘플러                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Descriptor 관련 용어 정리

| 용어 | 설명 |
|:---|:---|
| Descriptor | 하나의 리소스를 가리키는 포인터/참조 |
| Descriptor Set | 여러 Descriptor를 묶은 그룹 |
| Descriptor Set Layout | Descriptor Set의 구조 정의 (바인딩 번호, 타입 등) |
| Descriptor Pool | Descriptor Set을 할당받는 메모리 풀 |

---

## Example

### 사전지식) VK_DESCRIPTOR_TYPE_STORAGE_IMAGE

#### 일반 이미지 vs Storage Image

| 특성 | Sampled Image (일반 텍스처) | Storage Image |
|:---|:---|:---|
| 읽기 | 샘플러를 통해 필터링된 값 | 픽셀 단위로 직접 읽기 |
| 쓰기 | ❌ 불가능 | ✅ 가능 |
| 접근 방식 | texture(sampler, uv) | imageLoad() / imageStore() |
| 용도 | 텍스처 매핑 | Compute 연산, 후처리 |

```cpp
// Sampled Image (일반 텍스처) - 읽기만 가능, 샘플러 필요
layout(set = 0, binding = 0) uniform sampler2D myTexture;
vec4 color = texture(myTexture, uv);  // 필터링된 샘플링

// Storage Image - 직접 읽기/쓰기 가능
layout(set = 0, binding = 0, rgba8) uniform readonly image2D inputImage;
layout(set = 0, binding = 1, rgba32f) uniform writeonly image2D outputImage;

vec4 pixel = imageLoad(inputImage, ivec2(x, y));   // 정확한 픽셀 좌표로 읽기
imageStore(outputImage, ivec2(x, y), newColor);    // 정확한 픽셀 좌표에 쓰기
```

* 시작

```cpp
vector<VkDescriptorSetLayoutBinding> bindings(2);

bindings[0].binding = 0;                                       // Matches shader binding = 0
bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // Storage Image는 셰이더에서 직접 읽기/쓰기가 가능한 이미지 타입
bindings[0].descriptorCount = 1;
bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;          // Used in compute stage
bindings[0].pImmutableSamplers = nullptr;                      // No samplers needed

bindings[1].binding = 1;                                       // Matches shader binding = 1
bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // Storage image type
bindings[1].descriptorCount = 1;                               // Single image
bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;          // Used in compute stage
bindings[1].pImmutableSamplers = nullptr;                      // No samplers needed

VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{
    VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
descriptorLayoutCI.bindingCount = static_cast<uint32_t>(bindings.size());
descriptorLayoutCI.pBindings = bindings.data();

VkDescriptorSetLayout descriptorSetLayout;
vkCreateDescriptorSetLayout(device, &descriptorLayoutCI, nullptr, &descriptorSetLayout);

// Create Pipeline Layout
VkPipelineLayoutCreateInfo pipelineLayoutCI{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
pipelineLayoutCI.setLayoutCount = 1;                 // We have one descriptor set (set = 0)
pipelineLayoutCI.pSetLayouts = &descriptorSetLayout; // Our image bindings
pipelineLayoutCI.pushConstantRangeCount = 0;         // No push constants in this example
pipelineLayoutCI.pPushConstantRanges = nullptr;

VkPipelineLayout pipelineLayout;
vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout);
```

## 추가 Compute Pipeline 생성까지 보자

```cpp
VkPipelineShaderStageCreateInfo shaderStageCI{
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
shaderStageCI.stage = VK_SHADER_STAGE_COMPUTE_BIT; // This is a compute shader
shaderStageCI.module = computeShaderModule;        // Our loaded shader module
shaderStageCI.pName = "main";                      // Entry point function name
shaderStageCI.pSpecializationInfo = nullptr;       // No shader specialization

VkComputePipelineCreateInfo pipelineCI{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
pipelineCI.layout = pipelineLayout;             // Resource layout
pipelineCI.stage = shaderStageCI;               // Single compute shader stage
pipelineCI.basePipelineHandle = VK_NULL_HANDLE; // No pipeline derivation
pipelineCI.basePipelineIndex = -1;

VkPipeline computePipelineHandle;
vkCreateComputePipelines(device, ctx.pipelineCache(), 1, &pipelineCI, nullptr, &computePipelineHandle);
```