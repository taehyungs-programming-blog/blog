---
layout: default
title: "1. NPM - init/install"
parent: "WebPack"
grand_parent: "Vue.JS"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

🛵 **Node.js** - 브라우저 밖에서도 자바스크립트를 실행할 수 있는 환경구축<br>
🛵 **NPM**(Node Package Manager) - 자바스크립트에서 사용되는 패키지 관리자<br>

```bash
# 버전확인
$ node -v
$ npm -v
```

```bash
# npm 초기화
# -y : 자동 yes
$ npm init -y

# 패키지 설치
$ npm install {패키지}              # dependencies(배포용 라이브러리) 에 설치됨
$ npm install {패키지} --save-dev   # devDependencies(개발용 보조 라이브러리) 에 설치됨
# 배포시에는 devDependencies가 포함되지 않는다.(만약 모두 devDependencies에 넣으면 속도에 문제가 발생)
# 패키지 삭제
$ npm uninstall {패키지}

# 패키지 설치(전역설치)
$ npm install {패키지} --global

# 만약 package.json이 있고 그냥 설치만 해야한다면?
$ npm install
```

🛵 **package.json**파일이 생성된다.

```json
{
  "name": "npm_package",
  "version": "1.0.0",
  "description": "",
  "main": "index.js",
  "scripts": {
    "test": "echo \"Error: no test specified\" && exit 1"
  },
  "author": "",
  "license": "ISC"
}
```

🛵 (질문) 꼭 npm을 써야할까? `<script src=""></script>`(CDN방식)를 통해서 js를 가져오면 안될까? -> Nope! 패키지를 코드에서 관리하면 패지키 버전관리나 어떤 패키지를 사용중인지 관리가 어려워 진다.<br>
🛵 추가적을 CDN방식을 이용시 매번 url주소를 찾아야하는 수고로움이 있다. 패키지 매니저는 그냥 `npn install {패키지}`로 간단하게 설치가능<br>

{% endraw %}
