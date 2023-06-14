---
title: "(webrtc) 이론적 부분"
permalink: webrtc/basic/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
---

## webrtc의 동작 흐름

아래는 간략설명이고 더 아래에 정확한 설명 기재

* [Fetching](/webrtc/basic/#fetching/) - 음성 및 영상 데이터 수집
* [Signaling](/webrtc/basic/#signaling/) - peer와 연결을 맺기 위해서, 상대 peer의 정보 탐색/교환
* [Connection](/webrtc/basic/#connection/) - 탐색된 peer와 connection 작업
* [Communication](/webrtc/basic/#communication/) - Connection을 통해 맺은 채널을 통해 음성/영상/텍스트 데이터를 주고 받음

---

### Fetching

`MediaStream`, `getUserMedia`를 이용 사용자의 영상 및 음성 정보를 가져온다.

### Signaling

별도로 존재하는 **Signaling Server**를 통해 아래의 정보를 교환한다.

> Signaling Server는 프로그래머가 별도로 구현해 줘야한다.

* Network 정보 교환
    - ICE Framework를 사용 find candidate(ip, port 정보 찾음)
    - 순서는 이러하다.
        1. `RTCPeerConnection` Object를 생성하고 `RTCPeerConnection.onicecandidate`를 통해 내 ICE Candidate정보(네트워크 정보)가 확보
        2. 확보된 ICE Candidate 정보를 Signaling Server로 전달
        3. 상대의 ICE Candidate 정보를 받으면 `RTCPeerConnection.addIceCandidate`를 통해 정보를 관리(쌍방이 모두 해야하는 작업)
* Media Capability 정보 교환
    - sdp(Session Description Protocol) 형식을 따르는 blob인 `offer`, `answer`를 주고 받음
    - 예를 들어 브라우저 정보, 상대가 사용가능한 코덱 해상도 정보등
* Session Control Messages 교환
    - Session 초기화, 종료
    - Error 정보
        1. `RTCPeerConnection.createOffer`를 호출해 Offer SDP를 생성(브라우저에서 사용간으한 코덱이나 해상도 정보 포함)
        2. Signaling Server를 통해 Offer SDP를 송신
        3. 상대편에서 Offer SDP를 받아 `RTCPeerConnection.setRemoteDescription` 수행
        4. `RTCPeerConnection.createAnswer`호출 Answer SDP를 생성 후 Signaling Server에 전달
        5. `RTCPeerConnection.setRemoteDescription`를 호출해 Answer SDP를 등록
        6. 여기까지 완료되면 p2p연결이 성공적으로 완료되었다고 판단

### Connection

`RTCPeerConnection.setRemoteDescription`이 완료되었다면 Connection이 완료되었다고 판단

### Communication

통신부 인데 Webrtc는 두 가지 데이터를 교환한다. 영상/오디오, text데이터

* 영상/오디오
    - 송신 : `getUserMedia`를 통해 영상/오디오 정보를 취득 `addTrack` 데이터 스트림 채널에 전송
    - 수신 : 상대방의 Track이 감지되면 수신 시작
* text 데이터
    - 송신 : `RTCPeerConnection.createDataChannel`를 통해 특정 이름의 데이터 채널 개설
    - 수신 : `RTCPeerConnection.ondatachannel`를 통해 수신

---

> * [참고사이트](http://jaynewho.com/post/36)