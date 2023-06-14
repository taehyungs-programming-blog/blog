---
title: "WebRTC - example : 복잡하기에 여기만 보자 대충 동작을 정리했음."
permalink: webrtc/basic_arrange/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-03 00:00:00 -0000
last_modified_at: 2020-05-03 00:00:00 -0000
sidebar:
  title: "webrtc"
  nav: webrtc
tag:
  - webrtc
category:
  - 정리
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

![](/file/image/webrtc-basic-arrange.png)

뭐 대략 이런데... 위 그림만으론 이해가 힘들다.<br>
우선 소스코드는 [여기](https://github.com/8bitscoding/webrtc-basic-arrange)를 참고하자

처음으로 기억할 것은 웹 2개 A, B에서 B는 기존에 접속이 되어있고<br>
A에서 접속을 시도한다는것을 기억하자.

**Client A**

```js
// index.ejs
io.emit('signal',{"type":"user_here", "message":"Are you ready for a call?", "room":SIGNAL_ROOM});
```

```js
// server.js
app.io.route('signal', function(req) {
	// Client A는 signaling_message를 B로 보내게 된다.
	req.io.room(req.data.room).broadcast('signaling_message', {
        type: req.data.type,
		message: req.data.message
    });
})
```

**Client B**

```js
// index.ejs
io.on('signaling_message', function(data) {
    displaySignalMessage("Signal received: " + data.type);
```

메시지를 받으면 `io.on`에서도 처리를 하지만 negotiation도 호출이 된다.

```js
// index.ejs
rtcPeerConn.onnegotiationneeded = function () {
    rtcPeerConn.createOffer(sendLocalDesc, logError);
}
```

negotiation이 호출되면 SDP Offer를 보내게 된다.

```js
// index.ejs
function sendLocalDesc(desc) {
    rtcPeerConn.setLocalDescription(desc, function () {
        displaySignalMessage("sending local description");
        io.emit('signal',{"type":"SDP", "message": JSON.stringify({ 'sdp': rtcPeerConn.localDescription }), "room":SIGNAL_ROOM});
    }, logError);
}
```

ice역시 같이 보내게 된다.

**Client A**

SDP-Offer를 받으면 RTCSessionDescription를 생성하고 Answer를 다시보내고 negotiation역시 호출이 된다.

```js
io.on('signaling_message', function(data) {
    displaySignalMessage("Signal received: " + data.type);
    
    //Setup the RTC Peer Connection object
    if (!rtcPeerConn) {
        startSignaling();
    }

    if (data.type != "user_here") {
        var message = JSON.parse(data.message);
        if (message.sdp) {
            rtcPeerConn.setRemoteDescription(new RTCSessionDescription(message.sdp), function () {
                // if we received an offer, we need to answer
                if (rtcPeerConn.remoteDescription.type == 'offer') {
                    rtcPeerConn.createAnswer(sendLocalDesc, logError);
                }
            }, logError);
        }
```

```js
// index.ejs
rtcPeerConn.onnegotiationneeded = function () {
    rtcPeerConn.createOffer(sendLocalDesc, logError);
}
```

뭐 대략 이런식으로 동작하는데 엄청 복잡함... 좀더 쉽게 설명한 사이트를 찾아야할 듯...

사용된 API를 정리해야할 것 같은데...

* `webkitRTCPeerConnection` : 전반적 WebRTC를 관리 Onicecandiate, onnegotiationeeded등을 관리
* `RTCSessionDescription` : SDP 관리
* `RTCIceCandidate` : ICE 관리