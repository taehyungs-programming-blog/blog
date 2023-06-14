---
title: "C++"
permalink: cpp/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-15 00:00:00 -0000
last_modified_at: 2020-05-09 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
tag:
  - C++
category:
  - category
classes: wide
excerpt: "C++와 관련된 지식을 정리한 페이지 입니다."
header:
  teaser: /file/image/qt-page-teaser.gif
  overlay_image: /file/image/main-page.jpg
  overlay_filter: 0.1 # rgba(255, 0, 0, 0.5)
  caption: "Photo credit: [**EBS**](https://ebs.co.kr)"
  actions:
    - label: "C++ HomePage"
      url: "http://www.cplusplus.com/reference/"
---

## 추가

* [not_null](/cpp/not_null/)
* [std::optional](/cpp/optional/)
* [Windows IPC](/cpp/windows/ipc/)

### 다시정리 시리즈

* [unique_ptr](/cpp/stl/spointer/unique_ptr/)
* [shared_ptr](/cpp/stl/spointer/shared_ptr/)
* [weak_ptr](/cpp/stl/spointer/weak_ptr/)

* [thread 시작](/cpp/cpp11-thread/start/)
* [thread 도우미 함수](/cpp/cpp11-thread/help-func/)
* [mutex](/cpp/cpp11-thread/mutex/)
* [notify, wait](/cpp/cpp11-thread/notify-wait/)

### Advanced C++

* [const, constexpr](/cpp/cpp11/const-constexpr/)

---

## Coding Test

* [코딩테스트 : 문제 풀이](/cpp/quize/)
* [코딩테스트 : Good Pattern](/cpp/good-pattern/)

---

## DirectX

* [강좌](https://www.youtube.com/watch?v=_4FArgOX1I4&list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD)
* [Get Code](https://github.com/EasyCoding-7/DirectX-basic-Tutorial)

* [Win32 : project setting](/cpp/directx/basic/1/)
* [Win32 : make window](/cpp/directx/basic/2/)
* [Win32 : WndProc](/cpp/directx/basic/3/)
* [Win32 : Window Message](/cpp/directx/basic/4/)
* [Win32 : WM_CHAR/Mouse](/cpp/directx/basic/5/)
* [Win32 : Window Framework](/cpp/directx/basic/6/)
* [Win32 : Error Handling](/cpp/directx/basic/7/)
* [Win32 : Keyboard/WM_SYSKEYDOWN](/cpp/directx/basic/8/)

* [D3D : Architecture/SwapChain](/cpp/directx/basic/12/)
* [D3D : Fill Screen/Present](/cpp/directx/basic/13/)
* [D3D : ComPrt Smart Pointer](/cpp/directx/basic/15/)

* [D3D : Tutorial Triangle Pt1](/cpp/directx/basic/16/)

---

## Unreal

* [언리얼 설치하기](/cpp/unreal/2-1/)
* [C++ 클래스, UObject 생성해보기](/cpp/unreal/2-2/)
* [Actor Class : 구성요소 설명](/cpp/unreal/2-3/)
* [Actor Class : 위치 벡터(Position Vectors)](/cpp/unreal/2-4/)
* [Actor Class : F-Vector](/cpp/unreal/2-5/)
* [Actor Class : F-Vector2](/cpp/unreal/2-5-1/)
* [Actor Class : Collision](/cpp/unreal/2-6/)

## WebRTC Native 다시 분석 시작 ...

* [Get Code](https://github.com/EasyCoding-7/webrtc_example)

* [Server](/cpp/nwebrtc/server/)
* [Client](/cpp/nwebrtc/client/)

* [ICE, STUN, TURN](/cpp/nwebrtc/ice-stun-turn/)

---

## C++ 게임만들기

Part1 - C++

### 어셈블리 언어 사용방법 정리

* [1 - 개발환경 구성](/cpp/game/p1/1/) : sasm
* [2 - 데이터, 레지스터](/cpp/game/p1/2/) : 10, 2, 16진수 표현법
* [3 - 문자와 엔디안](/cpp/game/p1/3/)
* [4 - 사칙연산](/cpp/game/p1/4/)
* [5 - 쉬프트 연산](/cpp/game/p1/5/)
* [6 - 분기문](/cpp/game/p1/6/)
* [7 - 반복문](/cpp/game/p1/7/)
* [8 - 배열과 주소](/cpp/game/p1/8/)
* [9 - 함수 기초](/cpp/game/p1/9/)
* [10 - 스택 메모리](/cpp/game/p1/10/)

* [11 - 개발환경 구성](https://visualstudio.microsoft.com/ko/) : VisualStudio, C++ 콘솔 앱
* [12 - 정수](/cpp/game/p1/12/)

나머지는 C++에 대한 내용이라 별도로 정리하지 앟음

---

## C++20 정리

* [lambda](/cpp/cpp20/lambda/)
* [attributes](/cpp/cpp20/attributes/)
* [three way comparison](/cpp/cpp20/three-way-comparison/)
* [MICS](/cpp/cpp20/misc/)

* [concept1](/cpp/cpp20/concept1/)
* [concept2](/cpp/cpp20/concept2/)
* [ranges1](/cpp/cpp20/ranges1/)
* [ranges2](/cpp/cpp20/ranges2/)

* [coroutine](/cpp/cpp20/coroutine/)
* [module](/cpp/cpp20/module/)

* [stl 변화 1](/cpp/cpp20/stl1/)
* [stl 변화 2](/cpp/cpp20/stl2/)

---

## C++ 문법

* [Basic C++(11이전) 문법](/cpp/basic/)
* [Modern C++(11이후) 문법](/cpp/modern/)

* [pragma region](/cpp/pragma-region/)

* [Exception](/cpp/exception/)
* [Function Pointer](/cpp/function-pointer/)
* [decltype](/cpp/decltype/)
* [auto](/cpp/auto/)
* [Range-Based Loops](/cpp/range-based-loops/)

* [Nested Template Classes](/cpp/nest-template/)
* [초기화](/cpp/initializer/)
* [람다식](/cpp/lambda/)
* [위임 생성자(delegate constructor)](/cpp/delegate-constructor/)
* [lvalue와 rvalue](/cpp/l-r-value/)

* [연산자와 lvalue](/cpp/l-value/)
* [move](/cpp/move/)
* [명시적형변환(Explicit casting 명시적형변환)](/cpp/explicit-casting/)
* [perfect forward 다시 정리](/cpp/perfect-forward/)
* [foreach](/cpp/foreach/)

* [NULL vs nullptr](/cpp/NULL-nullptr/)
* [static_assert](/cpp/static_assert/)
* [override](/cpp/override/)
* [constexpr](/cpp/constexpr/)
* [char 종류](/cpp/char/)

## Effective C++

* [explicit](/cpp/effective/explicit/)
* [define](/cpp/effective/define/)
* [const](/cpp/effective/const/)
* [객체의 초기화](/cpp/effective/initialize/)
* [컴파일러에 의해 생성되는 생성자](/cpp/effective/constructor/)

* [묵시적 함수 사용을 막아보자](/cpp/effective/implied-func/)
* [기본 클래스와 가상함수의 정리](/cpp/effective/base-class/)
* [소멸자에서 발생한 예외](/cpp/effective/exception/)
* [생성자/소멸자 호출 중 가상함수를 부르면 컴파일 에러](https://www.youtube.com/watch?v=8tlHAkaqNL4&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=9)

## STL

* [STL](/cpp/stl/)

## Template

* [Template](/cpp/template/)

## Desing Pattern

* [Design Pattern](/cpp/desing-pattern/)

## Thread

* [C++11 Thread](/cpp/thread/)

## WIN32, MFC

* [과거자료](https://github.com/8BitsCoding/RobotMentor/wiki/MFC)

* [Console log 출력](/cpp/mfc/console-log/)
* [16진수로 핸들(HWND) 주소 받아 HWND 객체 만들기](/cpp/mfc/16hwnd/)

```cpp
// int to HWND
int xAdd = 0x5176e;
HWND hwnd = (HWND)xAdd;

// HWND to string
stringstream ss;
ss << std::hex << xAdd;
ss.str();         // string
ss.str().c_str(); // const char *

// const char * to HWND
HWND hwnd = (HWND)(int)strtol(const_char, NULL, 16);  // 0x일 경우 0으로
```

* [MFC 이미지 처리하기(링크)](https://lazineer.tistory.com/10)
* [특정 프로그램 Capture](/cpp/specific-window-capture/)

* [wchar_t cout 처리]()

```cpp
wchar_t * wc = "hello";
std::wcout << wc << std::endl;
```

* [process간 Event 공유(OpenEvent)](/cpp/win32/process-event-share/)
* [공유메모리 접근(shared memoey mapping)](/cpp/win32/file-mapping/)
* [Dll 함수사용 (GetProcAddress)](/cpp/win32/use-dll-func/)

* [Thread 사용](/cpp/win32/thread/)
* [DC에 연결된 BITMAP 정보 처리](/cpp/win32/dc-bitmap/)

* [window 클립보드에 데이터 저장]()

```cpp
//copy to clipboard
OpenClipboard(nullptr);
EmptyClipboard();
SetClipboardData(CF_BITMAP, capturedScreenBitmap);
CloseClipboard();
```

---

## cef(Chromium Embedded Framework)

* [build](/cpp/cef/build/)

---

## DirectX2D

* [참고사이트](https://hombody.tistory.com/11?category=846211)
* [DirectX SDK Download](https://www.microsoft.com/en-us/download/confirmation.aspx?id=6812)
* [SDK가 설치되지 않는다면](https://jiras.tistory.com/17)

```s
# 만약 DirectX SDK 설치가 안된다면?
$ MsiExec.exe /passive /X{F0C3E5D1-1ADE-321E-8167-68EF0DE699A5}
$ MsiExec.exe /passive /X{1D8E6291-B0D5-35EC-8441-6616F567A0F7}
```

> * 빌드환경 : OS(Windows10(1909/18363.1082)), IDE(VS2017(15.9.24))

* [1. DirectX2d Window 띄우기](/cpp/dxd2/1/)
* [2. 세모그리기](/cpp/dxd2/2/)
* [3. imgui 적용하기](/cpp/dxd2/3/)

* 셰이더(Shader) : 그림을 그려주는 도구, Pixel, Texture Shader가 있다.
* 텍스쳐(Texture) : 3D 폴리곤 위에 그려지는 이미지라 생각
* `ID3D11ShaderResourceView*` : 셰이더 리소스 뷰 (그려주는 셰이더의 뷰를 변수로 받음)
* [obs에서 사용된 Shader] `device->OpenSharedResource` : 외부에 열린 device의 texture를 device handle로 연다
* [obs에서 사용된 Shader] `device->CreateShaderResourceView` : texture로 shaderresourceview를 만든다

---

## Direct X

 * [참고강의](https://www.youtube.com/playlist?list=PLOKPEzlY4JKSZLgY_jH4danTYinRKIPz1)
 * [참고사이트](https://blog.naver.com/PostView.nhn?blogId=whdgkks12347&logNo=221334350047&proxyReferer=https:%2F%2Fwww.google.com%2F)
 * [Get Code](https://github.com/EasyCoding-7/Direct3DExample)

 * [Direct3D 개발 환경 조성](/dxd/environment/)
 * [Direct3D Frame work 만들기](/dxd/make-framework/)
 * [Verices](/dxd/verices/)
 * [Matrix](/dxd/matrix/)

 * [index buffer](/dxd/d3d-index-buffer/)
 * [Framework 이용하기](/dxd/d3d-using-framework/)
 * [Direct3D를 이용한 2D게임 만들기](/dxd/2d-game/) : sprite, 이미지 로딩

---

## ROS

* [과거자료](https://github.com/8BitsCoding/RobotMentor/wiki/ROS)

---

## obs

* [obs build](/obs/build/)
* [sourcetree를 통해 본 model and view 구현](https://8bitscoding.github.io/obs-sourcetree-model-view/)
* [rtmp 동작에 관해서](https://8bitscoding.github.io/obs-rtmp/)

* [output과 service 분석](/obs/output-service/)
* [start stream](/obs/start-stream/)

---

## WebRTC

### Signaling 이론

1. [local에 카메라, 오디오 정보를 받는다.](/webrtc/sum_1/) : `navigator.getUserMedia` 사용하기
2. [통화를 원하는 A가 Signal Server로 SDP(Session Description Protocol)을 보낸다.](/webrtc/sum_2/)
3. Signal Server는 그 정보를 B에게 보내고 B에게서도 SDP를 받는다.
4. [A는 B에게서 받은 SDP를 바탕으로 ICE(Internet Connectivity Establishment) 정보를 Signal Server로 주고 서버는 다시 B에게 전달한다.](/webrtc/sum_3/)
5. ICE를 받은 B는 자신의 IP를 Server에게 전달한다.
6. IP의 위치에 따라 공통망인지, STUN, TURN을 사용할 것인지 결정후 통신이 시작된다.

* [WebRTC Signaling 이론](/webrtc/basicapp_02/) : 결국 [WebRTC 이론적 부분](/webrtc/basic/)과 비슷한 설명임.

### Janus WebRTC Server

* [Hyper-V 설치](/webrtc/janus/hyper-v/)
* [Janus Server 설치](/webrtc/janus/install/)

* [Janus 디버깅은?](/webrtc/janus/debug/)

### ffmpeg

* [기본적 기능 사용해보기](/webrtc/ffmepg/use/)

### 여기서 부터는 과거자료

### WebRTC란?

* [WebRTC 이론적 부분](/webrtc/basic/)

### 간단한 WebRTC 어플리케이션 만들어보기 - basicapp

#### Fetching

* Mediastream(`getUserMedia`)를 통해 영상/음성 정보를 web으로 가져온다.

* [WebRTC 기초 다지기](/webrtc/basicapp_01/) : nvm 설치 + `getUserMedia`으로 web에서 비디오/오디오 정보 받기

#### Signaling

* [복잡하기에 여기만 보자 대충 동작을 정리했음.](/webrtc/basic_arrange/)

* [socket.io 사용하기](/webrtc/basicapp_03/)
* [signaling 해보기](/webrtc/basicapp_04/) : `RTCSessionDescription`, `RTCIceCandidate`, `webkitRTCPeerConnection`사용
* [data channel 사용해보기](/webrtc/basicapp_05/) : 

나머지 사항은 Example인데 더 깊게 학습이 필요할 경우 진행할 것.

아래는 그리 중요하진 않은 사항

* [사전사항](/webrtc/basicapp_00/) : 소스클론 및 개발환경 세팅

#### 정리가 안되는거 같아서 중요한 부분만 정리한다.

우선 통신은 다음과 같은 방식으로 진행된다.

1. Fetching - getUserMedia를 이용 사용자의 영상/음성 정보를 가져온다.

```js
function startStream() {
navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
// ...

navigator.getUserMedia(constraints, onSuccess, onError);
}
```

2. (추가) Signaling Server 접속을 위한 io사용법

* 송신 : `app.io.room(req.data).broadcast()`
* 수신 : `io.on()`
* Signal : `io.emit()` -> slot : `app.io.route()`

```js
var express = require('express.io');
var app = express();
app.http().io();

// ...

app.io.route('ready', function(req) {
	req.io.join(req.data)
	app.io.room(req.data).broadcast('announce', {   // 상대방의 announce로 메시지 송신
		message: 'New client in the ' + req.data + ' room.'
	})
})

app.io.route('send', function(req) {
    app.io.room(req.data.room).broadcast('message', {
        message: req.data.message,
		author: req.data.author
    });
})

// ...

io = io.connect();
io.emit('ready', ROOM);   // io.route('read')로 signal을 보낸다.

io.on('announce', function(data) {  // announce메시지 수신 
  displayMessage(data.message);
});

io.on('message', function(data) {
  displayMessage(data.author + ": " + data.message);
});

sendMessage.addEventListener('click', function(ev){
  io.emit('send', {"author":myName.value, "message":myMessage.value, "room":ROOM});
  ev.preventDefault();
}, false);
```

3. Signaling - Client측에서 Signaling Server에 접속하여 통신하고자 하는 Client의 정보를 받는다.

* webkitRTCPeerConnection : Signal Server에게 내 정보를 송신(Offer SDP)
* RTCSessionDescription : 상대방의 정보를 수신 (Receive SDP)
* RTCIceCandidate : 후보군 정보를 수신

### WebRTC Native

* [WebRTC Native Build](/webrtc/native_build/)

* [Native Example - 1](/webrtc/native-example-1/) : WebRTC 내부 Example 실행해보기
* [Native Example - 2](/webrtc/native-example-2/) : Client내부 분석 1차
* [Native Example - 3](/webrtc/native-example-3/) : PeerConnection Client 측
* [Native Example - 4](/webrtc/native-example-4/) : PeerConnection Server 측

### 기타사항

* [WebRTC ejs, express에 관하여](https://8bitscoding.github.io/webrtc-ejs-express/)

### 미정리

* [WebRTC Signaling 실습(Setting Up Socket.io)](https://8bitscoding.github.io/webrtc-signaling2/)
* [WebRTC Signaling 실습(Implementing Signaling)](https://8bitscoding.github.io/webrtc-Implementing-Signaling/) : 설명은 생략, 코드를 살펴보면 됨. - 강의에서도 다른 코드를 보며 자세한 WebRTC Connection과정에 대해 설명하는데 필요할 경우 참고
* [WebRTC For Data Exchange(이론)](https://8bitscoding.github.io/webrtc-data-exchange/)

> cannot find module 'coffee-script'에 대해서<br>
> package.json 파일에 `"coffee-script": "~1.6.3"` 추가 후 `$ node update`