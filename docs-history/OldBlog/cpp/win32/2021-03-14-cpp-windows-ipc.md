---
title: "(Win32, MFC) Windows IPC"
permalink: cpp/windows/ipc/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-14 00:00:00 -0000
last_modified_at: 2021-03-14 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - windows
category:
  - ipc
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](http://dolphin.ivyro.net/file/windows_api/windows_ipc.html)

---

## Message 기반

* SendMessage, PostMessage로 통신

* SendMessage :
    * 윈도우 프로시저를 직접 호출하며, 프로시저가 메시지를 처리할 때 까지 반환하지 않는다.
    * 순차적으로 처리(sequentially)
    * 동기 방식(synchronous)

* PostMessage :
    * 메시지 큐에 메시지가 삽입되며, 윈도우 프로시저에서 메시지를 처리한다. 해당 메시지가 언제 처리될 지 예측이 어렵다.
    * 비 순차적으로 처리(not sequentially)
    * 비동기 방식(asynchronous) 

```cpp
void CExamMessageDlg::OnBnClickedButtonSendmessage()
{
	m_wndListBox.AddString(_T("Enter Send Message Button"));
	SendMessage(EVENT_SENDMESSAGE);
	m_wndListBox.AddString(_T("Leave Send Message Button"));
}
```

```cpp
void CExamMessageDlg::OnBnClickedButtonPostmessage()
{
	m_wndListBox.AddString(_T("Enter Post Message Button"));
	PostMessage(EVENT_POSTMESSAGE);
	m_wndListBox.AddString(_T("Leave Post Message Button"));
}
```

![](/file/image/cpp-windows-ipc-1.png)


* [참고사이트](https://luckygg.tistory.com/174)

---

## Shared Memory

* 파일 맵핑을 이용

---

더 자세한건 참고사이트 참조!