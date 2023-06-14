---
title: "(webrtc) signaling 해보기"
permalink: webrtc/basicapp_04/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-12 00:00:00 -0000
last_modified_at: 2020-03-12 00:00:00 -0000
---

> 참고 [Github](https://github.com/8bitscoding/webrtc-js-example)의 Chapter 4. Implementing Signaling 소스코드 참조

```s
$ npm install
$ node server
```

> ![](/file/image/webrtc-basicapp-04-image-01.png)<br>
> 다음과 같은 에러가 발생한다면?<br>
> `\node_modules\express\`에서 `$ npm install connect` 수행

```js
// server.js
var express = require('express.io');
var app = express();
app.http().io();
var PORT = 3000;
console.log('server started on port ' + PORT);

app.use(express.static(__dirname + '/public'));

app.get('/', function(req, res){
	res.render('index.ejs');
});

app.listen(PORT);

app.io.route('ready', function(req) {
	req.io.join(req.data.chat_room);
	req.io.join(req.data.signal_room);
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

app.io.route('signal', function(req) {
	//Note the use of req here for broadcasting so only the sender doesn't receive their own messages
	req.io.room(req.data.room).broadcast('signaling_message', {
        type: req.data.type,
		message: req.data.message
    });
})
```

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
		<video id="myVideoTag" autoplay></video>
		<video id="theirVideoTag" autoplay></video>
		<div>
			<label>Your Name</label><input id="myName" type="text"/>
			<label>Message</label><input id="myMessage" type="text"/>
			<input id="sendMessage" type="submit"/>
			<div id="chatArea">Message Output:</br></div>
			<div id="signalingArea">Signaling Messages:</br></div>
		</div>
		
		<script>
			var myVideoArea = document.querySelector("#myVideoTag");
			var theirVideoArea = document.querySelector("#theirVideoTag");
			var myName = document.querySelector("#myName");
			var myMessage = document.querySelector("#myMessage");
			var sendMessage = document.querySelector("#sendMessage");
			var chatArea = document.querySelector("#chatArea");
			var signalingArea = document.querySelector("#signalingArea");
			var ROOM = "chat";
			var SIGNAL_ROOM = "signal_room";
			var configuration = {
				'iceServers': [{
					'url': 'stun:stun.l.google.com:19302'
				}]
			};
			var rtcPeerConn;
			
			io = io.connect();
			io.emit('ready', {"chat_room": ROOM, "signal_room": SIGNAL_ROOM});  // 여기서 방을 생성
			
			//Send a first signaling message to anyone listening
			//This normally would be on a button click
			io.emit('signal',{"type":"user_here", "message":"Are you ready for a call?", "room":SIGNAL_ROOM});  // signal을 전송
			
			io.on('signaling_message', function(data) {     // 상대가 시그널 수신
				displaySignalMessage("Signal received: " + data.type);
				
				//Setup the RTC Peer Connection object
				if (!rtcPeerConn)
					startSignaling();
					
				if (data.type != "user_here") {
					var message = JSON.parse(data.message);
					if (message.sdp) {
						rtcPeerConn.setRemoteDescription(new RTCSessionDescription(message.sdp), function () {
							// if we received an offer, we need to answer
							if (rtcPeerConn.remoteDescription.type == 'offer') {
								rtcPeerConn.createAnswer(sendLocalDesc, logError);
							}
						}, logError);
					}
					else {
						rtcPeerConn.addIceCandidate(new RTCIceCandidate(message.candidate));
					}
				}
				
			});
			
			function startSignaling() {
				displaySignalMessage("starting signaling...");
				
				rtcPeerConn = new webkitRTCPeerConnection(configuration);
				
				// send any ice candidates to the other peer
				rtcPeerConn.onicecandidate = function (evt) {
					if (evt.candidate)
						io.emit('signal',{"type":"ice candidate", "message": JSON.stringify({ 'candidate': evt.candidate }), "room":SIGNAL_ROOM});
					displaySignalMessage("completed that ice candidate...");
				};
				
				// let the 'negotiationneeded' event trigger offer generation
				rtcPeerConn.onnegotiationneeded = function () {
					displaySignalMessage("on negotiation called");
					rtcPeerConn.createOffer(sendLocalDesc, logError);
				}
				
				// once remote stream arrives, show it in the remote video element
				rtcPeerConn.onaddstream = function (evt) {
					displaySignalMessage("going to add their stream...");
					theirVideoArea.src = URL.createObjectURL(evt.stream);
				};
				
				// get a local stream, show it in our video tag and add it to be sent
				navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
				navigator.getUserMedia({
					'audio': false,
					'video': true
				}, function (stream) {
					displaySignalMessage("going to display my stream...");
					myVideoArea.src = URL.createObjectURL(stream);
					rtcPeerConn.addStream(stream);
				}, logError);
			  
			}
			
			function sendLocalDesc(desc) {
				rtcPeerConn.setLocalDescription(desc, function () {
					displaySignalMessage("sending local description");
					io.emit('signal',{"type":"SDP", "message": JSON.stringify({ 'sdp': rtcPeerConn.localDescription }), "room":SIGNAL_ROOM});
				}, logError);
			}
			
			function logError(error) {
				displaySignalMessage(error.name + ': ' + error.message);
			}
			
			io.on('announce', function(data) {
				displayMessage(data.message);
			});
			
			io.on('message', function(data) {
				displayMessage(data.author + ": " + data.message);
			});
			
			function displayMessage(message) {
				chatArea.innerHTML = chatArea.innerHTML + "<br/>" + message;
			}
			
			function displaySignalMessage(message) {
				signalingArea.innerHTML = signalingArea.innerHTML + "<br/>" + message;
			}
			
			sendMessage.addEventListener('click', function(ev){
				io.emit('send', {"author":myName.value, "message":myMessage.value, "room":ROOM});
				ev.preventDefault();
			}, false);
			
			
			
		</script>
	</body>
</html>
```