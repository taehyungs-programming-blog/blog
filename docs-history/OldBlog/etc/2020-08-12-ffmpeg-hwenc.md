---
title: "ffmpeg : ffmpeg 기본적 사용법 + hw-encoding"
permalink: ffmpeg/hwenc/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-21 00:00:00 -0000
last_modified_at: 2020-08-12 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - ffmpeg
category:
  - hw-encoding
header:
  teaser: /file/image/etc-page-teaser.gif
---

* 참고사이트 : https://peche326.tistory.com/58
* ffmpeg 사용 참고 유투브 : https://www.youtube.com/watch?v=H81fgnaXkHY
* ffmpeg 다운로드 사이트 : http://ffmpeg.org/download.html

> 참고로 너무 최신버전의 ffmpeg을 받아버리면 각 hw api를 지원못하는 경우가 발생해 버린다.<br>
> 적당히 옛날 버전의 ffmpeg을 사용하자<br>

---

## NVENC Encode

```s
ffmpeg -hwaccel cuda -hwaccel_output_format cuda -i 4k-1.mkv -c:v h264_nvenc -preset slow 4k-1-output.mkv
```

---

## QSV Encode

```s
ffmpeg -hwaccel qsv -c:v h264_qsv -i 4k-1.mkv -c:v h264_qsv 4k-1-qsv.mkv
```

---

## AMD Encode

```s
ffmpeg -i 4k-1.mkv -c:v hevc_amf -rc cqp -qp_p 0 -qp_i 0 -c:a copy 4k-1-amd.mkv
```

---

## ffmpeg 명령 옵션

```s
ffmpeg -encoders
```

![](/file/image/ffmpeg-hwenc.png)

* `-vcodec` or `-v:c` : video codec
* `-acodec` or `-a:c` : audio codec