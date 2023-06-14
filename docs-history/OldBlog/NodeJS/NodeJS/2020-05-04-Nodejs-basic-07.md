---
title: "NodeJS Basic 7강 - URL로 입력된 값 사용하기"
permalink: NodeJS/basic-07/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-04 00:00:00 -0000
last_modified_at: 2020-05-04 00:00:00 -0000
header:
  teaser: /file/image/nodejs-page-teaser.gif
sidebar:
  title: "NodeJS"
  nav: NodeJS
---

* [강의](https://opentutorials.org/course/3332/21046)

![](/file/image/Nodejs-7_image1.png)

웹의 기본적 동작은 `주소/html주소`를 통해 html주소를 보고 찾아가는 식이다.<br>
그렇다면 매 페이지마다 html파일을 새로 만들어줘야한다는 말인가? 네이버는 그럼 몇천개의 html을 보관중이란말인가??<br>
물론 이런식으로 동작하진않는다.

이것을 이해하기 위해서 URL을 좀더 이해해야한다.

```s
http://localhost:3000/main/?id=HTML&page=12
# http: protocol
# localhost: host(domain name) 컴퓨터를 가리키는 주소를 의미
# 3000: port num, 호스트(컴퓨터)의 포트넘버를 의미하겠지?
# main: 단순 path이다.
# ?id=HTML: Query string이라한다. (설명은 아래)
```

## Query string

물음표 뒤의 `id=HTML` 부분을 Query string이라 하는데 이번에 살펴볼 부분이다.<br>
**Query string**에 따라 다른 페이지를 보여주게 만들어 본다.<br>
보여줄수 있는 방법은 Query string을 통해서 웹서버에게 명령을 내릴수 있다. 특정 html을 보여달라 이런식!<br>
`?id=HTML&page=12` ? = & 등이 들어가는데 Query string의 통신규약이기에 받아들여야한다.

```js
// main.js
var http = require('http');
var fs = require('fs');
var app = http.createServer(function(request,response){
    var url = request.url;
    
    console.log(url); // url을 뽑아본다.

    if(request.url == '/'){
        url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    response.end(fs.readFileSync(__dirname + url));

});
app.listen(3000);
```

코드를 위와 같이 작성하고 `http://localhost:3000/?id=CSS`로 접속을해보면 결과는 아래와 같다.

![](/file/image/Nodejs-7_image2.png)

하고싶은 말은 url에서는 `/?id=CSS` query string을 읽으려 한다는 것이다. 

어떻게 해야할까??

```js
// main.js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    
    console.log(queryData); // url을 뽑아본다.

    if(request.url == '/'){
        _url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    response.end(fs.readFileSync(__dirname + url));

});
app.listen(3000);
```

![](/file/image/Nodejs-7_image3.png)

```js
// main.js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    
    console.log(queryData.id); // id값을 얻어온다.

    if(request.url == '/'){
        _url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    response.end(fs.readFileSync(__dirname + url));

});
app.listen(3000);
```

![](/file/image/Nodejs-7_image4.png)

```js
// main.js
var http = require('http');
var fs = require('fs');
var url = require('url');

var app = http.createServer(function(request,response){
    var _url = request.url;
    var queryData = url.parse(_url, true).query;
    if(request.url == '/'){
        _url = '/index.html';
    }
    if(request.url == '/favicon.ico'){
        return response.writeHead(404);
    }
    response.writeHead(200);
    response.end(queryData.id);
    // id값을 적용해 본다.

});
app.listen(3000);
```

![](/file/image/Nodejs-7_image5.png)

![](/file/image/Nodejs-7_image6.png)

요청한 쿼리에 따라 다른 정보를 출력하게 만듦