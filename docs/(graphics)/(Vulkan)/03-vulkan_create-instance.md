---
layout: default
title: "03. Create Instance"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## createInstance

```cpp
void createInstance(vector<const char*> requiredInstanceExtensions)
{
#ifdef NDEBUG
    bool useValidation = false;
#else
    bool useValidation = true;
#endif

    const uint32_t apiVersion = VK_API_VERSION_1_3;
    const string name = "Vulkan Examples";

    // 지원되는 인스턴스 확장 조회
    vector<string> supportedInstanceExtensions;
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    if (extCount > 0) {
        vector<VkExtensionProperties> extensions(extCount);
        if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) ==
            VK_SUCCESS) {
            for (VkExtensionProperties& extension : extensions) {
                supportedInstanceExtensions.push_back(extension.extensionName);
            }
        }
    }

    // 필수 확장 지원 여부 검증
    for (const char* requiredExtension : requiredInstanceExtensions) {
        if (find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(),
                 requiredExtension) == supportedInstanceExtensions.end()) {
            exitWithMessage("Required instance extension \"{}\" is not supported",
                            requiredExtension);
        }
    }

    // 애플리케이션 정보 설정
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = name.c_str();
    appInfo.pEngineName = name.c_str();
    appInfo.apiVersion = apiVersion;

    // 인스턴스 생성 정보 설정
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    if (portabilityAlreadyAdded) {
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    // 디버그 메신저 설정 (validation 활성화 시)
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{};
    if (useValidation) {
        debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsMessengerCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugUtilsMessengerCI.pfnUserCallback = debugUtilsMessageCallback;
        debugUtilsMessengerCI.pNext = instanceCreateInfo.pNext;
        instanceCreateInfo.pNext = &debugUtilsMessengerCI;

        const char* debugExtension = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        if (find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(),
                 debugExtension) != supportedInstanceExtensions.end()) {
            requiredInstanceExtensions.push_back(debugExtension);
        }
    }

    if (!requiredInstanceExtensions.empty()) {
        instanceCreateInfo.enabledExtensionCount = (uint32_t)requiredInstanceExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
    }

    // Validation 레이어 설정
    if (useValidation) {
        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());

        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        bool validationLayerPresent =
            find_if(instanceLayerProperties.begin(), instanceLayerProperties.end(),
                    [&](const VkLayerProperties& layer) {
                        return strcmp(layer.layerName, validationLayerName) == 0;
                    }) != instanceLayerProperties.end();

        if (validationLayerPresent) {
            instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
            instanceCreateInfo.enabledLayerCount = 1;
        } else {
            exitWithMessage("Validation layer VK_LAYER_KHRONOS_validation not present");
        }
    }

    check(vkCreateInstance(&instanceCreateInfo, nullptr, &instance_));

    // 디버그 유틸리티 함수 로드
    if (find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(),
             VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end()) {
        vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(
            vkGetInstanceProcAddr(instance_, "vkCmdBeginDebugUtilsLabelEXT"));
        vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(
            vkGetInstanceProcAddr(instance_, "vkCmdEndDebugUtilsLabelEXT"));
        vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(
            vkGetInstanceProcAddr(instance_, "vkCmdInsertDebugUtilsLabelEXT"));
    }

    // 디버그 메신저 생성
    if (useValidation) {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT messengerCI{};
        messengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        messengerCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        messengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        messengerCI.pfnUserCallback = debugUtilsMessageCallback;
        check(vkCreateDebugUtilsMessengerEXT(instance_, &messengerCI, nullptr,
                                             &debugUtilsMessenger));
    }
}
```

---

## selectPhysicalDevice

```cpp
void selectPhysicalDevice()
{
    // 1. 물리 디바이스 열거 및 선택
    uint32_t gpuCount = 0;
    check(vkEnumeratePhysicalDevices(instance_, &gpuCount, nullptr));
    if (gpuCount == 0) {
        exitWithMessage("gpuCount is 0");
    }

    vector<VkPhysicalDevice> physicalDevices(gpuCount);
    check(vkEnumeratePhysicalDevices(instance_, &gpuCount, physicalDevices.data()));

    uint32_t selectedDevice = 0;
    physicalDevice_ = physicalDevices[selectedDevice];

    // 2. 디바이스 속성/기능/메모리 속성 저장
    vkGetPhysicalDeviceProperties(physicalDevice_, &deviceProperties_);
    vkGetPhysicalDeviceFeatures(physicalDevice_, &deviceFeatures_);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &deviceMemoryProperties_);

    // 3. 큐 패밀리 속성 저장
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);
    queueFamilyProperties_.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount,
                                             queueFamilyProperties_.data());

    // 4. 지원 확장 목록 저장
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount, nullptr);
    if (extCount > 0) {
        vector<VkExtensionProperties> extensions(extCount);
        if (vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &extCount,
                                                 &extensions.front()) == VK_SUCCESS) {
            for (auto& ext : extensions) {
                supportedExtensions_.push_back(ext.extensionName);
            }
        }
    }
}
```

---

## createLogicalDevice

```cpp
void createLogicalDevice(bool useSwapChain)
{
    // 1. 요청할 큐 타입 설정 (Compute + Graphics)
    const VkQueueFlags requestedQueueTypes = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT;

    // 2. Vulkan 1.3 기능 활성화
    VkPhysicalDeviceVulkan13Features enabledFeatures13{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    enabledFeatures13.dynamicRendering = VK_TRUE;  // 렌더패스 없이 렌더링 가능
    enabledFeatures13.synchronization2 = VK_TRUE;  // 향상된 동기화 API

    vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    const float defaultQueuePriority(0.0f);

    // 3. 큐 패밀리 설정
    // 3-1. Graphics 큐: 렌더링 명령 처리
    if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
        queueFamilyIndices_.graphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamilyIndices_.graphics;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    } else {
        queueFamilyIndices_.graphics = 0;
    }

    // 3-2. Compute 큐: 연산 명령 처리 (Graphics와 다른 인덱스일 때만 별도 생성)
    if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
        queueFamilyIndices_.compute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
        if (queueFamilyIndices_.compute != queueFamilyIndices_.graphics) {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamilyIndices_.compute;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    } else {
        queueFamilyIndices_.compute = queueFamilyIndices_.graphics;
    }

    // 3-3. Transfer 큐: 데이터 전송용 (Graphics/Compute와 다를 때만 별도 생성)
    if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT) {
        queueFamilyIndices_.transfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
        if ((queueFamilyIndices_.transfer != queueFamilyIndices_.graphics) &&
            (queueFamilyIndices_.transfer != queueFamilyIndices_.compute)) {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamilyIndices_.transfer;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    } else {
        queueFamilyIndices_.transfer = queueFamilyIndices_.graphics;
    }

    // 4. 디바이스 확장 설정
    vector<const char*> deviceExtensions(enabledDeviceExtensions_);
    if (useSwapChain) {
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    // 5. 디바이스 기능 활성화
    enabledFeatures_.samplerAnisotropy = deviceFeatures_.samplerAnisotropy;
    enabledFeatures_.depthClamp = deviceFeatures_.depthClamp;
    enabledFeatures_.depthBiasClamp = deviceFeatures_.depthBiasClamp;

    // 6. 논리적 장치 생성 정보 구성
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    // Vulkan 1.3 기능을 pNext 체인으로 연결
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
    physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    physicalDeviceFeatures2.features = enabledFeatures_;
    physicalDeviceFeatures2.pNext = &enabledFeatures13;
    deviceCreateInfo.pEnabledFeatures = nullptr;
    deviceCreateInfo.pNext = &physicalDeviceFeatures2;

    // 확장 지원 여부 확인 및 설정
    if (deviceExtensions.size() > 0) {
        for (const char* enabledExtension : deviceExtensions) {
            if (!extensionSupported(enabledExtension)) {
                printLog("Enabled device extension \"{}\" is not present at device level",
                         enabledExtension);
            }
        }
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    }

    // 7. 논리적 장치 생성
    check(vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_));

    // 8. 커맨드 풀 생성 (큐 인덱스가 같으면 공유하여 메모리 절약)
    graphicsCommandPool_ = createCommandPool(queueFamilyIndices_.graphics);

    if (queueFamilyIndices_.compute != queueFamilyIndices_.graphics) {
        computeCommandPool_ = createCommandPool(queueFamilyIndices_.compute);
    } else {
        computeCommandPool_ = graphicsCommandPool_;
    }

    if (queueFamilyIndices_.transfer != queueFamilyIndices_.graphics &&
        queueFamilyIndices_.transfer != queueFamilyIndices_.compute) {
        transferCommandPool_ = createCommandPool(queueFamilyIndices_.transfer);
    } else if (queueFamilyIndices_.transfer == queueFamilyIndices_.compute) {
        transferCommandPool_ = computeCommandPool_;
    } else {
        transferCommandPool_ = graphicsCommandPool_;
    }
}
```

---

