---
title: "Hyper-v : 해상도 변경"
permalink: hyper-v/resolution/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-13 00:00:00 -0000
last_modified_at: 2020-06-13 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Hyper-V
category:
  - 해상도
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://qastack.kr/superuser/518484/how-can-i-increase-the-hyper-v-display-resolution)

```s
$ sudo apt-get install linux-image-extra-virtual
$ sudo gedit /etc/default/grub
```

```
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash video=hyperv_fb:1920x1080"
```

```s
$ sudo update-grub
$ sudo reboot
```
