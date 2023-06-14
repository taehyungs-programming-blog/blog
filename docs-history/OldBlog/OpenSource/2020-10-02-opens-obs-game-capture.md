---
title: "(OpenSource - obs) Game-capture : dll-injection"
permalink: opens/obs/game-capture/dll-injection/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-02 00:00:00 -0000
last_modified_at: 2020-10-02 00:00:00 -0000
sidebar:
  title: "OpenSource 목차"
  nav: OpenS
tag:
  - OpenSource
  - obs
category:
  - game-capture
  - dll-injection
  - OpenProcess
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

* [Get Code](https://github.com/Gbapy/NoxPlayer-Recorder-by-hooking-the-DirectX-OpenGL)

우선 Dll injection하는 부분

```cpp
static inline bool hook_direct(PNOXINFO pNox)
{
	HANDLE process;
	int ret;

    // 1
	process = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pNox->processID);
	//process = open_process(PROCESS_ALL_ACCESS, false, pNox->processID);
	if (!process) {
		int error = GetLastError();
		warn("hook_direct: could not open process: %s (%lu)");
		return false;
	}

    // 2
	ret = inject_library(process, hook_dll_path);
	CloseHandle(process);

	if (ret != 0) {
		warn("hook_direct: inject failed: %d");
		return false;
	}

	return true;
}
```

**1**

`OpenProcess` : [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess)

* [참고사이트](http://blog.naver.com/PostView.nhn?blogId=kgg1959&logNo=30188641999)

```cpp
HANDLE OpenProcess(
    DWORD dwDesiredAccess,
    BOOL  bInheritHandle,
    DWORD dwProcessId
);
```

Process ID를 알고있을때 Handle을 받아올때 사용된다.

* dwDesiredAccess : 접근유형, 보통 PROCESS_ALL_ACCESS 쓴다고 생각하자
* bInheritHandle : 상속여부, 
* dwProcessId : 내가 알고있는 Process ID

---

**2**

Kernel 모드에서 thread실행 메모리 쓰기 해제 등을 하기위한 과정이다.

```cpp
static inline int inject_library(HANDLE process, const wchar_t *dll)
{
	return inject_library_obf(process, dll, 
        "D|hkqkW`kl{k\\osofj", 0xa178ef3655e5ade7, 
        "[uawaRzbhh{tIdkj~~", 0x561478dbd824387c, 
        "[fr}pboIe`dlN}", 0x395bfbc9833590fd, 
        "\\`zs}gmOzhhBq", 0x12897dd89168789a, 
        "GbfkDaezbp~X", 0x76aff7238788f7db);
}
```

```cpp
int inject_library_obf(HANDLE process, const wchar_t *dll,
		       const char *create_remote_thread_obf, uint64_t obf1,
		       const char *write_process_memory_obf, uint64_t obf2,
		       const char *virtual_alloc_ex_obf, uint64_t obf3,
		       const char *virtual_free_ex_obf, uint64_t obf4,
		       const char *load_library_w_obf, uint64_t obf5)
{
	int ret = INJECT_ERROR_UNLIKELY_FAIL;
	DWORD last_error = 0;
	bool success = false;
	size_t written_size;
	DWORD thread_id;
	HANDLE thread = NULL;
	size_t size;
	void *mem;

	/* -------------------------------- */

    // 2-1
	HMODULE kernel32 = GetModuleHandleW(L"KERNEL32");
	create_remote_thread_t create_remote_thread;
	write_process_memory_t write_process_memory;
	virtual_alloc_ex_t virtual_alloc_ex;
	virtual_free_ex_t virtual_free_ex;
	FARPROC load_library_w;

    // 2-2
	create_remote_thread =
		get_obfuscated_func(kernel32, create_remote_thread_obf, obf1);  // 2-3
	write_process_memory =
		get_obfuscated_func(kernel32, write_process_memory_obf, obf2);  // 2-4
	virtual_alloc_ex =
		get_obfuscated_func(kernel32, virtual_alloc_ex_obf, obf3);  // 2-5
	virtual_free_ex =
		get_obfuscated_func(kernel32, virtual_free_ex_obf, obf4);  // 2-6
	load_library_w =
		get_obfuscated_func(kernel32, load_library_w_obf, obf5);  // 2-7

	/* -------------------------------- */

	size = (wcslen(dll) + 1) * sizeof(wchar_t);
	mem = virtual_alloc_ex(process, NULL, size, MEM_RESERVE | MEM_COMMIT,
			       PAGE_READWRITE);
	if (!mem) {
		goto fail;
	}

	success = write_process_memory(process, mem, dll, size, &written_size);
	if (!success) {
		goto fail;
	}

	thread = create_remote_thread(process, NULL, 0,
				      (LPTHREAD_START_ROUTINE)load_library_w,
				      mem, 0, &thread_id);
	if (!thread) {
		goto fail;
	}

	if (WaitForSingleObject(thread, 4000) == WAIT_OBJECT_0) {
		DWORD code;
		GetExitCodeThread(thread, &code);
		ret = (code != 0) ? 0 : INJECT_ERROR_INJECT_FAILED;

		SetLastError(0);
	}

fail:
	if (ret == INJECT_ERROR_UNLIKELY_FAIL) {
		last_error = GetLastError();
	}
	if (thread) {
		CloseHandle(thread);
	}
	if (mem) {
		virtual_free_ex(process, mem, 0, MEM_RELEASE);
	}
	if (last_error != 0) {
		SetLastError(last_error);
	}

	return ret;
}
```

**2-1**

`GetModuleHandleW` : [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlew)

* [참고사이트](http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=2)

```cpp
HMODULE GetModuleHandleW(
    LPCWSTR lpModuleName
);
```

명시된 모듈명칭(lpModuleName)에 해당하는 모듈이 해당 프로세스 주소공간에 연결(mapping)되어 있다면 해당 모듈의 핸들을 반환한다.

```cpp
// Example

// test.dll 모듈이 해당 프로세스에 연결된다.
HANDLE h_test_dll = LoadLibrary("Test.dll");

// h_get_dll은 h_test_dll과 동일한 값을 가지게 된다.
HANDLE h_get_dll = GetModuleHandle("test.dll"); 

// test.dll 모듈이 해당 프로세스와 연결이 해제된다.
// 이미 test.dll이 해제되었기 때문에 이 시점에서 사용하는 h_test_dll과 관련된 코드는
// 모두 오류처리된다.
FreeLibrary(h_get_dll);

// 이미 해제된 모듈을 해제하려고 하기 때문에 오류이다. 
FreeLibrary(h_test_dll); 
```

---

**2-2**

```cpp
void *get_obfuscated_func(HMODULE module, const char *str, uint64_t val)
{
	char new_name[128];
	strcpy(new_name, str);
	deobfuscate_str(new_name, val);

    // 2-2-1
	return GetProcAddress(module, new_name);
}
```

**2-2-1**

`GetProcAddress` : [MSDN](https://docs.microsoft.com/ko-kr/cpp/build/getprocaddress?view=vs-2019)

* [참고사이트](https://gatsbyv.tistory.com/174)

HMODULE은 모듈핸들로 DLL의 핸들값을 받아온다. 그다음 lpProcName은 모듈안에 찾고자하는 함수의 이름을 넣으면 주소값을 받아온다.

```cpp
// Example
#include
#include
#include

typedef int (*add_func)(int, int);
typedef int (*sub_func)(int, int);

int main()
{
	HINSTANCE hInst;

	add_func f_add_func;
	sub_func f_sub_func;

	hInst = LoadLibrary(_T("Gatsby.dll"));

	if (hInst == NULL)
	{
		printf("hInst NULL");
		return 1;
	}

	f_add_func = (add_func)GetProcAddress(hInst, "add_func");
	f_sub_func = (sub_func)GetProcAddress(hInst, "sub_func");

	int a = 150;
	int b = 80;
	int nRet = 0;

	nRet = f_add_func(a, b);
	printf("%d + %d = %d\n", a, b, nRet);

	nRet = f_sub_func(a, b);

	printf("%d - %d = %d\n", a, b, nRet);

	FreeLibrary(hInst);

	return 0;
}
```

```cpp
#include
#include

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int add_func(int a, int b);
EXPORT int sub_func(int a, int b);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

EXPORT int add_func(int a, int b)
{
	return a + b;
}

EXPORT int sub_func(int a, int b)
{
	return a - b;
}
```

---

**2-3**

`CreateRemoteThread` : [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread)

* [참고사이트](https://reversecore.com/40) : 설명이 아주 좋음 여길 보는 것을 추천.(injection의 핵심)

```cpp
HANDLE CreateRemoteThread(
    HANDLE                 hProcess,
    LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    SIZE_T                 dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID                 lpParameter,
    DWORD                  dwCreationFlags,
    LPDWORD                lpThreadId
);
```

---

**2-4**

`WriteProcessMemory` : [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)

```cpp
BOOL WriteProcessMemory(
    HANDLE  hProcess,
    LPVOID  lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T  nSize,
    SIZE_T  *lpNumberOfBytesWritten
);
```

* [참고사이트](https://playingr.tistory.com/entry/WriteProcessMemory%ED%95%B4%EB%8B%B9-%ED%94%84%EB%A1%9C%EC%84%B8%EC%8A%A4-%EB%A9%94%EB%AA%A8%EB%A6%AC-%EB%A5%BC-%EC%A1%B0%EC%9E%91)

해당 프로세스의 메모리 조작

---

**2-5**

`VirtualAlloc` : [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)

```cpp
LPVOID VirtualAlloc(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
);
```

* [참고사이트](https://diehard98.tistory.com/entry/%EC%9C%88%EB%8F%84%EC%9A%B0-%EB%A9%94%EB%AA%A8%EB%A6%AC-3-VirtualAlloc)

윈도우 메모리 할당
