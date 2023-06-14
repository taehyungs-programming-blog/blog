---
title: "(WebRTC) Example Client"
permalink: cpp/nwebrtc/client/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-02 00:00:00 -0000
last_modified_at: 2021-02-02 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - webrtc
category:
  - client
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 로그인 시도

```cpp
void MainWnd::OnDefaultAction() {
  if (!callback_)
    return;
  if (ui_ == CONNECT_TO_SERVER) {
    std::string server(GetWindowText(edit1_));
    std::string port_str(GetWindowText(edit2_));
    int port = port_str.length() ? atoi(port_str.c_str()) : 0;

    // 로그인
    callback_->StartLogin(server, port);
  } else if (ui_ == LIST_PEERS) {
    LRESULT sel = ::SendMessage(listbox_, LB_GETCURSEL, 0, 0);
    if (sel != LB_ERR) {
      LRESULT peer_id = ::SendMessage(listbox_, LB_GETITEMDATA, sel, 0);
      if (peer_id != -1 && callback_) {

        // 피어에 연결
        callback_->ConnectToPeer((int)peer_id);
      }
    }
  } else {
    MessageBoxA(wnd_, "OK!", "Yeah", MB_OK);
  }
}
```

### 로그인

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

### 피어에 연결

```cpp
void Conductor::ConnectToPeer(int peer_id) {
	printf("Conductor::ConnectToPeer\n");
  RTC_DCHECK(peer_id_ == -1);
  RTC_DCHECK(peer_id != -1);

  if (peer_connection_.get()) {
    main_wnd_->MessageBox(
        "Error", "We only support connecting to one peer at a time", true);
    return;
  }

  if (InitializePeerConnection()) {
    peer_id_ = peer_id;
    peer_connection_->CreateOffer(
        this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
  } else {
    main_wnd_->MessageBox("Error", "Failed to initialize PeerConnection", true);
  }
}
```

```cpp
bool Conductor::InitializePeerConnection() {
	printf("Conductor::InitializePeerConnection\n");

  RTC_DCHECK(!peer_connection_factory_);
  RTC_DCHECK(!peer_connection_);

  peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
	  nullptr /* network_thread */, nullptr /* worker_thread */,
	  nullptr /* signaling_thread */, nullptr /* default_adm */,
	  webrtc::CreateBuiltinAudioEncoderFactory(),
	  webrtc::CreateBuiltinAudioDecoderFactory(),
	  webrtc::CreateBuiltinExternalVideoEncoderFactory(),/*webrtc::CreateBuiltinVideoEncoderFactory(),*/
	  webrtc::CreateBuiltinVideoDecoderFactory(), /*ffmpeg::CreateBuiltinVideoDecoderFactory(),*/
	  nullptr /* audio_mixer */,
      nullptr /* audio_processing */);

  if (!peer_connection_factory_) {
    main_wnd_->MessageBox("Error", "Failed to initialize PeerConnectionFactory",
                          true);
    DeletePeerConnection();
    return false;
  }
```

```cpp
// Conductor Class에 관해서
class Conductor : public webrtc::PeerConnectionObserver,
                  public webrtc::CreateSessionDescriptionObserver,
                  public PeerConnectionClientObserver,
                  public MainWndCallback {

// ...
```