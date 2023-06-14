---
title: "(WebRTC) Example Server"
permalink: cpp/nwebrtc/server/                # link 직접 지정
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
  - server
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Top

Server는 Peer관리와 연결만 하기에 WebRTC에 대한것은 아니다.

Main부터 시작함

* [전달아규먼트 처리, 필요하다면 볼 것](#1)
* [WinSock을 열고 지정한 port로 대기한다](#2)
* [peer and datasocket class](#3)

* [전체코드](#전체코드)

### 1

[Top](#Top)

전달아규먼트 처리, 필요하다면 볼 것

```cpp
int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      "Example usage: ./peerconnection_server --port=8888\n");
  absl::ParseCommandLine(argc, argv);

  // InitFieldTrialsFromString stores the char*, so the char array must outlive
  // the application.
  const std::string force_field_trials = absl::GetFlag(FLAGS_force_fieldtrials);
  webrtc::field_trial::InitFieldTrialsFromString(force_field_trials.c_str());

  int port = absl::GetFlag(FLAGS_port);

  // Abort if the user specifies a port that is outside the allowed
  // range [1, 65535].
  if ((port < 1) || (port > 65535)) {
    printf("Error: %i is not a valid port.\n", port);
    return -1;
  }
```

### 2

[Top](#Top)

WinSock을 열고 지정한 port로 대기한다

* [ListeningSocket class](#2-1)

```cpp
  ListeningSocket listener;
  if (!listener.Create()) {
    printf("Failed to create server socket\n");
    return -1;
  } else if (!listener.Listen(port)) {
    printf("Failed to listen on server socket\n");
    return -1;
  }
```

#### 2-1

ListeningSocket class

winsock wrapper라 생각하자

```cpp
class ListeningSocket : public SocketBase {
 public:
  ListeningSocket() {}

  bool Listen(unsigned short port);
  DataSocket* Accept() const;
};
```

```cpp
class SocketBase {
 public:
  SocketBase() : socket_(INVALID_SOCKET) {}
  explicit SocketBase(NativeSocket socket) : socket_(socket) {}
  ~SocketBase() { Close(); }

  NativeSocket socket() const { return socket_; }
  bool valid() const { return socket_ != INVALID_SOCKET; }

  bool Create();
  void Close();

 protected:
  NativeSocket socket_;
};
```

### 3

[Top](#Top)

* [PeerChannel class](#3-1)
* [DataSocket class](#3-2)

```cpp
PeerChannel clients;
  typedef std::vector<DataSocket*> SocketArray;
  SocketArray sockets;
  bool quit = false;
  while (!quit) {
    fd_set socket_set;
    FD_ZERO(&socket_set);
    if (listener.valid())
      FD_SET(listener.socket(), &socket_set);

    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
      FD_SET((*i)->socket(), &socket_set);

    struct timeval timeout = {10, 0};
    if (select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout) == SOCKET_ERROR) {
      printf("select failed\n");
      break;
    }

    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i) {
      DataSocket* s = *i;
      bool socket_done = true;
      if (FD_ISSET(s->socket(), &socket_set)) {
        if (s->OnDataAvailable(&socket_done) && s->request_received()) {
          ChannelMember* member = clients.Lookup(s);
          if (member || PeerChannel::IsPeerConnection(s)) {
            if (!member) {
              if (s->PathEquals("/sign_in")) {
                clients.AddMember(s);
              } else {
                printf("No member found for: %s\n", s->request_path().c_str());
                s->Send("500 Error", true, "text/plain", "",
                        "Peer most likely gone.");
              }
            } else if (member->is_wait_request(s)) {
              // no need to do anything.
              socket_done = false;
            } else {
              ChannelMember* target = clients.IsTargetedRequest(s);
              if (target) {
                member->ForwardRequestToPeer(s, target);
              } else if (s->PathEquals("/sign_out")) {
                s->Send("200 OK", true, "text/plain", "", "");
              } else {
                printf("Couldn't find target for request: %s\n",
                       s->request_path().c_str());
                s->Send("500 Error", true, "text/plain", "",
                        "Peer most likely gone.");
              }
            }
          } else {
            HandleBrowserRequest(s, &quit);
            if (quit) {
              printf("Quitting...\n");
              FD_CLR(listener.socket(), &socket_set);
              listener.Close();
              clients.CloseAll();
            }
          }
        }
      } else {
        socket_done = false;
      }

      if (socket_done) {
        printf("Disconnecting socket\n");
        clients.OnClosing(s);
        assert(s->valid());  // Close must not have been called yet.
        FD_CLR(s->socket(), &socket_set);
        delete (*i);
        i = sockets.erase(i);
        if (i == sockets.end())
          break;
      }
    }

    clients.CheckForTimeout();

    if (FD_ISSET(listener.socket(), &socket_set)) {
      DataSocket* s = listener.Accept();
      if (sockets.size() >= kMaxConnections) {
        delete s;  // sorry, that's all we can take.
        printf("Connection limit reached\n");
      } else {
        sockets.push_back(s);
        printf("New connection...\n");
      }
    }
  }

  for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
    delete (*i);
  sockets.clear();
```

#### 3-1

* [Top](#3)

PeerChannel class

현재 연결된 peer에 대한 관리를 해준다.

```cpp
class PeerChannel {
 public:
  typedef std::vector<ChannelMember*> Members;

  PeerChannel() {}

  ~PeerChannel() { DeleteAll(); }

  const Members& members() const { return members_; }

  // Returns true if the request should be treated as a new ChannelMember
  // request.  Otherwise the request is not peerconnection related.
  static bool IsPeerConnection(const DataSocket* ds);

  // Finds a connected peer that's associated with the |ds| socket.
  ChannelMember* Lookup(DataSocket* ds) const;

  // Checks if the request has a "peer_id" parameter and if so, looks up the
  // peer for which the request is targeted at.
  ChannelMember* IsTargetedRequest(const DataSocket* ds) const;

  // Adds a new ChannelMember instance to the list of connected peers and
  // associates it with the socket.
  bool AddMember(DataSocket* ds);

  // Closes all connections and sends a "shutting down" message to all
  // connected peers.
  void CloseAll();

  // Called when a socket was determined to be closing by the peer (or if the
  // connection went dead).
  void OnClosing(DataSocket* ds);

  void CheckForTimeout();

 protected:
  void DeleteAll();
  void BroadcastChangedState(const ChannelMember& member,
                             Members* delivery_failures);
  void HandleDeliveryFailures(Members* failures);

  // Builds a simple list of "name,id\n" entries for each member.
  std::string BuildResponseForNewMember(const ChannelMember& member,
                                        std::string* content_type);

 protected:
  Members members_;
};
```

#### 3-2

* [Top](#3)

SocketBase은 winsock wrapper이다.

```cpp
class DataSocket : public SocketBase {
 public:
  enum RequestMethod {
    INVALID,
    GET,
    POST,
    OPTIONS,
  };

  explicit DataSocket(NativeSocket socket)
      : SocketBase(socket), method_(INVALID), content_length_(0) {}

  ~DataSocket() {}

  static const char kCrossOriginAllowHeaders[];

  bool headers_received() const { return method_ != INVALID; }

  RequestMethod method() const { return method_; }

  const std::string& request_path() const { return request_path_; }
  std::string request_arguments() const;

  const std::string& data() const { return data_; }

  const std::string& content_type() const { return content_type_; }

  size_t content_length() const { return content_length_; }

  bool request_received() const {
    return headers_received() && (method_ != POST || data_received());
  }

  bool data_received() const {
    return method_ != POST || data_.length() >= content_length_;
  }

  // Checks if the request path (minus arguments) matches a given path.
  bool PathEquals(const char* path) const;

  // Called when we have received some data from clients.
  // Returns false if an error occurred.
  bool OnDataAvailable(bool* close_socket);

  // Send a raw buffer of bytes.
  bool Send(const std::string& data) const;

  // Send an HTTP response.  The |status| should start with a valid HTTP
  // response code, followed by a string.  E.g. "200 OK".
  // If |connection_close| is set to true, an extra "Connection: close" HTTP
  // header will be included.  |content_type| is the mime content type, not
  // including the "Content-Type: " string.
  // |extra_headers| should be either empty or a list of headers where each
  // header terminates with "\r\n".
  // |data| is the body of the message.  It's length will be specified via
  // a "Content-Length" header.
  bool Send(const std::string& status,
            bool connection_close,
            const std::string& content_type,
            const std::string& extra_headers,
            const std::string& data) const;

  // Clears all held state and prepares the socket for receiving a new request.
  void Clear();

 protected:
  // A fairly relaxed HTTP header parser.  Parses the method, path and
  // content length (POST only) of a request.
  // Returns true if a valid request was received and no errors occurred.
  bool ParseHeaders();

  // Figures out whether the request is a GET or POST and what path is
  // being requested.
  bool ParseMethodAndPath(const char* begin, size_t len);

  // Determines the length of the body and it's mime type.
  bool ParseContentLengthAndType(const char* headers, size_t length);

 protected:
  RequestMethod method_;
  size_t content_length_;
  std::string content_type_;
  std::string request_path_;
  std::string request_headers_;
  std::string data_;
};
```

### 전체코드

```cpp
int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage(
      "Example usage: ./peerconnection_server --port=8888\n");
  absl::ParseCommandLine(argc, argv);

  // InitFieldTrialsFromString stores the char*, so the char array must outlive
  // the application.
  const std::string force_field_trials = absl::GetFlag(FLAGS_force_fieldtrials);
  webrtc::field_trial::InitFieldTrialsFromString(force_field_trials.c_str());

  int port = absl::GetFlag(FLAGS_port);

  // Abort if the user specifies a port that is outside the allowed
  // range [1, 65535].
  if ((port < 1) || (port > 65535)) {
    printf("Error: %i is not a valid port.\n", port);
    return -1;
  }

  ListeningSocket listener;
  if (!listener.Create()) {
    printf("Failed to create server socket\n");
    return -1;
  } else if (!listener.Listen(port)) {
    printf("Failed to listen on server socket\n");
    return -1;
  }

  printf("Server listening on port %i\n", port);

  PeerChannel clients;
  typedef std::vector<DataSocket*> SocketArray;
  SocketArray sockets;
  bool quit = false;
  while (!quit) {
    fd_set socket_set;
    FD_ZERO(&socket_set);
    if (listener.valid())
      FD_SET(listener.socket(), &socket_set);

    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
      FD_SET((*i)->socket(), &socket_set);

    struct timeval timeout = {10, 0};
    if (select(FD_SETSIZE, &socket_set, NULL, NULL, &timeout) == SOCKET_ERROR) {
      printf("select failed\n");
      break;
    }

    for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i) {
      DataSocket* s = *i;
      bool socket_done = true;
      if (FD_ISSET(s->socket(), &socket_set)) {
        if (s->OnDataAvailable(&socket_done) && s->request_received()) {
          ChannelMember* member = clients.Lookup(s);
          if (member || PeerChannel::IsPeerConnection(s)) {
            if (!member) {
              if (s->PathEquals("/sign_in")) {
                clients.AddMember(s);
              } else {
                printf("No member found for: %s\n", s->request_path().c_str());
                s->Send("500 Error", true, "text/plain", "",
                        "Peer most likely gone.");
              }
            } else if (member->is_wait_request(s)) {
              // no need to do anything.
              socket_done = false;
            } else {
              ChannelMember* target = clients.IsTargetedRequest(s);
              if (target) {
                member->ForwardRequestToPeer(s, target);
              } else if (s->PathEquals("/sign_out")) {
                s->Send("200 OK", true, "text/plain", "", "");
              } else {
                printf("Couldn't find target for request: %s\n",
                       s->request_path().c_str());
                s->Send("500 Error", true, "text/plain", "",
                        "Peer most likely gone.");
              }
            }
          } else {
            HandleBrowserRequest(s, &quit);
            if (quit) {
              printf("Quitting...\n");
              FD_CLR(listener.socket(), &socket_set);
              listener.Close();
              clients.CloseAll();
            }
          }
        }
      } else {
        socket_done = false;
      }

      if (socket_done) {
        printf("Disconnecting socket\n");
        clients.OnClosing(s);
        assert(s->valid());  // Close must not have been called yet.
        FD_CLR(s->socket(), &socket_set);
        delete (*i);
        i = sockets.erase(i);
        if (i == sockets.end())
          break;
      }
    }

    clients.CheckForTimeout();

    if (FD_ISSET(listener.socket(), &socket_set)) {
      DataSocket* s = listener.Accept();
      if (sockets.size() >= kMaxConnections) {
        delete s;  // sorry, that's all we can take.
        printf("Connection limit reached\n");
      } else {
        sockets.push_back(s);
        printf("New connection...\n");
      }
    }
  }

  for (SocketArray::iterator i = sockets.begin(); i != sockets.end(); ++i)
    delete (*i);
  sockets.clear();

  return 0;
}
```