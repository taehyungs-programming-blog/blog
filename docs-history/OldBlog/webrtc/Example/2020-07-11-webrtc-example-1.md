---
title: "webrtc : Example - 1 : WebRTC 내부 Example 실행해보기"
permalink: webrtc/native-example-1/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-07-11 00:00:00 -0000
last_modified_at: 2020-07-11 00:00:00 -0000
sidebar:
  title: "webrtc 목차"
  nav: webrtc
tag:
  - webrtc
category:
  - example
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

## Test Ver.

    * WebRTC : 70
    * Visual Studio : 2017

> WebRTC 빌드에 대해서는 따로 설명하지 않겠음

---

## Example 빌드 및 실행 방법

WebRTC빌드 할때, .sln 까지 모두 빌드하면 example까지 빌드가 된다.

![](/file/image/WebRTC-Example-01.png)

사용할 프로젝트는 peerconnection_client, peerconnection_server를 사용하며<br>
빌드는 우클릭 후 각각을 별도로 빌드해야한다.<br>
또한 디버깅은 우클릭 후 각각을 별도로 디버깅 하거나 exe를 직접실행하면 된다.

우선, peerconnection_server.exe를 실행한다.

![](/file/image/WebRTC-Example-02.png)

다음으로 peerconnection_client.exe를 실행한다.

![](/file/image/WebRTC-Example-03.png)

Connect를 누른 후 Visual Studio의 디버깅 모드로 peerconnection_client를 실행한다.

![](/file/image/WebRTC-Example-04.png)

보이는 리스트 중 하나를 더블클릭한다.<br>
영상 전송이 시작된다.

> 참고로 `peerconnection_client.exe --port "4000"`과 같이 flag를 줄 수 있다.

---

## 분석

```cpp
// main.cc

int PASCAL wWinMain(HINSTANCE instance,
                    HINSTANCE prev_instance,
                    wchar_t* cmd_line,
                    int cmd_show) {
  rtc::EnsureWinsockInit();
  rtc::Win32SocketServer w32_ss;
  rtc::Win32Thread w32_thread(&w32_ss);
  rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

  rtc::WindowsCommandLineArguments win_args;
  int argc = win_args.argc();
  char** argv = win_args.argv();

    // Flag 처리
  rtc::FlagList::SetFlagsFromCommandLine(&argc, argv, true);
  if (FLAG_help) {
    rtc::FlagList::Print(NULL, false);
    return 0;
  }

  // Abort if the user specifies a port that is outside the allowed
  // range [1, 65535].
  if ((FLAG_port < 1) || (FLAG_port > 65535)) {
    printf("Error: %i is not a valid port.\n", FLAG_port);
    return -1;
  }

  MainWnd wnd(FLAG_server, FLAG_port, FLAG_autoconnect, FLAG_autocall);
  if (!wnd.Create()) {
    RTC_NOTREACHED();
    return -1;
  }

  rtc::InitializeSSL();

  // 핵심은 PeerConnectionClient, Conductor
  PeerConnectionClient client;
  rtc::scoped_refptr<Conductor> conductor(
      new rtc::RefCountedObject<Conductor>(&client, &wnd));

  // Main loop.
  MSG msg;
  BOOL gm;
  while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
    if (!wnd.PreTranslateMessage(&msg)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }

  if (conductor->connection_active() || client.is_connected()) {
    while ((conductor->connection_active() || client.is_connected()) &&
           (gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
      if (!wnd.PreTranslateMessage(&msg)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    }
  }

  rtc::CleanupSSL();
  return 0;
}
```