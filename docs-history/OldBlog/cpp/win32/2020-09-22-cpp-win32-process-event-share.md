---
title: "(Win32, MFC) process간 Event 공유"
permalink: cpp/win32/process-event-share/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-09-22 00:00:00 -0000
last_modified_at: 2020-09-22 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - process-event-share
  - OpenEvent
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://jakejang.tistory.com/71)
* [참고사이트2](http://www.tipssoft.com/bulletin/board.php?bo_table=FAQ&wr_id=20)

---

* [Get Code](https://github.com/EasyCoding-7/win32-openevent-example)

---

Event 등록

```cpp
// set Event
SECURITY_DESCRIPTOR sd = { 0 };

::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
::SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
SECURITY_ATTRIBUTES sa = { 0 };
sa.nLength = sizeof(SECURITY_ATTRIBUTES);
sa.lpSecurityDescriptor = &sd;

HANDLE share_event_handle_ = CreateEvent(&sa, TRUE, FALSE, _T("My New Event"));
```

OpenEvent

```cpp
void CApplication1Dlg::OnBnClickedOk()
{
	opened_event_ = OpenEvent(EVENT_ALL_ACCESS, TRUE, _T("My New Event"));

	if (!opened_event_)
	{

		// __dv("OpenEvent failed");
		AfxMessageBox(_T("OpenEvent failed"));
	}
	else
	{
		//__dv("OpenEvent success: 0x%04x", opened_event_);
		AfxMessageBox(_T("OpenEvent Success"));
	}
}
```

SetEvent

```cpp
void CApplication1Dlg::OnBnClickedCancel()
{
	if (opened_event_ == nullptr) return;

	m_setResult = SetEvent(opened_event_);

	if (m_setResult)
	{
		AfxMessageBox(_T("Set Event success"));
		//__dv("OpenEvent success");
	}
	else
	{
		AfxMessageBox(_T("Set Event failed"));
		//__dv("OpenEvent failed");
	}
}
```

Check Set Event

```cpp
void CApplication1Dlg::OnBnClickedButton1()
{
	DWORD ret;
	//while (TRUE) {
		ret = WaitForSingleObject(opened_event_, INFINITE);

		if (ret == WAIT_FAILED) {
			AfxMessageBox(_T("WAIT_FAILED"));
		}
		else if (ret == WAIT_ABANDONED) {
			ResetEvent(opened_event_);
			AfxMessageBox(_T("WAIT_ABANDONED"));
		}
		else if (ret == WAIT_TIMEOUT) {
			AfxMessageBox(_T("WAIT_TIMEOUT"));
		}
		else {
			AfxMessageBox(_T("Event Signaled"));
			ResetEvent(opened_event_);
		}
	//}
}

```