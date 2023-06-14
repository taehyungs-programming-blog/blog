---
title: "(WebRTC) Data Exchange(이론)"
date: 2020-01-26 00:00:00 -0000
---

## Two Types of Data Channel

* Reliable
    * a little slower, but guaranteed delivery and order
    `option = {ordered: true};`

* Unreliable
    * Faster, but no guaranteed delivery or ordering
    `option = {ordered: false};`

> 뭐 이런 모드가 있다는 정도만 알아 두자.

---

## Other data channel options

* `ordered` - true for reliable, false for unreliable
* `maxRetransmitTime` - milliseconds
* `maxRetransmits` - maximum number of times to retry
* `protocol` - If you want something other than [SCTP(Stream Control Transmission Protocol)](https://m.blog.naver.com/PostView.nhn?blogId=winipe&logNo=150159936388&proxyReferer=https%3A%2F%2Fwww.google.com%2F)
* `negotiated` - true keeps WebRTC from setting up a channel on other side
* `id` - provide your own id

---

## Data channel vs WebSockets

* Data Channels use the same API
* But Data Channels are encrypted

---

## Transferring a file with webrtc

![](/file/image/webrtc-data-exchange_Image_01.png)