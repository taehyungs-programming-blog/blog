---
title: "(Win32, MFC) Thread 사용"
permalink: cpp/win32/thread/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-24 00:00:00 -0000
last_modified_at: 2020-09-24 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - thread
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://tjstory.tistory.com/18)

---

* (주의)
MFC에서 GetDlgItem과 같은 Temporary 객체를 반환하는 함수는 사용하면 안된다.<br>
Thread가 동작하는 동안 Main Thread에서는 임시객체(GetDlgItem에서 반환된 객체)의 메모리는 모두 지우기 때문!<br>

---

## Thread가 필요한 Example

하나의 버튼을 누르면 아래와 같은 작업을 한다고 가정해보자.

```cpp
// Button Click
CString str;
int index;

for(int i = 0; i < 10000; i++)
{
    str.Format(L"item %05d", i);
    index = m_data_listbox.InsertString(-1, str);
    m_data_listbox.SetCurSel(index);
}
```

위 for문이 모두 동작하기 전 메인 다이얼로그를 움직이거나 추가 클릭이 불가능하다.<br>
Thread를 통하여 개선해 보자.<br>

```cpp
DWORD WINAPI InsertItemToList(void* ap_data)
{
    CString str;
    int index;

    for(int i = 0; i < 10000; i++)
    {
        str.Format(L"item %05d", i);
        index = ap_data->InsertString(-1, str);
        ap_data->SetCurSel(index);
    }

    return 0;
}

// ...

HANDLE h_thread;
h_thread = CreateThread(NULL, 0, InsertItemToList, &m_data_listbox, 0, NULL);

if(h_thread != NULL) {
    // Thread 생성
    
}
```

## CreateThread에 대한 설명

```cpp
HANDLE CreateThread(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    SIZE_T                  dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    __drv_aliasesMem LPVOID lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
);
```

* lpThreadAttributes : 사용자 계정 컨트롤, 관리자모드, 일반사용자 모드 등
* dwStackSize : 각 Thread의 Stack Size, 0으로 두면 1MB (보통 1MB임) 32bit 운영체제 기준으로 최대 사용가능 메모리가 4GB이고 2GB는 OS에서 사용하기에 2GB 중 1700MB 정도만 사용이 가능 따라서 StackSize를 1MB로 두면 Thread 1700개 정도사용이 가능
* lpStartAddress : ThreadFunc 주소
* lpParameter : Thread에 넘길 인자
* dwCreationFlags : Thread 생성 시 Flag 지정
* lpThreadId : 반환되는 Thread ID

## 정말 쉽네?? 뭐가 문제지???

(문제1) Main Thread가 종료된다고 내부 Thread가 모두 종료되는것이 아니다!<br>
For문이 모두 돌아가기 전 MainWindow를 죽이면?? -> Thread는 그대로 살아있는다.<br>

```cpp
// Dlg.h
HANDLE mh_thread = NULL;

mh_thread = CreateThread(NULL, 0, InsertItemToList, &m_data_listbox, 0, NULL);

if(mh_thread != NULL) {
    // Thread 생성
    
}
```

```cpp
// OnDestroy()
CDialogEx::OnDestroy();

if(mh_thread != NULL)
{
    TerminateThread(mh_thread, 0);
}
```

위 처럼 하면 될까??<br>
문제는 Thread 내부에서 동적 할당된 메모리를 회수하지 않고 비정상 종료를 해버리게 된다.<br>
이런 비정상 종료의 가장 큰 문제점은 Mutex영역을 설정하면 비정상 종료된 Mutex에 의해서 lock이 발생할 수 있다는 점이다.<br>
정말 급하게 일을 처리해야할때 `TerminateThread`를 쓰자...<br>

## 그럼 어떻게 하나?

EVENT를 이용해보자.

```cpp
// Dlg.h
struct ThreadData
{
    CListBox* p_list_box;
    HANDLE h_kill_event;
    HANDLE h_thread;
};

private:
    ThreadData m_thread_data;
```

```cpp
// OnInitDialog
m_thread_data.p_list_box = &m_data_list;
m_thread_data.h_kill_event = CreateEvent(NULL, 1, 0, L"Tips001");
m_thread_data.h_thread = NULL;
```

CreateEvent 설명

```cpp
HANDLE CreateEventA(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    BOOL                  bManualReset,
    BOOL                  bInitialState,
    LPCSTR                lpName
);
```

* lpEventAttributes : 사용자 계정 컨트롤, 관리자모드, 일반사용자 모드 등
* bManualReset : 1(ManualReset)로 두면 내가 SetEvent하면 Set 설정 / 0으로 두면 일정 시간이 지나면 CloseEvent
* bInitialState : 처음에 0으로 시작할지 1로 시작할지, 0으로 두면 0으로 시작
* lpName : 이벤트객체 이름, 이벤트 이름을 알면 OpenEvent()를 통해서 이벤트를 공유할 수 있다.

```cpp
m_thread_data.h_thread = CreateThread(NULL, 0, InsertItemToList, &m_thread_data, 0, NULL);

if(m_thread_data.h_thread != NULL) {
    // Thread 생성
    
}
```

```cpp
DWORD WINAPI InsertItemToList(void* ap_data)
{
    ThreadData * p_data = (ThreadData *)ap_data;

    CString str;
    int index;

    for(int i = 0; i < 10000; i++)
    {
        if(WaitForSingleObject(p_data->h_kill_event, 1) == WAIT_OBJECT_0)
        {
            // Event가 Set 상태이면 == WAIT_OBJECT_0
            // Event가 Set 상태가 아니면 == WAIT_TIMEOUT
            break;
        }
        str.Format(L"item %05d", i);
        index = p_data->p_list_box->InsertString(-1, str);
        p_data->p_list_box->SetCurSel(index);
    }

    CloseHandle(p_data->h_thread);
    p_data->h_thread = NULL;

    return 0;
}
```

여기서 꼭 Event를 써야만 할까? 그냥 flag를 하나 두면 더 편하지 않나?<br>
보통은 아래와 같이 event를 받으면 thread를 처리하는 구문을 넣기도 하기 때문에 범용성 문제로 Event를 많이 사용<br>

```cpp
while(TRUE) {
    ret = WaitForSingleObject(hHandle, INFINITE);

    if(ret == WAIT_FAILED)
        return 0;
    else if(ret == WAIT_ABANDONED) {
        ResetEvent(hHandle);
        continue;
    }
    else if(WAIT_TIMEOUT)
        continue;
    else
        ResetEvent(hHandle);
        // Do SomeThing
}
```

## 이렇게 하면 모든 문제는 해결이 될까?

```cpp
// OnDestroy()
CDialogEx::OnDestroy();

if(m_thread_data.h_thread != NULL)
{
    SetEvent(m_thread_data.h_kill_event);
    while(m_thread_data.h_thread != NULL);
}
```

이렇게 하면 또 다른 문제점이 발생<br>
`while(m_thread_data.h_thread != NULL);` 에서는 Thread가 죽기를 기다리고 있고<br>
`Thread의 index = p_data->p_list_box->InsertString(-1, str);`는 Main Thread에서 메시지 처리후 응답을 주기를 기다리면서 데드락 상태가 발생하게 된다.<br>

## 해결방안

```cpp
// OnDestroy()
CDialogEx::OnDestroy();

if(m_thread_data.h_thread != NULL)
{
    SetEvent(m_thread_data.h_kill_event);

    MSG msg;

    while(m_thread_data.h_thread != NULL && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessge(&msg);
        DispatchMessage(&msg);
    }
}
```

메시지를 처리하는 구문을 넣어준다.

하지만 이번 예제의 치명적 문제점이 있음.

`Thread에서 p_data->p_list_box->InsertString(-1, str);` 메인 Thread의 동작을 요구하게 되는데,<br>
이렇게 동작할 경우 Thread가 몇개 안되면 정상적으로 동작하나<br>
Thread의 개수가 많아 질수록 Main Thread의 부하가 커지게 된다.<br>

---

여기서 부터 MutilThread 관련

## 해결해 보자...

Thread를 여러개 만들어서 협력하며 쓰는 법에 대해 고민<br>
우선 소수를 구하는 함수를 하나 생성해본다.<br>

```cpp
int IsPrime(int a_num)
{
    int i;
    for(i = 2; i < a_num; i++)
    {
        if(a_num % i == 0) return 0;
    }
    return 1;
}
```

2~200000 사이의 소수를 모두 합산하는 Thread를 생성해 보자.

```cpp
DWORD WINAPI InsertItemToList(void * ap_data)
{
    ThreadData * p_data = (ThreadData *)ap_data;
    CString str;
    int index;
    __int64 sum = 0;

    int start_tick = GetTickCount();

    for(unsigned int i = 2; i < 200000; i++) {
        if(WaitForSingleObject(p_data->h_kill_event,0) == WAIT_OBJECT_0) break;

        if(IsPrime(i)) sum += i;

        if(!(i%100000)) {
            str.Format(L"작업 진행중 %d까지 체크함!");
            index = p_data->p_list_box->InsertString(-1, str);
            p_data->p_list_box_SetCurSel(index);
        }
    }

    str.Format(L"2~200000까지 소수의 합은 %lld이다. (%dms)", sum, GetTickCount() - start_tick);
    index = p_data->p_list_box->InsertString(-1, str);
    p_data->p_list_box_SetCurSel(index);

    CloseHandle(p_data->h_thread);
    p_data->h_thread = NULL;

    return 0;
}
```

## 버튼을 누를때마다 Thread를 만들게 변경해보자.

```cpp
// Dlg.h
struct ThreadData
{
    HWND h_wnd;         // 대화상자의 Handle
    unsigned int step;  // 
    CListBox * p_list_box;
    HANDLE h_kill_event;
    HANDLE h_thread;
    DWORD thread_id;
};
```

```cpp
// Dlg.cpp
private:
    unsigned int m_step = 200000;
```

```cpp
// Thread 생성 버튼
ThreadData * p = new ThreadData;
p->h_wnd = m_hWnd;
p->step = m_stop;
p->p_list_box = &m_data_list;
p->h_kill_event = CreateEvent(NULL, 1, 0, NULL);
p->h_thread = CreateThread(NULL, 0, InsertItemToList, p, 0, &p->thread_id);

CString str;
str.Format(L"%08x : %u 까지 합산", p->thread_id, p->step);
int index = m_thread_list.InsertString(-1, str);
m_thread_list.SetItemDataPtr(index, p);

m_step *= m_step;
```

```cpp
DWORD WINAPI InsertItemToList(void * ap_data)
{
    ThreadData * p_data = (ThreadData *)ap_data;
    CString str;
    int index;

    str.Format(L"[%08x] 작업을 시작합니다!", p_data->thread_id);
    p_data->p_list_box->SetCurSel(index);
    __int64 sum = 0, kill_flag = 0;
    int start_tick = GetTickCount();
    unsigned int i;

    for(i = 2; i < 200000; i++) {
        if(WaitForSingleObject(p_data->h_kill_event,0) == WAIT_OBJECT_0) {
            str.Format(L"[%08x] 작업 중단 체크함!", p_data->thread_id);
            index = p_data->p_list_box->InsertString(-1, str);
            kill_flag = 1;
            break;
        }

        if(IsPrime(i)) sum += i;

        if(!(i%100000)) {
            str.Format(L"[%08x] 작업 진행중 %d까지 체크함!", p_data->thread_id);
            index = p_data->p_list_box->InsertString(-1, str);
            p_data->p_list_box_SetCurSel(index);
        }
    }

    str.Format(L"[%08x] 2~%u까지 소수의 합은 %lld이다. (%dms)", p_data->thread_id, i, sum, GetTickCount() - start_tick);
    index = p_data->p_list_box->InsertString(-1, str);
    p_data->p_list_box_SetCurSel(index);

    str.Format(L"[%08x] 작업을 중단합니다!", p_data->thread_id);
    p_data->p_list_box->SetCurSel(index);

    CloseHandle(p_data->h_thread);
    
    if(WaitForSingleObject(p_data->h_kill_event, 0) == WAIT_OBJECT_0) kill_flag= 1;
    ::PostMessage(p_data->h_wnd, 27001, kill_flag, (LPARAM)p_data);

    return 0;
}
```

코드설명

(참고) LPARAM, WPARAM<br>
LPARAM : 정통적으로 4 bytes 주소값을 보통 넘겨준다.
WPARAM : 최근에는 4 bytes 가 되었으나 예전에 2 bytes로 보통 주소값을 넣지 않고 char, byte등을 넣었음.<br>
`::PostMessage(p_data->h_wnd, 27001, kill_flag, (LPARAM)p_data);` : 부모 다이얼로그(p_data->h_wnd)로 메시지(27001)를 보낸다. 메시지의 WPARAM = kill_flag(운용자가 죽인것인지 자연스럽게 죽은것인지), LPARAM = p_data의 주소값.<br>
`if(WaitForSingleObject(p_data->h_kill_event, 0) == WAIT_OBJECT_0) kill_flag= 1;` : 마지막에 이 한 줄이 더 들어가는 이유는?? -> 200000번까지 도는데 199999까지 도는 순간에 운용자가 Thread종료를 누르면 kill_flag가 0인채로 끝나버린다. 그 상황을 방지하기 위해서.

```cpp
// thread 선택 종료
int index = m_thread_list.GetCurSel();
if(LB_ERR != index)
{
    ThreadData* p = (ThreadData*)m_thread_list.GetItemDataPtr(index);
    if(p->h_thread != NULL) {
        SetEvent(p->h_kill_event);
        MSG msg;
        while(p->h_thread != NULL) {
            if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    delete p;
}
}
else {

}
```

코드설명

PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) : 메시지가 있으면 동작 없으면 그냥 넘어감, RM_REMOVE : 메시지를 복사하지 않고 꺼내옴<br>
기존 GetMessage()와의 차이점은 메시지가 있는지 없는지 확인한다는 점이다.<br>
GetMessage를 쓰면 메시지를 무한정 기다리며 데드락에 걸릴 수 있다.<br>
사용자 지정 메시지(27001) 추가<br>
클래스 마법사(Shift+Ctrl+x) -> 사용자 지정 메시지 추가 -> 27001<br>

```cpp
// On27001
ThreadData * p = (ThreadData *)lParam;
int count = m_thread_list.GetCount();
for(int i = 0; i < count; i++) {
    if(m_thread_list.GetItemDataPtr(i) == p) {
        m_thread_list.DeleteString(i);
        CloseHandle(p->h_kill_event);

        if(wParam == 0) delete p;
        else p->thread = NULL;

        break;
    }
}
```

```cpp
// OnDestroy()
OnBtnAllStop();
```

```cpp
// 모든 Thread 종료 버튼 (OnBtnAllStop())
ThreadData * p;
int count = m_thread_list.GetCount();
for(int i = 0; i < count; i++){
    p = (ThreadData*) m_thread_list.GetItemDataPtr(i);
    SetEvent(p->h_kill_event);
}

CString str;
str.Format(L"Thread %d개를 종료합니다.", count);
int index = m_data_list.InsertString(-1, str);
m_data_list.SetCurSel(index);

MSG msg;
while(count) {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if(msg.message == 27001) {
            count--;
            msg.wParam = 0;     // kill_flag를 1로 바꾸면 여기서 delete를 하지 않아도 된다.
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

index = m_data_list.InsertString(-1, L"모든 Thread가 종료되었습니다.!");
m_data_list.SetCurSel(index);
```

Thread가 특정 자원을 공유하며 쓰는 예시<br>
기존에 200000개씩 추가로 소수를 구하던 과정을<br>
1600000개의 소수의 합을 구하는데 32개의 Thread를 생성해서 구해보자.<br>
Thread의 구성은 32개의 Thread를 갖고<br>
하나의 Thread는 2, 34, 66 ....<br>
3, 35, 67 .... 이런식으로 32씩 더하면서 32까지 총 32개의 Thread를 생성하게 된다.<br>
그런데 Core가 32개가 아닌데 Thread를 32개나 만드는 이유는??<br>
2의 배수의 소수를 구하는 과정은 금방끝나기에 32개를 만든다 하더라도 절반이상은 금방끝나기때문!<br>

```cpp
// Dlg.h
struct ThreadData 
{
    HWND h_wnd;
    CListBox* p_list_box;
    HANDLE h_kill_event;
    HANDLE h_thread;
    DWORD thread_id;
    unsigned int start_step, end_step, thread_count;
    __int64* p_sum;
};

private:
    unsigned int m_step = 1600000;
    int m_thread_count = 32;
    __int64 m_sum = 0;              // 모든 Thread가 공유할 자원
    unsigned int m_start_tick;
```

시작버튼 클릭 시 32개의 Thread 생성

```cpp
if(m_thread_list.GetCount() > 0) return;

m_sum = 0;
m_start_tick = GetTickCount();

ThreadData * p;

for(int i = 0; i < m_thread_count; i++) {
    p = new ThreadData;
    p->h_wnd = m_hWnd;
    p->start_step = 2 + i;
    p->end_step = m_step;
    p->p_list_box = &m_data_list;
    p->thread_count = m_thread_count;
    p->p_sum = &m_sum;
    p->h_kill_event = CreateEvent(NULL, 1, 0, NULL);
    p->h_thread = CreateThread(NULL, 0, InsertItemToList, p, 0, &p->thread_id);

    CString str;
    str.Format(L"%08x : (%u ~ %u)까지 합산", p->thread_id, p->start_step, p->end_step);
    m_thread_list.InsertString(i, str);
    m_thread_list.SetItemData(i, p);
}
```

```cpp
DWORD WINAPI InsertItemToList(void* ap_data)
{
    ThreadData* p_data = (ThreadData*)ap_data;
    CString str;

    int start_tick = GetTickCount(), kill_flag = 0;
    int index;

    unsigned int i ;
    for(i = p_data->start_step; i <= p_data->end_step; i += p_data->thread_count) {
        if(WaitForSingleObject(p_data->h_kill_event, 0) == WAIT_OBJECT_0) {
            kill_flag = 1;
            break;
        }

        if(IsPrime(i)) *p_data->p_sum += i;
    }

    CloseHandle(p_data->h_)
}
```

```cpp
// On27001
ThreadData * p = (ThreadData *)lParam;
int count = m_thread_list.GetCount();
for(int i = 0; i < count; i++) {
    if(m_thread_list.GetItemDataPtr(i) == p) {
        m_thread_list.DeleteString(i);
        CloseHandle(p->h_kill_event);

        if(wParam == 0) delete p;
        else p->thread = NULL;

        break;
    }

if(count == 1) {
    CString str;
    str.Format(L"2~%u까지 소수의 합은 %lld이다. (%ums)", m_step, m_sum, GetTickCount() - m_start_tick);
    int index = m_data_list.InsertString(-1, str);
    m_data_list.SetCurSel(index);
}
}
```

그런데 문제는 동시에 m_sum을 쓰게 되는경우???

```cpp
// Dlg.cpp
CRITICAL_SETCTION gh_sum_lock;

// OnInitDlg
InitializeCriticalSection(&gh_sum_lock);

// OnDestroy
DeleteCriticalSection(&gh_sum_lock);
```

```cpp
DWORD WINAPI InsertItemToList(void* ap_data)
{
    // ...

    if(IsPrime(i)) {
        EnterCriticalSection(&gh_sum_lock);
        *p_data->p_sum += i;
        LeaveCriticalSection(&gh_sum_lock);
    }

    // ...
```

CriticalSection을 쓸 때 주의 사항은 TerminateThread와 같은 함수를 쓰면 Enter후 Leave를 하지 않는 문제가 발생하며 데드락에 걸릴 수 있다.<br>
여기서 중요한 점은 Critical Section을 거는 시점이다<br>
만약 아래와 같이 Critical Section을 걸었다고 가정해보자<br>

```cpp
EnterCriticalSection(&gh_sum_lock);
if(IsPrime(i)) *p_data->p_sum += i;
LeaveCriticalSection(&gh_sum_lock);
```

가장 부하가 큰 구간이 IsPrime을 구하는 구간인데 거기에 CriticalSection을???<Br>
정말 성능이 떨어지는 코드가 될 것이다.<br>