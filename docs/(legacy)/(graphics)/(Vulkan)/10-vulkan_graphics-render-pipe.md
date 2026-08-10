---
layout: default
title: "10. graphics render pipe"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## vkCreateShaderModule

```cpp
VkShaderModule shaderModule;

VkShaderModuleCreateInfo shaderModuleCI{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
shaderModuleCI.codeSize = shaderCode.size();
shaderModuleCI.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule);
```

## 

```cpp
vector<char> vertShaderCode = readSpvFile(vertShaderFilename);
vector<char> fragShaderCode = readSpvFile(fragShaderFilename);

// createShaderModule를 통해 vkCreateShaderModule를 얻는다
VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

VkPipelineShaderStageCreateInfo vertShaderStageCI{
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
vertShaderStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT; // This is a vertex shader
vertShaderStageCI.module = vertShaderModule;          // Use our vertex shader module
vertShaderStageCI.pName = "main";                     // Entry point function name

VkPipelineShaderStageCreateInfo fragShaderStageCI{
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
fragShaderStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT; // This is a fragment shader
fragShaderStageCI.module = fragShaderModule;            // Use our fragment shader module
fragShaderStageCI.pName = "main";                       // Entry point function name

vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertShaderStageCI, fragShaderStageCI};

// 여기는 아직 사용하지 않기에 무시해도 좋음.
VkPipelineVertexInputStateCreateInfo vertexInputCI{
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
vertexInputCI.vertexBindingDescriptionCount = 0; // No vertex buffer bindings
vertexInputCI.pVertexBindingDescriptions = nullptr;
vertexInputCI.vertexAttributeDescriptionCount = 0; // No vertex attributes
vertexInputCI.pVertexAttributeDescriptions = nullptr;

VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
inputAssemblyCI.primitiveRestartEnable = VK_FALSE; // Don't enable primitive restart

// Viewport와 Scissor 선언
VkViewport viewport{};
viewport.x = 0.0f;                            // Start at left edge
viewport.y = 0.0f;                            // Start at top edge
viewport.width = static_cast<float>(width);   // Full width
viewport.height = static_cast<float>(height); // Full height
viewport.minDepth = 0.0f;                     // Near clipping plane
viewport.maxDepth = 1.0f;                     // Far clipping plane

VkRect2D scissor{};
scissor.offset = {0, 0};          // No offset
scissor.extent = {width, height}; // Full image size

VkPipelineViewportStateCreateInfo viewportStateCI{
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
viewportStateCI.viewportCount = 1;
viewportStateCI.pViewports = &viewport;
viewportStateCI.scissorCount = 1;
viewportStateCI.pScissors = &scissor;

VkPipelineRasterizationStateCreateInfo rasterizationCI{
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizationCI.depthClampEnable = VK_FALSE;                 // Don't clamp depth values
    rasterizationCI.rasterizerDiscardEnable = VK_FALSE;          // Don't discard primitives
    rasterizationCI.polygonMode = VK_POLYGON_MODE_FILL;          // Fill triangles (not wireframe)
    rasterizationCI.lineWidth = 1.0f;                            // Line width for wireframe mode
    rasterizationCI.cullMode = VK_CULL_MODE_NONE;                // Don't cull any faces
    rasterizationCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // CCW = front face
    rasterizationCI.depthBiasEnable = VK_FALSE;                  // No depth bias

VkPipelineMultisampleStateCreateInfo multisampleCI{
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
multisampleCI.sampleShadingEnable = VK_FALSE;               // No per-sample shading
multisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // 1 sample per pixel

VkPipelineDepthStencilStateCreateInfo depthStencilCI{
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
depthStencilCI.depthTestEnable = VK_FALSE;            // No depth testing
depthStencilCI.depthWriteEnable = VK_FALSE;           // Don't write to depth buffer
depthStencilCI.depthCompareOp = VK_COMPARE_OP_ALWAYS; // Always pass (not used)
depthStencilCI.depthBoundsTestEnable = VK_FALSE;      // No depth bounds testing
depthStencilCI.stencilTestEnable = VK_FALSE;          // No stencil testing

VkPipelineColorBlendAttachmentState colorBlendAttachment{};
// Enable writing to all color channels (RGBA)
colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
colorBlendAttachment.blendEnable = VK_FALSE; // No blending, just replace

VkPipelineColorBlendStateCreateInfo colorBlendCI{
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
colorBlendCI.logicOpEnable = VK_FALSE;   // No logical operations
colorBlendCI.logicOp = VK_LOGIC_OP_COPY; // Simple copy operation
colorBlendCI.attachmentCount = 1;        // One color attachment
colorBlendCI.pAttachments = &colorBlendAttachment;

VkPipelineLayoutCreateInfo pipelineLayoutCI{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
pipelineLayoutCI.setLayoutCount = 0; // No descriptor sets
pipelineLayoutCI.pSetLayouts = nullptr;
pipelineLayoutCI.pushConstantRangeCount = 0; // No push constants
pipelineLayoutCI.pPushConstantRanges = nullptr;

VkPipelineLayout pipelineLayout;
check(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

VkPipelineRenderingCreateInfo pipelineRenderingCI{
    VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
pipelineRenderingCI.colorAttachmentCount = 1;
VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
pipelineRenderingCI.pColorAttachmentFormats = &colorFormat;
pipelineRenderingCI.depthAttachmentFormat = VK_FORMAT_UNDEFINED;   // No depth buffer
pipelineRenderingCI.stencilAttachmentFormat = VK_FORMAT_UNDEFINED; // No stencil buffer

// 여기서 통합
VkGraphicsPipelineCreateInfo pipelineCI{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
pipelineCI.pNext = &pipelineRenderingCI; // Chain the dynamic rendering info
pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
pipelineCI.pStages = shaderStages.data();
pipelineCI.pVertexInputState = &vertexInputCI;
pipelineCI.pInputAssemblyState = &inputAssemblyCI;
pipelineCI.pViewportState = &viewportStateCI;
pipelineCI.pRasterizationState = &rasterizationCI;
pipelineCI.pMultisampleState = &multisampleCI;
pipelineCI.pDepthStencilState = &depthStencilCI;
pipelineCI.pColorBlendState = &colorBlendCI;
pipelineCI.pDynamicState = nullptr; // No dynamic state
pipelineCI.layout = pipelineLayout;
pipelineCI.renderPass = VK_NULL_HANDLE; // No render pass for dynamic rendering
pipelineCI.subpass = 0;
pipelineCI.basePipelineHandle = VK_NULL_HANDLE; // Not deriving from another pipeline
pipelineCI.basePipelineIndex = -1;

VkPipeline graphicsPipeline;
check(vkCreateGraphicsPipelines(device, ctx.pipelineCache(), 1, &pipelineCI, nullptr,
                                &graphicsPipeline));
```

---

## 초심자를 위해 조금 더 상세히

### Shader Stage Configuration (셰이더 스테이지 설정)

```cpp
VkPipelineShaderStageCreateInfo vertShaderStageCI{
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
vertShaderStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT; // This is a vertex shader
vertShaderStageCI.module = vertShaderModule;          // Use our vertex shader module
vertShaderStageCI.pName = "main";                     // Entry point function name

VkPipelineShaderStageCreateInfo fragShaderStageCI{
    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
fragShaderStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT; // This is a fragment shader
fragShaderStageCI.module = fragShaderModule;            // Use our fragment shader module
fragShaderStageCI.pName = "main";                       // Entry point function name
```

| 필드 | 설명 |
|:---|:---|
| stage | 셰이더가 어느 파이프라인 단계에서 실행될지 지정 |
| module | 사용할 VkShaderModule |
| pName | 셰이더 코드 내 진입점 함수 이름 (보통 "main") |
| pSpecializationInfo | 셰이더 상수 특수화 (여기선 미사용) |

---

### Vertex Input State (정점 입력 상태)

```cpp
VkPipelineVertexInputStateCreateInfo vertexInputCI{
    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
vertexInputCI.vertexBindingDescriptionCount = 0; // No vertex buffer bindings
vertexInputCI.pVertexBindingDescriptions = nullptr;
vertexInputCI.vertexAttributeDescriptionCount = 0; // No vertex attributes
vertexInputCI.pVertexAttributeDescriptions = nullptr;
```

```
일반적인 정점 버퍼 구조:

┌─────────────────────────────────────────────────────────────┐
│  Vertex 0        │  Vertex 1        │  Vertex 2        │...│
├──────┬──────┬────┼──────┬──────┬────┼──────┬──────┬────┤   │
│ pos  │ color│ uv │ pos  │ color│ uv │ pos  │ color│ uv │...│
│(vec3)│(vec3)│(v2)│(vec3)│(vec3)│(v2)│(vec3)│(vec3)│(v2)│   │
└──────┴──────┴────┴──────┴──────┴────┴──────┴──────┴────┘
       ↑
       stride (한 정점의 전체 크기)
```

```cpp
// Binding: 정점 버퍼 바인딩 설명
VkVertexInputBindingDescription binding{};
binding.binding = 0;                           // 바인딩 인덱스
binding.stride = sizeof(Vertex);               // 한 정점의 크기
binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // 정점마다 새 데이터

// Attribute: 각 속성의 위치와 형식
VkVertexInputAttributeDescription posAttr{};
posAttr.binding = 0;                    // 어느 바인딩에서 가져올지
posAttr.location = 0;                   // 셰이더의 layout(location=0)
posAttr.format = VK_FORMAT_R32G32B32_SFLOAT;  // vec3
posAttr.offset = offsetof(Vertex, pos); // Vertex 구조체 내 위치
```

---

### Input Assembly State (입력 어셈블리 상태)

```cpp
VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{
    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
inputAssemblyCI.primitiveRestartEnable = VK_FALSE; // Don't enable primitive restart
```

```
POINT_LIST:        각 정점이 독립적인 점
  V0  V1  V2  V3
   •   •   •   •

LINE_LIST:         2개 정점 = 1개 선분
  V0──V1  V2──V3
  
LINE_STRIP:        연결된 선분들
  V0──V1──V2──V3

TRIANGLE_LIST:     3개 정점 = 1개 삼각형 (가장 일반적)
     V0          V3
    /  \        /  \
   V1──V2      V4──V5

TRIANGLE_STRIP:    공유 정점으로 연결된 삼각형들
     V0──V2──V4
    / \ / \ /
   V1──V3──V5

TRIANGLE_FAN:      중심점을 공유하는 삼각형들
        V1
       /|
      / |
   V0───V2
      \ |
       \|
        V3
```

---

### Viewport & Scissor State (뷰포트 & 시저 상태)

```cpp
VkViewport viewport{};
viewport.x = 0.0f;                            // Start at left edge
viewport.y = 0.0f;                            // Start at top edge
viewport.width = static_cast<float>(width);   // Full width
viewport.height = static_cast<float>(height); // Full height
viewport.minDepth = 0.0f;                     // Near clipping plane
viewport.maxDepth = 1.0f;                     // Far clipping plane

VkRect2D scissor{};
scissor.offset = {0, 0};          // No offset
scissor.extent = {width, height}; // Full image size

VkPipelineViewportStateCreateInfo viewportStateCI{
    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
viewportStateCI.viewportCount = 1;
viewportStateCI.pViewports = &viewport;
viewportStateCI.scissorCount = 1;
viewportStateCI.pScissors = &scissor;
```

---

### Rasterization State (래스터화 상태)

```cpp
VkPipelineRasterizationStateCreateInfo rasterizationCI{
    VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
rasterizationCI.depthClampEnable = VK_FALSE;                 // Don't clamp depth values
rasterizationCI.rasterizerDiscardEnable = VK_FALSE;          // Don't discard primitives
rasterizationCI.polygonMode = VK_POLYGON_MODE_FILL;          // Fill triangles (not wireframe)
rasterizationCI.lineWidth = 1.0f;                            // Line width for wireframe mode
rasterizationCI.cullMode = VK_CULL_MODE_NONE;                // Don't cull any faces
rasterizationCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // CCW = front face
rasterizationCI.depthBiasEnable = VK_FALSE;                  // No depth bias
```

```
     정점들                    래스터화                프래그먼트들
   (Vertices)              (Rasterization)          (Fragments)
   
      V0                                              ■ ■ ■
     /  \                                           ■ ■ ■ ■ ■
    /    \          ───────────────→              ■ ■ ■ ■ ■ ■ ■
   /      \                                       ■ ■ ■ ■ ■ ■ ■ ■ ■
  V1──────V2                                    ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■
  
  삼각형의 3개 정점을         각 픽셀 위치에 대한
  화면 좌표로 투영            프래그먼트 생성
```

| 필드 | 값 | 설명 |
|:---|:---|:---|
| depthClampEnable | VK_FALSE | 깊이 클램핑 비활성화. VK_TRUE면 near/far 범위 밖 프래그먼트가 버려지지 않고 클램핑됨 |
| rasterizerDiscardEnable | VK_FALSE | VK_TRUE면 래스터화 단계 이후 모든 출력 버림 (쿼리만 필요할 때 사용) |
| polygonMode | FILL | 삼각형 내부를 채움. LINE=와이어프레임, POINT=정점만 |
| cullMode | NONE | 면 제거 없음. BACK=뒷면 제거, FRONT=앞면 제거 |
| frontFace | CCW | 반시계방향(Counter-Clockwise) 정점 순서가 앞면 |
| depthBiasEnable | VK_FALSE | 깊이 바이어스 비활성화 (그림자 맵에서 Z-fighting 방지용) |

---

### Multisample State (멀티샘플 상태)

```cpp
VkPipelineMultisampleStateCreateInfo multisampleCI{
    VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
multisampleCI.sampleShadingEnable = VK_FALSE;               // No per-sample shading
multisampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // 1 sample per pixel
```

```
일반 렌더링 (1 sample):          MSAA 4x (4 samples per pixel):

    ┌───┬───┬───┐                ┌───┬───┬───┐
    │   │   │ ▲ │                │   │ ◐ │ ● │
    ├───┼───┼───┤                ├───┼───┼───┤
    │   │ ▲ │ ▲ │                │   │ ● │ ● │
    ├───┼───┼───┤                ├───┼───┼───┤
    │ ▲ │ ▲ │ ▲ │                │ ◐ │ ● │ ● │
    └───┴───┴───┘                └───┴───┴───┘
    
    계단 현상 (Aliasing)          부드러운 경계
    
    ● = 4/4 샘플 커버             ◐ = 일부 샘플만 커버
```

---

### Depth Stencil State (깊이/스텐실 상태)

```cpp
VkPipelineDepthStencilStateCreateInfo depthStencilCI{
    VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
depthStencilCI.depthTestEnable = VK_FALSE;            // No depth testing
depthStencilCI.depthWriteEnable = VK_FALSE;           // Don't write to depth buffer
depthStencilCI.depthCompareOp = VK_COMPARE_OP_ALWAYS; // Always pass (not used)
depthStencilCI.depthBoundsTestEnable = VK_FALSE;      // No depth bounds testing
depthStencilCI.stencilTestEnable = VK_FALSE;          // No stencil testing
```

```
카메라에서 본 장면:

      ┌─────┐
      │ Box │ (z=0.5)
  ────┴─────┴────  Ground (z=1.0)

깊이 버퍼 없이:                깊이 버퍼 사용:
그리는 순서에 따라             가까운 물체가 항상
결과가 달라짐                  앞에 표시됨

  Ground 먼저    Box 먼저
  ┌─────────┐   ┌─────────┐    ┌─────────┐
  │░░░░░░░░░│   │░░░░░░░░░│    │░░░░░░░░░│
  │░░░░░░░░░│   │░░███░░░░│    │░░███░░░░│
  │░░███░░░░│   │░░███░░░░│    │░░███░░░░│ ← 항상 올바름
  └─────────┘   └─────────┘    └─────────┘
   Box가 안 보임  Box가 보임
```

---

### Color Blend State (컬러 블렌딩 상태)

```cpp
VkPipelineColorBlendAttachmentState colorBlendAttachment{};
// Enable writing to all color channels (RGBA)
colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
colorBlendAttachment.blendEnable = VK_FALSE; // No blending, just replace

VkPipelineColorBlendStateCreateInfo colorBlendCI{
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
colorBlendCI.logicOpEnable = VK_FALSE;   // No logical operations
colorBlendCI.logicOp = VK_LOGIC_OP_COPY; // Simple copy operation
colorBlendCI.attachmentCount = 1;        // One color attachment
colorBlendCI.pAttachments = &colorBlendAttachment;
```

```
새 프래그먼트 색상 (Source)     기존 프레임버퍼 색상 (Destination)
        │                              │
        ▼                              ▼
    ┌───────┐                      ┌───────┐
    │ Src   │                      │ Dst   │
    │ Color │                      │ Color │
    └───┬───┘                      └───┬───┘
        │                              │
        └────────────┬─────────────────┘
                     │
                     ▼
              ┌─────────────┐
              │   Blend     │
              │  Operation  │
              └──────┬──────┘
                     │
                     ▼
              최종 출력 색상
```

---

### Pipeline Layout (파이프라인 레이아웃)

```cpp
VkPipelineLayoutCreateInfo pipelineLayoutCI{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
pipelineLayoutCI.setLayoutCount = 0; // No descriptor sets
pipelineLayoutCI.pSetLayouts = nullptr;
pipelineLayoutCI.pushConstantRangeCount = 0; // No push constants
pipelineLayoutCI.pPushConstantRanges = nullptr;

VkPipelineLayout pipelineLayout;
check(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));
```

