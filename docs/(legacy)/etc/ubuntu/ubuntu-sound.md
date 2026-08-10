---
layout: default
title: "Ubuntu 스피커/마이크 오류"
parent: "Ubuntu(Linux)"
grand_parent: "Etc 🛠"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고 사이트 🌍](https://www.youtube.com/watch?v=oJdwOqfMpSg)

* 마이크, 스피커가 Dummy만 나오고 잡히지 않는다면 아래와 같이 해보자

```bash
$ sudo touch /usr/share/pipewire/media-session.d/with-pulseaudio
$ systemctl --user restart pipewire-session-manager
```