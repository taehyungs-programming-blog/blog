---
title: "간단한 WebRTC 어플리케이션 만들어보기 준비"
permalink: webrtc/basicapp_00/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-10 00:00:00 -0000
last_modified_at: 2020-03-25 00:00:00 -0000
---

## 정리하자면 ...

아래 예제는 `/Chapter 3/GetUserMediaInASingleBrowser/`으로 진행하겠음.

```s
$ git clone https://github.com/8bitscoding/webrtc-js-example
$ cd ./Chapter 3/GetUserMediaInASingleBrowser/

# check version
$ npm show ejs version
$ npm show express.io version
$ npm show express version

# edit package.json file

$ npm install
$ node server
```

![](/file/image/webrtc_Image_02.png)

server started가 나온다면 서버는 정상 동작<br>
접속해보자 http://localhost:3000

![](/file/image/webrtc_Image_03.png)

---

## 추가설명

* [Example Github](https://github.com/8bitscoding/webrtc-js-example)

단순 Clone해서 사용하면 안됨, <br>
`$ npm show <module name> version` 명령을 통해 자신의 버전과 맞게 package.json를 수정<br>
추가해야하는 모듈은 : coffee-script, connect 두 개이다.<br>

![](/file/image/webrtc_Image_01.png)

node_module폴더는 삭제 후 `$ npm install` 진행

---

**좀 더 상세히 설명**

> package.json은 아래와 같이 수정하면 된다.<br>

```s
$ npm show ejs version
$ npm show express.io version
$ npm show express version
```

확인 후

```json
{
	"name": "intro-webrtc",
	"version": "0.0.1",
	"private": true,
	"dependencies": {
		"coffee-script": "^1.12.7",
		"connect": "3.x",
		"ejs": "3.x",
		"express": "4.x",
		"express.io": "1.x"
	}
}
```

package.json 수정 후 `$ npm install` 수행<br>
설치가 된다면 `$ node server` 실행<br>

---

