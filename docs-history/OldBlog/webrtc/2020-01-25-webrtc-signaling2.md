---
title: "(WebRTC) Signaling 실습(Setting Up Socket.io)"
date: 2020-01-25 00:00:00 -0000
---

## Setting up Socket.io

> 우선은 각 버전확인

```s
$ npm show ejs version
$ npm show express.io version
$ npm show express version
```

> 위 세 개의 패키지를 사용한다.

> package.json

```json
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

> install

```s
$ npm install
```

> views\index.ejs

```html
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
				videoArea.src = window.URL.createObjectURL(stream);
				videoArea.play();
			}
			
			function onError(error) {
				console.log("Error with getUserMedia: ", error);
			}
			
		</script>
	</body>
</html>
```

> server.js

```js
var express = require('express.io');
var app = express();
app.http().io();
var PORT = 3000;
console.log('server started on port ' + PORT);

app.use(express.static(__dirname + '/public'));

app.get('/', function(req, res){
	res.render('index.ejs');
});

app.io.route('ready', function(req) {
	req.io.join(req.data)
	app.io.room(req.data).broadcast('announce', {
		message: 'New client in the ' + req.data + ' room.'
	})
})

app.io.route('send', function(req) {
    app.io.room(req.data.room).broadcast('message', {
        message: req.data.message,
		author: req.data.author
    });
})

app.listen(PORT);
```

> npm server start

```s
$ node server
```