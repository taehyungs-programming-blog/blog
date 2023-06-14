---
title: "webrtc : janus - Janus Server 설치"
permalink: webrtc/janus/install/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-12 00:00:00 -0000
last_modified_at: 2020-06-12 00:00:00 -0000
sidebar:
  title: "webrtc 목차"
  nav: webrtc
tag:
  - webrtc
  - janus
category:
  - install
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

* [참고사이트1](https://github.com/meetecho/janus-gateway)
* [참고사이트2](https://alnova2.tistory.com/1119)

## install

```s
$ sudo -i
$ sudo apt-get install libmicrohttpd-dev libjansson-dev libnice-dev libssl-dev libsrtp-dev libsofia-sip-ua-dev libglib2.0-dev libopus-dev libogg-dev libcurl4-openssl-dev pkg-config gengetopt libtool automake cmake libconfig-dev
```

```s
wget https://github.com/cisco/libsrtp/archive/v2.0.0.tar.gz
tar xzvf v2.0.0.tar.gz
cd libsrtp-2.0.0/
./configure --prefix=/usr --enable-openssl
make shared_library && sudo make install
```

```s
git clone git://git.libwebsockets.org/libwebsockets
cd libwebsockets
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_C_FLAGS="-fpic" ..
make && sudo make install
```

```s
git clone https://github.com/meetecho/janus-gateway.git
cd janus-gateway
sh autogen.sh
./configure --prefix=/usr/local
make && sudo make install
```

```s
cd /usr/local/etc/janus
sudo rename -v 's/cfg.sample/cfg/' *
```

## start

```s
 /usr/local/bin/janus -F /usr/local/etc/janus
```