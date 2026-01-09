---
layout: default
title: "04. Compute Shader"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 컴퓨트 셰이더란?

* GPU에서 범용 연산(이미지 처리, 물리 시뮬레이션 등)을 수행하는 셰이더
* 그래픽스 파이프라인과 독립적으로 실행

## 실행 계층 구조

* GPU는 수천 개의 스레드를 효율적으로 관리하기 위해 3단 계층 구조를 사용합니다.

| 계층 | 설명 | 결정 시점 |
|:---|:---|:---|
| Global | 전체 스레드 수 | 런타임 (CPU에서 dispatch) |
| Work Group | 스레드 묶음 단위 | 컴파일 타임 (셰이더에서 고정) |
| Thread (Invocation) | 개별 실행 단위 | - |

```cpp
// 셰이더 코드에서 이렇게 선언시, 현재 Work Group의 Thread(Invocation)을 16x16으로 해주세요!
layout (local_size_x = 16, local_size_y = 16) in;
```

### 왜 워크그룹이 필요한가?

* GPU 하드웨어(SM/CU) 단위로 실행되며, 워크그룹 안에서만:
    * 공유 메모리(Shared Memory) 사용 가능 (글로벌 메모리보다 10~20배 빠름)
    * barrier() 로 스레드 동기화 가능

### 왜 Global이 필요한가?

* 핵심: 셰이더는 한번 컴파일하면 코드 수정 없이 재사용해야 함

#### Example

* 게임에서 블러 필터를 적용하는 셰이더를 만들어보자

```cpp
layout (local_size_x = 16, local_size_y = 16) in;  // 고정됨
```

| 상황 | 이미지 크기 |
|:---|:---|
| 메인 화면 | 1920 × 1080 |
| 미니맵 | 256 × 256 |
| 인벤토리 아이콘 | 64 × 64 |
| 스크린샷 | 3840 × 2160 |

* 만약 Global이 없다면?
    * 셰이더를 크기마다 따로 만들어야 한다

```cpp
// blur_1920x1080.comp
layout (local_size_x = 1920, local_size_y = 1080) in;  // ❌ 이미 제한 초과

// blur_256x256.comp
layout (local_size_x = 256, local_size_y = 256) in;    // ❌ 제한 초과

// 애초에 local_size는 최대 1024개까지만 가능
```

```cpp
// C++ - 런타임에 크기만 바꿔서 호출

// 1920×1080 처리
vkCmdDispatch(cmd, 1920/16, 1080/16, 1);  // 120×68 워크그룹

// 256×256 처리
vkCmdDispatch(cmd, 256/16, 256/16, 1);    // 16×16 워크그룹

// 64×64 처리
vkCmdDispatch(cmd, 64/16, 64/16, 1);      // 4×4 워크그룹

// 3840×2160 처리
vkCmdDispatch(cmd, 3840/16, 2160/16, 1);  // 240×135 워크그룹
```

---

## 키워드 정리

### `layout`

* 변수나 설정의 속성을 지정하는 키워드
* 워크그룹 크기 선언 -> `layout (local_size_x = 16, local_size_y = 16) in;`

| 요소 | 설명 |
|:---|:---|
| local_size_x, local_size_y | 예약어 (임의 변경 불가), 숫자만 변경 가능 |
| in | GLSL 문법 규칙 (필수) |
| 제약 | x × y × z ≤ 1024 (대부분의 GPU) |

* 리소스 바인딩 -> `layout (set = 0, binding = 0, rgba8) uniform readonly image2D inputImage;`

| 요소 | 설명 |
|:---|:---|
| set, binding | CPU에서 바인딩할 슬롯 번호 |
| rgba8, rgba32f | 이미지 포맷 |
| uniform | CPU에서 바인딩하는 외부 리소스 |
| readonly / writeonly | 접근 권한 |

### 셰이더 내장 변수

```cpp
gl_GlobalInvocationID   // 전체에서 현재 스레드 위치 (0 ~ 전체크기-1)
gl_WorkGroupID          // 현재 워크그룹 번호
gl_LocalInvocationID    // 워크그룹 내에서 현재 스레드 위치 (0 ~ local_size-1)
```

---

## CPU to Shader

```
┌─────────────────────────────────────────────┐
│  C++ (Vulkan)                               │
│                                             │
│  // 리소스 바인딩                            │
│  vkCmdBindDescriptorSets(...)               │
│    └── set 0, binding 0 → 입력 이미지        │
│    └── set 0, binding 1 → 출력 이미지        │
│                                             │
│  // 실행 (워크그룹 개수 지정)                 │
│  vkCmdDispatch(width/16, height/16, 1)      │
└─────────────────┬───────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────┐
│  셰이더 (test.comp)                         │
│                                             │
│  layout(local_size_x=16, local_size_y=16)in;│
│  uniform ... inputImage;   ← binding 0      │
│  uniform ... outputImage;  ← binding 1      │
└─────────────────────────────────────────────┘
```

---

## Example

```cpp
void main() 
{
    ivec2 imageCoords = ivec2(gl_GlobalInvocationID.xy);
    
    ivec2 imageDims = imageSize(inputImage);
    
    if (imageCoords.x >= imageDims.x || imageCoords.y >= imageDims.y) {
        return;
    }
    
    vec4 color = imageLoad(inputImage, imageCoords);

    bool isRedSquare = ((gl_WorkGroupID.x + gl_WorkGroupID.y) % 2) == 0;
    
    if (isRedSquare) {
        color.rgb *= vec3(1.0, 0.5, 0.5);
    } else {
        color.rgb *= vec3(1.5);
    }

    imageStore(outputImage, imageCoords, color);
}
```

### 픽셀 좌표 구하기

```cpp
ivec2 imageCoords = ivec2(gl_GlobalInvocationID.xy);
```

| 변수 | 의미 | 값 범위 |
|:---|:---|:---|
| gl_WorkGroupID | 워크그룹 번호 | 0 ~ (워크그룹 개수 - 1) |
| gl_LocalInvocationID | 워크그룹 내 스레드 번호 | 0 ~ (local_size - 1) |
| gl_GlobalInvocationID | 전역 스레드 번호 (= 픽셀 좌표) | 0 ~ (전체 크기 - 1) |

> (참조) gl_GlobalInvocationID가 픽셀 좌표가 될 수 있는 이유 : 
> `gl_GlobalInvocationID = gl_WorkGroupID × local_size + gl_LocalInvocationID`

### 범위 체크

```cpp
ivec2 imageDims = imageSize(inputImage);

if (imageCoords.x >= imageDims.x || imageCoords.y >= imageDims.y) {
    return;
}
```

* 왜 필요한가?
    * 워크그룹은 항상 local_size의 배수로 실행되지만, 이미지 크기는 딱 맞아떨어지지 않을 수 있음.
    * 예: 500×500 이미지, local_size = 16×16필요한 워크그룹: 500 ÷ 16 = 31.25 → 올림해서 32개실제 스레드 수: 32 × 16 = 512스레드 500~511번은 이미지 밖 → 접근하면 에러!
    * 범위 밖 스레드는 아무것도 안 하고 return으로 종료.

### 픽셀 읽기

```cpp
vec4 color = imageLoad(inputImage, imageCoords);
```

| 함수 | 설명 |
|:---|:---|
| imageLoad(image, coord) | 이미지에서 특정 좌표의 픽셀값 읽기 |
| 반환값 | vec4 (RGBA, 0.0 ~ 1.0 범위) |

### 체스판 패턴 계산

```cpp
bool isRedSquare = ((gl_WorkGroupID.x + gl_WorkGroupID.y) % 2) == 0;
```

* 핵심: gl_GlobalInvocationID가 아닌 gl_WorkGroupID 사용
    * gl_GlobalInvocationID 사용 시 → 1×1 픽셀 단위 체스판
    * gl_WorkGroupID 사용 시 → 16×16 픽셀 단위 체스판

