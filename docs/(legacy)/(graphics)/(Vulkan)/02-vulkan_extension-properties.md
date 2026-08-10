---
layout: default
title: "02. Vulkan Extensions"
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

## Vulkan 인스턴스 확장 (Instance Extensions) 개념

* Vulkan을 기본 레고 세트라고 생각
    * 기본 세트: 핵심 기능만 포함 (GPU 연산, 메모리 관리 등)
    * 확장 (Extensions): 추가 레고 팩 (특별한 기능을 더해줌)

* 왜 확장이 필요한가?
    * Vulkan은 크로스 플랫폼 API. 하지만:
| 플랫폼 | 필요한 기능 |
|:---|:---|
| Windows | Windows 창에 그리기 |
| Linux | X11/Wayland 창에 그리기 |
| macOS | Metal 위에서 동작 (MoltenVK) |
| Android | Android Surface에 그리기 |

* 이런 플랫폼별 기능들은 모든 시스템에 필요한 게 아니므로 핵심 API에 포함시키지 않고, 필요할 때만 활성화하는 방식으로 설계

### 인스턴스 확장 vs 디바이스 확장

```
┌─────────────────────────────────────────────────┐
│              Vulkan Application                 │
├─────────────────────────────────────────────────┤
│  VkInstance (인스턴스 레벨)                      │
│  └─ 인스턴스 확장들                              │
│     • VK_KHR_surface (창 표면 지원)              │
│     • VK_KHR_win32_surface (Windows 창)         │
│     • VK_EXT_debug_utils (디버깅)               │
├─────────────────────────────────────────────────┤
│  VkDevice (디바이스 레벨)                        │
│  └─ 디바이스 확장들                              │
│     • VK_KHR_swapchain (화면 스왑체인)           │
│     • VK_KHR_ray_tracing (레이트레이싱)          │
└─────────────────────────────────────────────────┤
                      ↓
              GPU Hardware
```

* 인스턴스 확장: 시스템/플랫폼 레벨 기능 (창 시스템 연동, 디버깅 등)
* 디바이스 확장: GPU 하드웨어 기능 (스왑체인, 레이트레이싱 등)

### 자주 쓰이는 인스턴스 확장들

| 확장 이름 | 용도 |
|:---|:---|
| VK_KHR_surface | 화면에 그리기 위한 기본 표면 |
| VK_KHR_win32_surface | Windows 창 지원 |
| VK_KHR_xlib_surface | Linux X11 창 지원 |
| VK_EXT_debug_utils | 디버그 메시지, 라벨링 |
| VK_KHR_portability_enumeration | macOS MoltenVK 지원 |

---

## Example

```cpp
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

// =============================================
// 1단계: 함수 포인터 변수 선언
// =============================================
// 확장 함수는 컴파일 타임에 링크되지 않으므로
// 함수 포인터로 런타임에 가져와야 함
PFN_vkCreateDebugUtilsMessengerEXT  pfnCreateDebugMessenger = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugMessenger = nullptr;

// 디버그 메시지를 받을 콜백 함수
VKAPI_ATTR VkBool32 VKAPI_CALL myDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* userData)
{
    std::cout << "[Vulkan] " << data->pMessage << std::endl;
    return VK_FALSE;
}

int main()
{
    // =============================================
    // 2단계: 확장 지원 여부 확인
    // =============================================
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExts(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, availableExts.data());
    
    // VK_EXT_debug_utils 확장이 있는지 확인
    bool debugExtSupported = false;
    for (const auto& ext : availableExts) {
        if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            debugExtSupported = true;
            break;
        }
    }
    
    if (!debugExtSupported) {
        std::cout << "디버그 확장이 지원되지 않습니다!" << std::endl;
        return -1;
    }

    // =============================================
    // 3단계: 인스턴스 생성 시 확장 활성화
    // =============================================
    std::vector<const char*> extensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME  // "VK_EXT_debug_utils"
    };
    
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();  // 👈 여기서 활성화!
    
    VkInstance instance;
    vkCreateInstance(&createInfo, nullptr, &instance);

    // =============================================
    // 4단계: 함수 포인터 로드 (핵심!)
    // =============================================
    // vkGetInstanceProcAddr로 확장 함수의 주소를 가져옴
    pfnCreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    pfnDestroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (!pfnCreateDebugMessenger || !pfnDestroyDebugMessenger) {
        std::cout << "함수 포인터 로드 실패!" << std::endl;
        return -1;
    }

    // =============================================
    // 5단계: 확장 기능 실제 사용!
    // =============================================
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
    messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    messengerInfo.pfnUserCallback = myDebugCallback;
    
    VkDebugUtilsMessengerEXT debugMessenger;
    
    // 👇 함수 포인터를 일반 함수처럼 호출!
    pfnCreateDebugMessenger(instance, &messengerInfo, nullptr, &debugMessenger);
    
    std::cout << "디버그 메신저 생성 성공!" << std::endl;
    
    // ... 여기서 Vulkan 작업 수행 ...
    
    // =============================================
    // 6단계: 정리
    // =============================================
    pfnDestroyDebugMessenger(instance, debugMessenger, nullptr);
    vkDestroyInstance(instance, nullptr);
    
    return 0;
}
```

---

## Exmaple) VkDebugUtilsMessengerCreateInfoEXT

### 역할

* VkDebugUtilsMessengerCreateInfoEXT는 Vulkan Validation Layer에서 발생하는 디버그 메시지를 애플리케이션이 받아볼 수 있도록 설정하는 구조체

| 필드 | 설명 |
|:---|:---|
| sType | 구조체 타입 식별자 (Vulkan 표준) |
| messageSeverity | 받을 메시지의 심각도 레벨 필터 |
| messageType | 받을 메시지의 종류 필터 |
| pfnUserCallback | 디버그 메시지 수신 시 호출될 콜백 함수 |
| pNext | 체인 연결 (Instance 생성 시 함께 전달) |

* Message Severity (심각도)

| 플래그 | 의미 |
|:---|:---|
| VERBOSE_BIT | 상세 진단 정보 |
| INFO_BIT | 일반 정보 메시지 |
| WARNING_BIT | 경고 |
| ERROR_BIT | 에러 |

* Message Type (메시지 종류)

| 플래그 | 의미 |
|:---|:---|
| GENERAL_BIT | 일반적인 이벤트 |
| VALIDATION_BIT | Vulkan 스펙 위반 검증 |
| PERFORMANCE_BIT | 성능 관련 경고 |

```cpp
// 콜백 함수 정의
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
    std::cerr << "Validation: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;  // VK_TRUE면 해당 Vulkan 호출 중단
}
```

```cpp
// 메신저 생성 정보 설정
VkDebugUtilsMessengerCreateInfoEXT createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
createInfo.pfnUserCallback = debugCallback;
createInfo.pUserData = nullptr;  // 선택적 사용자 데이터
```

```cpp
// 메신저 생성 (Instance 생성 후)

// 확장 함수 로드
auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

VkDebugUtilsMessengerEXT messenger;
vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &messenger);
```