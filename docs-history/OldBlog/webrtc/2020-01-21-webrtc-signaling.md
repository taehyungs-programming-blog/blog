---
title: "(WebRTC) Signaling 이론"
permalink: webrtc/basicapp_02/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
---

Signaling은 초기에 메시지를 맺은 후 이후는 Peer to Peer로 동작하기에 이후에는 동작하지 않는다.

![](/file/image/webrtc-signaling_image_01.png)

단 Server가 있어서 Peer to Peer연결까지는 해줘야겠지?(말인 즉슨 서비스 공급자가 Signaling Server까지 만들어 줘야한다.)<br>
우선 연결은 다음과 같은 절차로 이루어 진다.<br>

![](/file/image/webrtc-signaling_image_02.png)

통화를 원하는 Alice가 web server에게 SDP(Session Sescription Protocol)을 보낸다.<br>

![](/file/image/webrtc-signaling_image_03.png)

서버에서는 Websockets, socket.io, pub/sub 등 어떤걸 쓰든 통신을 연결해 준다.<br>

![](/file/image/webrtc-signaling_image_04.png)

방화벽이 있어도 통신가능!<br>

![](/file/image/webrtc-signaling_image_05.png)

서로의 IP를 아는 방법은 세 가지가 있다.<br>

1. 같은 망에 있거나 특정한 조건으로 서로의 IP를 이미 알고 있다.
2. [STUN](https://www.3cx.com/global/kr/voip-sip-webrtc/stun-server/)을 이용한다. 
3. TURN을 이용한다. 

2. 3.은 뭐 이런 서버가 있다고 받아들이자.<br>
IP를 찾는 순서는 1. -> 2. -> 3.의 순이다.<br>

여기가 중요!! 어떤식으로 연결되는지 확인!

![](/file/image/webrtc-signaling_image_06.png)