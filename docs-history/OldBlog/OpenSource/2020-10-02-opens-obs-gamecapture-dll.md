---
title: "(OpenSource - obs) game-capture : inject된 dll 동작"
permalink: opens/obs/game-capture/dll/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-02 00:00:00 -0000
last_modified_at: 2020-10-02 00:00:00 -0000
tag:
  - OpenSource
  - obs
category:
  - dll
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

```cpp
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID unused1)
{
	if (reason == DLL_PROCESS_ATTACH) {
		wchar_t name[MAX_PATH];

		dll_inst = hinst;

		HANDLE cur_thread;
		bool success = DuplicateHandle(GetCurrentProcess(),
					       GetCurrentThread(),
					       GetCurrentProcess(), &cur_thread,
					       SYNCHRONIZE, false, 0);

		if (!success)
			DbgOut("Failed to get current thread handle");

		if (!init_signals()) {
			return false;
		}
		if (!init_system_path()) {
			return false;
		}
		if (!init_hook_info()) {
			return false;
		}
		if (!init_mutexes()) {
			return false;
		}

		/* this prevents the library from being automatically unloaded
		 * by the next FreeLibrary call */
		//GetModuleFileNameW(hinst, name, MAX_PATH);
		//LoadLibraryW(name);
		capture_thread = CreateThread(
			NULL, 0, (LPTHREAD_START_ROUTINE)main_capture_thread,
			(LPVOID)cur_thread, 0, 0);
		if (!capture_thread) {
			CloseHandle(cur_thread);
			return false;
		}

	} else if (reason == DLL_PROCESS_DETACH) {
		if (capture_thread) {
			stop_loop = true;
			WaitForSingleObject(capture_thread, 300);
			CloseHandle(capture_thread);
		}

		free_hook();
	}

	(void)unused1;
	return true;
}
```

```cpp
static DWORD WINAPI main_capture_thread(HANDLE thread_handle)
{
	if (!init_hook(thread_handle)) {
		DbgOut("Failed to init hook\n");
		free_hook();
		return 0;
	}
	capture_loop();
	return 0;
}
```

```cpp
static inline void capture_loop(void)
{
	WaitForSingleObject(signal_init, INFINITE);
	attempt_hook();
}
```

```cpp
static inline bool attempt_hook(void)
{
	//static bool ddraw_hooked = false;
	static bool d3d8_hooked = false;
	static bool d3d9_hooked = false;
	static bool dxgi_hooked = false;
	static bool gl_hooked = false;

	if (!d3d9_hooked) {
	//	if (!d3d9_hookable()) {
	//		DbgOut("no D3D9 hook address found!\n");
	//		d3d9_hooked = true;
	//	} else {
			d3d9_hooked = hook_d3d9();
			if (d3d9_hooked) {
				return true;
			}
	//	}
	}

    // ...
```