---
layout: default
title: "10. SharedMemory"
parent: "(Windows System Programming)"
grand_parent: "(C++)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## SharedMemory

* Shared Memory 구현은 Memory-Mapped File과 Mutex를 사용하여 프로세스 간에 이미지 데이터를 공유

```cpp
m_hMemMap = CreateFileMapping(
    INVALID_HANDLE_VALUE,    // 페이징 파일 사용 (실제 파일 아님)
    NULL,                    // 보안 속성
    PAGE_READWRITE,          // 읽기/쓰기 권한
    0,                       // 크기 상위 32비트
    m_dwMaxMemSize,          // 크기 하위 32비트
    wchMemoryMappedFileName  // 공유 메모리 이름
);
```

| 매개변수 | 설명 |
|:---|:---|
| hFile | 매핑할 파일의 핸들. 공유 메모리만 사용할 경우 INVALID_HANDLE_VALUE |
| lpAttributes | 보안 속성. 보통 NULL 사용 |
| flProtect | 메모리 보호 속성 (PAGE_READONLY, PAGE_READWRITE 등) |
| dwMaximumSizeHigh/Low | 매핑할 최대 크기 (64비트 값을 두 개의 32비트로 분리) |
| lpName | 매핑 객체의 이름 (다른 프로세스에서 접근할 때 사용) |

* 주요 보호 속성 (flProtect)

| 값 | 설명 |
|:---|:---|
| PAGE_READONLY | 읽기 전용 |
| PAGE_READWRITE | 읽기/쓰기 가능 |
| PAGE_WRITECOPY | Copy-on-write 방식 |
| PAGE_EXECUTE_READWRITE | 실행/읽기/쓰기 가능 |


```cpp
m_hMutex = CreateMutex(
    nullptr,       // 보안 속성
    FALSE,         // 초기 소유 여부 (FALSE = 미소유)
    wchMutexName   // Mutex 이름 (프로세스 간 공유)
);
```

```cpp
m_pWriteBuffer = (char*)MapViewOfFile(
    m_hMemMap,       // 매핑 객체 핸들
    FILE_MAP_WRITE,  // 쓰기 권한
    0, 0,            // 오프셋 (시작 위치)
    m_dwMaxMemSize   // 매핑할 크기
);
```

```
┌─────────────────────────────────────────────────────────────┐
│                    물리 메모리 / 페이징 파일                  │
│         ┌────────────────────────────────────┐              │
│         │        Shared Memory               │              │
│         │        (m_dwMaxMemSize)            │              │
│         └────────────────────────────────────┘              │
│                     ▲              ▲                         │
│                     │              │                         │
│    MapViewOfFile()  │              │  MapViewOfFile()        │
│                     │              │                         │
│         ┌───────────┴──┐     ┌────┴────────────┐            │
│         │ m_pWriteBuffer│     │ m_pSharedMemory │            │
│         │ (Writer)      │     │ (Reader)        │            │
│         └──────────────┘     └─────────────────┘            │
│              Process A              Process B                │
└─────────────────────────────────────────────────────────────┘
```

```cpp
// 프로세스 B - 기존 공유 메모리 열기
HANDLE hMapFile = OpenFileMapping(
    FILE_MAP_ALL_ACCESS,
    FALSE,
    L"MySharedMemory"
);

LPVOID pBuf = MapViewOfFile(
    hMapFile,
    FILE_MAP_ALL_ACCESS,
    0, 0,
    256
);

// 데이터 읽기
printf("Received: %s\n", (char*)pBuf);
```

---

## Example) obs

```
┌─────────────────────────────┐        공유 메모리         ┌─────────────────────────────┐
│   게임 프로세스 (타겟)        │  ◄──────────────────────►  │   캡처 앱 (SDK 사용측)        │
│                             │                            │                             │
│    Capturer.dll (인젝션됨)   │    CreateFileMapping       │    CaptureSDK               │
│  - 화면 프레임 캡처          │    ──────────────────►     │  - 프레임 데이터 읽기         │
│  - 공유 메모리 생성          │    OpenFileMapping         │  - 공유 메모리 열기           │
└─────────────────────────────┘                            └─────────────────────────────┘
```

### 공유 메모리로 구조체 전달하기

```cpp
#pragma pack(push, 8)  // 메모리 정렬 강제 (중요!)

struct hook_info {
    // 캡처 정보
    uint32_t state;
    uint32_t type;
    uint32_t cx;           // 가로 해상도
    uint32_t cy;           // 세로 해상도
    uint32_t format;       // 픽셀 포맷
    uint32_t pitch;        // 한 줄의 바이트 수
    uint32_t map_id;
    uint32_t map_size;
    bool flip;
    
    // 추가 옵션
    uint64_t frame_interval;
    bool use_scale;
    bool force_shmem;
};

struct shmem_data {
    volatile int last_tex;     // 마지막으로 쓰인 버퍼 번호
    uint32_t tex1_offset;      // 텍스처 버퍼 1 오프셋
    uint32_t tex2_offset;      // 텍스처 버퍼 2 오프셋
};

#pragma pack(pop)
```

> 왜 #pragma pack이 필요한가?
> 32비트/64비트 프로세스 간 통신 시 구조체 패딩이 다를 수 있습니다.
> #pragma pack(8)으로 양쪽 메모리 레이아웃을 동일하게 강제합니다.

### 생성 측 코드 (Capturer.dll)

```cpp
// 1. 공유 메모리 생성
HANDLE hMap = CreateFileMappingW(
    INVALID_HANDLE_VALUE,           // 페이징 파일 사용 (실제 파일 없음)
    NULL,                           // 기본 보안
    PAGE_READWRITE,                 // 읽기/쓰기 가능
    0,                              // 크기 상위 32비트
    sizeof(struct hook_info),       // 크기 하위 32비트
    L"GameCapture_HookInfo12345"    // 이름 (PID 포함)
);

// 2. 가상 주소 공간에 매핑
struct hook_info* info = (struct hook_info*)MapViewOfFile(
    hMap,
    FILE_MAP_ALL_ACCESS,
    0, 0,
    sizeof(struct hook_info)
);

// 3. 구조체처럼 사용!
info->cx = 1920;
info->cy = 1080;
info->format = DXGI_FORMAT_B8G8R8A8_UNORM;
```

### 열기 측 코드 (CaptureSDK)

```cpp
// 1. 같은 이름으로 공유 메모리 열기
HANDLE hMap = OpenFileMappingW(
    FILE_MAP_READ | FILE_MAP_WRITE,
    FALSE,
    L"GameCapture_HookInfo12345"
);

// 2. 같은 구조체 타입으로 캐스팅
struct hook_info* info = (struct hook_info*)MapViewOfFile(
    hMap,
    FILE_MAP_ALL_ACCESS,
    0, 0,
    sizeof(struct hook_info)
);

// 3. 상대방이 쓴 값을 바로 읽기 가능!
printf("Resolution: %dx%d\n", info->cx, info->cy);  // 1920x1080
```

### 프로세스 간 동기화 (Named Mutex)

* Named Mutex란?
    * 일반 CRITICAL_SECTION은 같은 프로세스 내에서만 동작.
    * Named Mutex는 이름으로 식별되어 다른 프로세스에서도 접근 가능.

```cpp
// 프로세스 A: Mutex 생성
HANDLE mutex = CreateMutexW(NULL, FALSE, L"MyMutex12345");

// 프로세스 B: 같은 이름으로 열기
HANDLE mutex = OpenMutexW(SYNCHRONIZE, FALSE, L"MyMutex12345");

// 이제 두 프로세스가 같은 Mutex를 공유!
```

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          동기화 메커니즘                                 │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  [프로세스 내부 동기화]              [프로세스 간 동기화]                  │
│  CRITICAL_SECTION                   Named Mutex                         │
│  ┌─────────────────┐               ┌─────────────────┐                 │
│  │ data_mutex      │               │ tex_mutexes[0]  │ ◄── 텍스처 버퍼0 │
│  │ (스레드 동기화)  │               │ tex_mutexes[1]  │ ◄── 텍스처 버퍼1 │
│  └─────────────────┘               └─────────────────┘                 │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```
shmem_info (MapViewOfFile 반환값)
    │
    ▼
┌────────────────────────────────────────────────────────────────────────┐
│  shmem_data (헤더)   │     텍스처 버퍼 0      │     텍스처 버퍼 1      │
│  ┌────────────────┐  │  ┌─────────────────┐  │  ┌─────────────────┐  │
│  │ last_tex: 0    │  │  │                 │  │  │                 │  │
│  │ tex1_offset    │──┼─►│  픽셀 데이터     │  │  │  픽셀 데이터     │  │
│  │ tex2_offset    │──┼──┼─────────────────┼─►│  │                 │  │
│  └────────────────┘  │  │  (pitch * cy)   │  │  │  (pitch * cy)   │  │
│                      │  └─────────────────┘  │  └─────────────────┘  │
└────────────────────────────────────────────────────────────────────────┘
                                  ▲                       ▲
                                  │                       │
                    shmem_textures[0]         shmem_textures[1]
```

```cpp
// 빈 버퍼 찾아서 락 획득 (논블로킹)
static inline int try_lock_shmem_tex(int id)
{
    int next = id == 0 ? 1 : 0;
    DWORD wait_result;

    // 현재 버퍼 락 시도
    wait_result = WaitForSingleObject(tex_mutexes[id], 0);
    if (wait_result == WAIT_OBJECT_0) {
        return id;
    }

    // 다른 버퍼 락 시도
    wait_result = WaitForSingleObject(tex_mutexes[next], 0);
    if (wait_result == WAIT_OBJECT_0) {
        return next;
    }

    return -1;  // 둘 다 사용중 → 스킵
}

// 복사 스레드
void copy_thread()
{
    for (;;) {
        WaitForMultipleObjects(2, events, false, INFINITE);
        
        int lock_id = try_lock_shmem_tex(shmem_id);
        
        if (lock_id != -1) {
            // 공유 메모리에 프레임 데이터 복사
            memcpy(shmem_textures[lock_id], frame_data, pitch * cy);
            
            // 락 해제
            ReleaseMutex(tex_mutexes[lock_id]);
            
            // 최신 버퍼 번호 기록
            shmem_data->last_tex = lock_id;
            
            // 다음엔 다른 버퍼 사용
            shmem_id = lock_id == 0 ? 1 : 0;
        }
    }
}
```

```cpp
static void copy_shmem_tex(GameCapture* gc)
{
    HANDLE mutex = NULL;
    
    // 마지막으로 쓰여진 버퍼 확인
    int cur_texture = gc->shmem_data->last_tex;
    int next_texture = cur_texture == 1 ? 0 : 1;

    // 논블로킹으로 락 시도
    if (WaitForSingleObject(gc->texture_mutexes[cur_texture], 0) == WAIT_OBJECT_0) {
        mutex = gc->texture_mutexes[cur_texture];
    }
    else if (WaitForSingleObject(gc->texture_mutexes[next_texture], 0) == WAIT_OBJECT_0) {
        mutex = gc->texture_mutexes[next_texture];
        cur_texture = next_texture;
    }
    else {
        return;  // 둘 다 사용중 → 스킵
    }

    // 데이터 읽기
    // ... 텍스처 데이터 처리 ...

    // 락 해제
    ReleaseMutex(mutex);
}
```