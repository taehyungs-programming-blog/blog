---
title: "webrtc : Example - 2 : Client내부 분석"
permalink: webrtc/native-example-2/                # link 직접 지정
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
  - peerconnection client
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

우선 어디서 부터 봐야할지 모르겠으니... 그냥 처음 부터 보자!

```cpp
// main.cc

int PASCAL wWinMain(HINSTANCE instance,
                    HINSTANCE prev_instance,
                    wchar_t* cmd_line,
                    int cmd_show) {
  // Win32 Socket을 여는거 같은데... 어디서 쓰는지 까진 잘 모르겠음.
  rtc::EnsureWinsockInit();
  rtc::Win32SocketServer w32_ss;
  rtc::Win32Thread w32_thread(&w32_ss);
  rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);

  // Flag 설정에 관한 부분 생략.

  MainWnd wnd(FLAG_server, FLAG_port, FLAG_autoconnect, FLAG_autocall);
  if (!wnd.Create()) {
    RTC_NOTREACHED();
    return -1;
  }

  rtc::InitializeSSL();
  PeerConnectionClient client;
  rtc::scoped_refptr<Conductor> conductor(
      new rtc::RefCountedObject<Conductor>(&client, &wnd));

  // Main loop.
  MSG msg;
  BOOL gm;
  // 여기가 메인루프 ..
  while ((gm = ::GetMessage(&msg, NULL, 0, 0)) != 0 && gm != -1) {
    if (!wnd.PreTranslateMessage(&msg)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }
  }

  // 여긴 혹시나 통신은 연결되어 있는데 윈도우 메시지가 끊어졌을 경우를 대비 한듯?
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

로그 메시지로 분석해보자.

아래는 Server에 Connect까지 한 상황

```s
0xf0fc 스레드가 종료되었습니다(코드: 0 (0x0)).
(win32socketserver.cc:598): WSAAsync:connect (1 ms)
(peer_connection_client.cc:301): Headers received
(peer_connection_client.cc:477): OnClose
(conductor.cc:211): OnPeerConnected
(conductor.cc:197): OnSignedIn
(conductor.cc:469): SEND_MESSAGE_TO_PEER
(win32socketserver.cc:598): WSAAsync:connect (11 ms)
```

```cpp
// 우선 서버로 연결시도
void PeerConnectionClient::DoConnect() {
  control_socket_.reset(CreateClientSocket(server_address_.ipaddr().family()));
  hanging_get_.reset(CreateClientSocket(server_address_.ipaddr().family()));
  InitSocketSignals();
  char buffer[1024];
  sprintfn(buffer, sizeof(buffer), "GET /sign_in?%s HTTP/1.0\r\n\r\n",
           client_name_.c_str());
  onconnect_data_ = buffer;

  bool ret = ConnectControlSocket();
  if (ret)
    state_ = SIGNING_IN;
  if (!ret) {
    callback_->OnServerConnectionFailure();
  }
}
```

```cpp
bool PeerConnectionClient::ConnectControlSocket() {
  RTC_DCHECK(control_socket_->GetState() == rtc::Socket::CS_CLOSED);
  int err = control_socket_->Connect(server_address_);
  if (err == SOCKET_ERROR) {
    Close();
    return false;
  }
  return true;
}
```

뭐 이런식으로 서버와 연결하고 통신은 두 개의 소켓을 통해 한다.

```cpp
// peer_connection_client.h
  std::unique_ptr<rtc::AsyncSocket> control_socket_;
  std::unique_ptr<rtc::AsyncSocket> hanging_get_;
```

```cpp
// hanging_get_ 수신 부
void PeerConnectionClient::OnHangingGetRead(rtc::AsyncSocket* socket) {
  RTC_LOG(INFO) << __FUNCTION__;
  size_t content_length = 0;
  if (ReadIntoBuffer(socket, &notification_data_, &content_length)) {
    size_t peer_id = 0, eoh = 0;
    bool ok =
        ParseServerResponse(notification_data_, content_length, &peer_id, &eoh);

    if (ok) {
      // Store the position where the body begins.
      size_t pos = eoh + 4;

      if (my_id_ == static_cast<int>(peer_id)) {
        // A notification about a new member or a member that just
        // disconnected.
        int id = 0;
        std::string name;
        bool connected = false;
        if (ParseEntry(notification_data_.substr(pos), &name, &id,
                       &connected)) {
          if (connected) {
            peers_[id] = name;
            callback_->OnPeerConnected(id, name);
          } else {
            peers_.erase(id);
            callback_->OnPeerDisconnected(id);
          }
        }
      } else {
        OnMessageFromPeer(static_cast<int>(peer_id),
                          notification_data_.substr(pos));
      }
    }

    notification_data_.clear();
  }

  if (hanging_get_->GetState() == rtc::Socket::CS_CLOSED &&
      state_ == CONNECTED) {
    hanging_get_->Connect(server_address_);
  }
}
```

```cpp
// control_socket_ 수신 부
void PeerConnectionClient::OnRead(rtc::AsyncSocket* socket) {
  size_t content_length = 0;
  if (ReadIntoBuffer(socket, &control_data_, &content_length)) {
    size_t peer_id = 0, eoh = 0;
    bool ok =
        ParseServerResponse(control_data_, content_length, &peer_id, &eoh);
    if (ok) {
      if (my_id_ == -1) {
        // First response.  Let's store our server assigned ID.
        RTC_DCHECK(state_ == SIGNING_IN);
        my_id_ = static_cast<int>(peer_id);
        RTC_DCHECK(my_id_ != -1);

        // The body of the response will be a list of already connected peers.
        if (content_length) {
          size_t pos = eoh + 4;
          while (pos < control_data_.size()) {
            size_t eol = control_data_.find('\n', pos);
            if (eol == std::string::npos)
              break;
            int id = 0;
            std::string name;
            bool connected;
            if (ParseEntry(control_data_.substr(pos, eol - pos), &name, &id,
                           &connected) &&
                id != my_id_) {
              peers_[id] = name;
              callback_->OnPeerConnected(id, name);
            }
            pos = eol + 1;
          }
        }
        RTC_DCHECK(is_connected());
        callback_->OnSignedIn();
      } else if (state_ == SIGNING_OUT) {
        Close();
        callback_->OnDisconnected();
      } else if (state_ == SIGNING_OUT_WAITING) {
        SignOut();
      }
    }

    control_data_.clear();

    if (state_ == SIGNING_IN) {
      RTC_DCHECK(hanging_get_->GetState() == rtc::Socket::CS_CLOSED);
      state_ = CONNECTED;
      hanging_get_->Connect(server_address_);
    }
  }
}
```

```cpp
void PeerConnectionClient::OnMessageFromPeer(int peer_id,
                                             const std::string& message) {
  if (message.length() == (sizeof(kByeMessage) - 1) &&
      message.compare(kByeMessage) == 0) {
    callback_->OnPeerDisconnected(peer_id);
  } else {
    callback_->OnMessageFromPeer(peer_id, message);
  }
}
```

## 연결시도

```cpp
void Conductor::ConnectToPeer(int peer_id) {
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