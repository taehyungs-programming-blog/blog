---
title: "webrtc : ffmpeg - 기본적 기능 사용해보기"
permalink: webrtc/ffmepg/use/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-21 00:00:00 -0000
last_modified_at: 2020-06-21 00:00:00 -0000
sidebar:
  title: "webrtc 목차"
  nav: webrtc
tag:
  - webrtc
  - ffmpeg
category:
  - 사용기
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

* [참고사이트](https://kyoko0825.tistory.com/entry/%EC%9C%88%EB%8F%84%EC%9A%B0-10%EC%97%90%EC%84%9C-ffmpeg-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0)

- [ffmpeg 다운로드 링크](https://ffmpeg.zeranoe.com/builds/)에서 다운로드를 받는다.
- bin파일을 환경변수 path에 넣는다.

![](/file/image/ffmpeg-01.png)

> bin파일 내부

- 임의의 video.mp4를 준비한다.
- 아래 명령을 시험삼아 cmd에서 실행해 본다.

```s
$ ffmpeg -i video.mp4 -vn -ar 44100 -ac 1 -b:a 32k -f mp3 audio.mp3
```

- video.mp4에서 audio.mp3를 뽑아낸다.

명령어에 대한 간단한 설명은

* -i video.mp4 : 인풋 파일의 이름이 video.mp4
* -vn : 비디오 스트림 버리기(?) (=Leave out the video stream)
* -ar 44100 : 오디오 resolution(해상도)을 44100Hz로 설정
* -ac 1 : 오디오 채널을 1로 설정(=모노)
* -b:a 32k : 오디오 비트레이트를 32kbps로 설정
* -f mp3 : mp3로 변환, 만약에 해당 명령어를 사용하지 않으면 ffmpeg이 출력 파일 확장자를 보고 자동으로 설정함
* audio.mp3 : 출력 파일 이름

- 이런식으로 인코딩을 지원해 주는 도구라고 생각하자.