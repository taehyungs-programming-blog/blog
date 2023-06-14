---
title: "Linux : Linux Samba(공유폴더) 사용하기"
permalink: linux/samba/                # link 직접 지정
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
  - Linux
category:
  - samba
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](https://psychoria.tistory.com/678)

```s
sudo apt-get -y update
sudo apt-get -y install samba
sudo smbpasswd -a <계정명>
sudo nano /etc/samba/smb.conf
```

아래를 제일 마지막 줄에 추가

```
[multimedia]
comment = multimedia directory
path = /data/Multimedia
valid users = id1,id2
writeable = yes
read only = no
create mode = 0777
directory mode = 0777
```

```s
sudo service smbd restart
```

폴더 우클릭 후 공유 폴더 지정해서 쓰면된다.

![](/file/image/linux-samba.png)