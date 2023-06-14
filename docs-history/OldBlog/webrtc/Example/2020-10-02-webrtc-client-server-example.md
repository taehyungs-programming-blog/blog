---
title: "(WebRTC) Client Server Example"
permalink: opens/webrtc/cs-example/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-02 00:00:00 -0000
last_modified_at: 2020-10-02 00:00:00 -0000
tag:
  - OpenSource
  - WebRTC
category:
  - Example
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/OpenS-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/webrtc-example)

## Client

### Class UML

![](/file/image/cs-example-1.png)

### WebRTC 통신

**서버와 연결**

```cpp
void MainWnd::OnDefaultAction() {
  if (!callback_)
    return;
  if (ui_ == CONNECT_TO_SERVER) {
    std::string server(GetWindowText(edit1_));
    std::string port_str(GetWindowText(edit2_));
    int port = port_str.length() ? atoi(port_str.c_str()) : 0;
    callback_->StartLogin(server, port);
    // ...
```

```cpp
void Conductor::StartLogin(const std::string& server, int port) {
	printf("Conductor::StartLogin\n");
  if (client_->is_connected())
    return;
  server_ = server;
  client_->Connect(server, port, GetPeerName());
}
```

```cpp
void PeerConnectionClient::Connect(const std::string& server,
                                   int port,
                                   const std::string& client_name) {
  RTC_DCHECK(!server.empty());
  RTC_DCHECK(!client_name.empty());

  if (state_ != NOT_CONNECTED) {
    RTC_LOG(WARNING)
        << "The client must not be connected before you can call Connect()";
    callback_->OnServerConnectionFailure();
    return;
  }

  if (server.empty() || client_name.empty()) {
    callback_->OnServerConnectionFailure();
    return;
  }

  if (port <= 0)
    port = kDefaultServerPort;

  server_address_.SetIP(server);
  server_address_.SetPort(port);
  client_name_ = client_name;

  if (server_address_.IsUnresolvedIP()) {
    state_ = RESOLVING;
    resolver_ = new rtc::AsyncResolver();
    resolver_->SignalDone.connect(this, &PeerConnectionClient::OnResolveResult);
    resolver_->Start(server_address_);
  } else {
    DoConnect();
  }
}
```

**함수 호출 순서**

```s
# 생성
Conductor::Conductor

# 로그인
Conductor::StartLogin
Conductor::OnMessageSent
Conductor::OnSignedIn
Conductor::UIThreadCallback

# Peer 확인
Conductor::OnPeerConnected

# Peer 연결
Conductor::ConnectToPeer
Conductor::InitializePeerConnection
Conductor::CreatePeerConnection
Conductor::AddTracks
Conductor::OnSuccess
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::OnIceCandidate
Conductor::SendMessage
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageSent
Conductor::UIThreadCallback
Conductor::OnMessageFromPeer
Conductor::OnAddTrack
Conductor::OnAddTrack
Conductor::UIThreadCallback
Conductor::UIThreadCallback
Conductor::OnMessageFromPeer
Conductor::OnMessageFromPeer
Conductor::OnMessageFromPeer
```

---

## Server