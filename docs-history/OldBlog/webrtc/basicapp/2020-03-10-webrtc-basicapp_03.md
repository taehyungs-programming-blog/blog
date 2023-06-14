---
title: "(webrtc) socket.io 사용하기"
permalink: webrtc/basicapp_03/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-12 00:00:00 -0000
last_modified_at: 2020-03-12 00:00:00 -0000
---

> 참고 [Github](https://github.com/8bitscoding/webrtc-js-example)의 Chapter 4. SettingUpSocketIO 소스코드 참조

## 개발환경 세팅

우선 Github를 수정해두겠지만 그냥 clone해서 사용하면 안된다<br>
이전강좌에서도 설명을 해뒀지만 버전을 맞추는 작업을 해야한다.<br>

```json
// package.json
{
	"name": "intro-webrtc",
	"version": "0.0.1",
	"private": true,
	"dependencies": {
		"express": "4.x",
		"ejs": "2.x",
		"express.io": "1.x"
	}
}
```

각각의 버전확인이 필요

```s
$ npm show ejs version
$ npm show express version
$ npm show express.io version
```

![](/file/image/webrtc-basicapp-03-image-01.png)

확인한 버전을 기반으로

```json
// package.json
{
	"name": "intro-webrtc",
	"version": "0.0.1",
	"private": true,
	"dependencies": {
		"express": "4.x",
		"ejs": "3.x",
		"express.io": "1.x"
	}
}
```

```s
$ npm install
```

![](/file/image/webrtc-basicapp-03-image-02.png)

오케이, <br>
우선은 한번 실행해 보자.

```s
$ node server
```

![](/file/image/webrtc-basicapp-03-image-03.png)

??? 패키지 설치가 안된게 있군...

![](/file/image/webrtc-basicapp-03-image-04.png)

```json
{
    // package.json
	"name": "intro-webrtc",
	"version": "0.0.1",
	"private": true,
	"dependencies": {
		"express": "4.x",
		"ejs": "2.x",
		"express.io": "1.x",
		"coffee-script": "1.x"
	}
}
```

```s
$ npm install
$ node server
```

> 혹시 이래도 안된다면
> ```json
> // package.json
> {
> 	"name": "intro-webrtc",
> 	"version": "0.0.1",
> 	"private": true,
> 	"dependencies": {
> 		"express": "4.x",
> 		"ejs": "2.x",
> 		"express.io": "1.x",
> 		"coffee-script": "~1.x"
> 	}
> }
> ```
> 
> ```s
> $ npm update
> $ node server
> ```

![](/file/image/webrtc-basicapp-03-image-05.png)

끝이 없군 ...<br>
만약 위와같은 에러가 발생하면 node_modules아래의 express폴더로 들어가서<br>

```s
$ npm install connect
```

명령을 통해 직접 connect module을 express안에 설치해 줘야한다.

```json
// package.json
{
	"name": "intro-webrtc",
	"version": "0.0.1",
	"private": true,
	"dependencies": {
		"express": "4.x",
		"ejs": "2.x",
		"express.io": "1.x",
        "coffee-script": "~1.x",
        "connect": "3.x"
	}
}
```

```s
$ npm update
$ node server
```

![](/file/image/webrtc-basicapp-03-image-06.png)

오케이... 접속해보자. https://localhost:3000

![](/file/image/webrtc-basicapp-03-image-07.png)

음 뭐 대충 채팅을하는 서버인가? 소스코드를 분석해보자.

---

## 코드분석

우선 서버입장에서 말고 웹에 들어온 클라이언트 입장에서 보자<br>
index.ejs가 호출된다

```html
<!-- index.ejs -->

<!-- 앞은 생략 -->
io = io.connect();
io.emit('ready', ROOM);

<!-- ready를 호출 -->
```

```js
// server.js

// 서버에서는 ready를 받으면 announce를 부른다.
app.io.route('ready', function(req) {
	req.io.join(req.data)
	app.io.room(req.data).broadcast('announce', {
		message: 'New client in the ' + req.data + ' room.'
	})
})
```

```html
<!-- index.ejs -->

io.on('announce', function(data) {
	displayMessage(data.message);
});
```

### 전체코드

```js
// server.js
var express = require('express.io');
var app = express();
app.http().io();
var PORT = 3000;
console.log('server started on port ' + PORT);

app.use(express.static(__dirname + '/public')); // styles.css를 쓰겠단 말인거 같고. ..

app.get('/', function(req, res){
	res.render('index.ejs');                    // ui는 index.ejs로 그려달라
});

// 새로운 유저가 들어오면 여기가 호출되는 듯 하며
app.io.route('ready', function(req) {
	req.io.join(req.data)
	app.io.room(req.data).broadcast('announce', {
		message: 'New client in the ' + req.data + ' room.'
	})
})

// 메시지를 보내면 여기가 호출되는 듯 하며
app.io.route('send', function(req) {
    app.io.room(req.data.room).broadcast('message', {
        message: req.data.message,
		author: req.data.author
    });
})

app.listen(PORT);
```

아래는 index.ejs소스인데 socket.io가 어떻게 사용되었는지 확인이 중요하다.

```html
<!-- index.ejs -->
<!doctype html>
<html lang="en">
	<head>
		<title>O'Reilly Introduction to WebRTC</title>
		<link rel="stylesheet" type="text/css" href="styles.css">
		<script src="/socket.io/socket.io.js"></script>
	</head>
	<body>
		<div id="cameraSelector">
			Video: <select id="camera"></select>
		</div>
		<p><button id="takeProfilePicture" type="button" autofocus="true">Create Profile Picture</button></p>
		<video id="videoTag" autoplay></video>
		<div>
			<label>Your Name</label><input id="myName" type="text"/>
			<label>Message</label><input id="myMessage" type="text"/>
			<input id="sendMessage" type="submit"/>
			<div id="chatArea">Message Output:</br></div>
		</div>
		
		<script>
			var videoArea = document.querySelector("video");
			var videoSelect = document.querySelector('#camera');
			var myName = document.querySelector("#myName");
			var myMessage = document.querySelector("#myMessage");
			var sendMessage = document.querySelector("#sendMessage");
			var chatArea = document.querySelector("#chatArea");
			var ROOM = "chat";
			
			if (typeof MediaStreamTrack === 'undefined' || typeof MediaStreamTrack.getSources === 'undefined') {
				document.querySelector("#cameraSelector").style.visibility="hidden";
			} else {
				MediaStreamTrack.getSources(getCameras);
			}
			
			videoSelect.onchange = startStream;
			
			//startStream();
			
			io = io.connect();
			io.emit('ready', ROOM);
			
			io.on('announce', function(data) {
				displayMessage(data.message);
			});
			
			io.on('message', function(data) {
				displayMessage(data.author + ": " + data.message);
			});
			
			sendMessage.addEventListener('click', function(ev){
				io.emit('send', {"author":myName.value, "message":myMessage.value, "room":ROOM});
				ev.preventDefault();
			}, false);
			
			function displayMessage(message) {
				chatArea.innerHTML = chatArea.innerHTML + "<br/>" + message;
			}
			
			function getCameras(sourceInfos) {
				for (var i = 0; i !== sourceInfos.length; ++i) {
					var sourceInfo = sourceInfos[i];
					var option = document.createElement('option');
					option.value = sourceInfo.id;
					if (sourceInfo.kind === 'video') {
						option.text = sourceInfo.label || 'camera ' + (videoSelect.length + 1);
						videoSelect.appendChild(option);
					}
				}
			}
			
			function startStream() {
				navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
				var videoSource = videoSelect.value;
				var constraints = {
					audio: false, 
					video: false
				};
				
				navigator.getUserMedia(constraints, onSuccess, onError);
			}
			
			function onSuccess(stream) {
				console.log("Success! We have a stream!");
				// videoArea.src = window.URL.createObjectURL(stream);
				// 구 버전이라 지원안됨
                videoArea.srcObject=stream;
				videoArea.play();
			}
			
			function onError(error) {
				console.log("Error with getUserMedia: ", error);
			}
			
		</script>
	</body>
</html>
```