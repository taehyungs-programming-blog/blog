---
layout: default
title: "18. Multi Thread Rendering"
parent: "(DirectX12 25년 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론) `_beginthreadex`

```cpp
uintptr_t _beginthreadex(
   void *security,                    // 보안 속성 (보통 NULL)
   unsigned stack_size,               // 스택 크기 (0 = 기본값)
   unsigned (__stdcall *start_address)(void *),  // 스레드 함수
   void *arglist,                     // 스레드 함수에 전달할 인자
   unsigned initflag,                 // 생성 플래그 (0 = 즉시 실행)
   unsigned *thrdaddr                 // 스레드 ID 반환 포인터
);
```

### CreateThread 대비 장점

```cpp
// CreateThread 사용 시 문제 발생 가능
CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
// 스레드 내에서 malloc, free, printf 등 CRT 함수 사용 시
// 메모리 누수나 크래시 발생 가능!

// _beginthreadex는 안전
_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
// CRT 함수들이 스레드별로 올바르게 초기화됨
```

#### 조금더 상세히

* 스레드 로컬 스토리지(TLS) 미초기화
    * C 런타임 라이브러리는 각 스레드마다 별도의 데이터 영역을 관리합니다:

```cpp
// CRT 내부에서 관리하는 스레드별 데이터 (간략화)
struct _tiddata {
    int       _terrno;              // errno 변수
    char*     _token;               // strtok 상태
    char*     _wctoken;             // wcstok 상태
    unsigned long  _randseed;       // rand() 시드값
    void*     _pxcptacttab;         // 예외 처리 테이블
    void*     _tpxcptinfoptrs;      // 예외 정보
    char      _errmsg[94];          // strerror 버퍼
    // ... 수십 개의 다른 필드들
};
```

* errno 변수 문제

```cpp
// CreateThread로 생성된 스레드
DWORD WINAPI BadThread(LPVOID param)
{
    FILE* fp = fopen("notexist.txt", "r");
    if (fp == NULL) {
        // errno는 스레드별로 관리되어야 하는데...
        int error = errno;  // ⚠️ 초기화되지 않은 메모리 읽기!
        printf("Error: %d\n", error);  // 쓰레기 값이나 크래시!
    }
    return 0;
}
```

* 문제점: CreateThread는 CRT의 TLS 데이터를 초기화하지 않았으므로, errno가 가리키는 메모리가 유효하지 않음

* strtok 같은 상태 유지 함수

```cpp
DWORD WINAPI BadThread(LPVOID param)
{
    char str[] = "Hello,World,Test";
    
    // strtok는 내부 상태를 스레드별로 저장함
    char* token = strtok(str, ",");  // ⚠️ TLS가 없으면 어디에 저장?
    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, ",");   // 크래시 가능!
    }
    return 0;
}
```

```cpp
DWORD WINAPI BadThread(LPVOID param)
{
    // CRT의 힙 관리자는 스레드별 힙 캐시를 유지
    char* buffer = (char*)malloc(1024);  // ⚠️ TLS 없이 할당
    
    strcpy(buffer, "Hello");
    printf("%s\n", buffer);
    
    free(buffer);  // ⚠️ 메모리 추적 정보가 손상됨
    // 스레드 종료 시 CRT 정리 코드가 실행되지 않음
    // -> 메모리 누수!
    return 0;
}
```

* `_beginthreadex`는?

```cpp
// _beginthreadex의 간략화된 내부 동작
uintptr_t _beginthreadex(...)
{
    // 1. 스레드별 CRT 데이터 구조 할당
    _ptiddata ptd = (_ptiddata)_calloc_crt(1, sizeof(struct _tiddata));
    if (ptd == NULL)
        return 0;
    
    // 2. TLS 슬롯에 데이터 저장
    TlsSetValue(_tlsindex, ptd);
    
    // 3. CRT 초기화
    ptd->_terrno = 0;           // errno 초기화
    ptd->_token = NULL;         // strtok 초기화
    ptd->_randseed = 1;         // rand 시드 초기화
    // ... 기타 초기화
    
    // 4. 실제 스레드 생성
    HANDLE hThread = CreateThread(...);
    
    return (uintptr_t)hThread;
}

void _endthreadex(unsigned retval)
{
    // 1. TLS에서 스레드 데이터 가져오기
    _ptiddata ptd = _getptd();
    
    // 2. 할당된 리소스 정리
    if (ptd->_token)
        free(ptd->_token);
    
    // 3. 스레드별 힙 캐시 정리
    _free_crt(ptd);
    
    // 4. TLS 클리어
    TlsSetValue(_tlsindex, NULL);
    
    // 5. 스레드 종료
    ExitThread(retval);
}
```

### Example

```cpp
// 스레드 함수 정의
unsigned int WINAPI MyThreadFunction(void* pArg)
{
    int* pData = (int*)pArg;
    printf("Thread ID: %d, Data: %d\n", GetCurrentThreadId(), *pData);
    
    // CRT 함수 안전하게 사용 가능
    char* buffer = (char*)malloc(100);
    // ...
    free(buffer);
    
    _endthreadex(0);  // 스레드 종료
    return 0;
}

// 메인에서 스레드 생성
int main()
{
    int data = 42;
    unsigned int threadId;
    
    HANDLE hThread = (HANDLE)_beginthreadex(
        NULL,              // 보안 속성 (기본값)
        0,                 // 스택 크기 (기본값)
        MyThreadFunction,  // 스레드 함수
        &data,             // 전달할 인자
        0,                 // 즉시 실행
        &threadId          // 스레드 ID 받을 변수
    );
    
    if (hThread == 0) {
        printf("스레드 생성 실패!\n");
        return 1;
    }
    
    // 스레드 종료 대기
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    
    return 0;
}
```

---

## 그냥 `std::thread`쓰면 안되나?

* _beginthreadex (C 런타임)
    * C 스타일 Windows 전용 스레드 생성 함수
    * Visual C++ 런타임 라이브러리 제공
    * 1990년대부터 사용되어 온 레거시 API
* std::thread (C++11 표준)
    * C++ 표준 라이브러리 스레드 클래스
    * C++11부터 도입 (2011년)
    * 플랫폼 독립적 (Windows, Linux, macOS 모두 지원)

### 언제 무엇을 사용해야 하나?

* _beginthreadex 사용해야 하는 경우 

- ✅ 레거시 코드 유지보수 (이미 사용 중) 
- ✅ Windows API와 깊은 통합 필요 
- ✅ C 프로젝트 (C++11 이전) 
- ✅ 매우 낮은 수준의 스레드 제어 필요

* std::thread 사용해야 하는 경우 

- ✅ 새 프로젝트 시작 
- ✅ 크로스 플랫폼 개발 
- ✅ 현대적인 C++ 코드 (C++11 이상) 
- ✅ 람다, 멤버 함수 사용 
- ✅ 타입 안전성과 RAII 중시## 성능 비교실제로 성능 차이는 거의 없음
    * std::thread도 내부적으로 Windows에서는 CreateThread나 유사한 API를 사용하기 때문

---

## 실제로 어떻게 썼는지 보자

```cpp
BOOL CD3D12Renderer::InitRenderThreadPool(DWORD dwThreadCount)
{
	m_pThreadDescList = new RENDER_THREAD_DESC[dwThreadCount];
	memset(m_pThreadDescList, 0, sizeof(RENDER_THREAD_DESC) * dwThreadCount);

	m_hCompleteEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	for (DWORD i = 0; i < dwThreadCount; i++)
	{
		for (DWORD j = 0; j < RENDER_THREAD_EVENT_TYPE_COUNT; j++)
		{
			m_pThreadDescList[i].hEventList[j] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			
		}
		m_pThreadDescList[i].pRenderer = this;
		m_pThreadDescList[i].dwThreadIndex = i;
		UINT uiThreadID = 0;
		m_pThreadDescList[i].hThread = (HANDLE)_beginthreadex(nullptr, 0, RenderThread, m_pThreadDescList + i, 0, &uiThreadID);
	}
	return TRUE;
}
```

```cpp
UINT WINAPI RenderThread(void* pArg)
{
	RENDER_THREAD_DESC* pDesc = (RENDER_THREAD_DESC*)pArg;
	CD3D12Renderer* pRenderer = pDesc->pRenderer;
	DWORD dwThreadIndex = pDesc->dwThreadIndex;
	const HANDLE* phEventList = pDesc->hEventList;
	while (1)
	{
		DWORD dwEventIndex = WaitForMultipleObjects(RENDER_THREAD_EVENT_TYPE_COUNT, phEventList, FALSE, INFINITE);

		switch (dwEventIndex)
		{
			case RENDER_THREAD_EVENT_TYPE_PROCESS:
				pRenderer->ProcessByThread(dwThreadIndex);
				break;
			case RENDER_THREAD_EVENT_TYPE_DESTROY:
				goto lb_exit;
			default:
				__debugbreak();
		}
	}
lb_exit:
	_endthreadex(0);
	return 0;
}
```

```cpp
void CD3D12Renderer::ProcessByThread(DWORD dwThreadIndex)
{
	CCommandListPool*	pCommandListPool = m_ppCommandListPool[m_dwCurContextIndex][dwThreadIndex];	// 현재 사용중인 command list pool

	// 렌더링큐를 처리하기 위해 RenderTaget설정
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(), m_uiRenderTargetIndex, m_rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDSVHeap->GetCPUDescriptorHandleForHeapStart());
	 
	// CommandList 1개당 400개씩 처리
	m_ppRenderQueue[dwThreadIndex]->Process(dwThreadIndex, pCommandListPool, m_pCommandQueue, 400, rtvHandle, dsvHandle, &m_Viewport, &m_ScissorRect);

	LONG lCurCount = _InterlockedDecrement(&m_lActiveThreadCount);
	if (0 == lCurCount)
	{
		SetEvent(m_hCompleteEvent);
	}
}
```

