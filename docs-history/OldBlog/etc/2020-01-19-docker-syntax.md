---
title: "(Docker) Docker file 문법 정리"
date: 2020-01-19 00:00:00 -0000
---

> * [참고사이트](https://subicura.com/2017/02/10/docker-guide-for-beginners-create-image-and-deploy.html) : 중간쯤 가면 Dockerfile 기본 명령어가 있는데 거기서 부터 참고하면 됨.

---

### 목차

* [FROM](#FROM)
* [MAINTAINER](#MAINTAINER)
* [COPY](#COPY)
* [ADD](#ADD)
* [RUN](#RUN)
* [CMD](#CMD)

* [Docker File 실행](#Docker-File-실행)

---

### FROM

```s
FROM <image>:<tag>
FROM ubuntu:16.04
```

> 베이스 이미지를 지정

---

### MAINTAINER

```s
MAINTAINER <name>
MAINTAINER subicura@subicura.com
```

> 관리자 정보 입력

---

### COPY

```s
COPY <src>... <dest>
COPY . /usr/src/app
```

> 파일이나 디렉토리를 이미지로 복사합니다.<br>
> 일반적으로 소스를 복사하는 데 사용합니다. <br>target디렉토리가 없다면 자동으로 생성합니다.

---

### ADD

```s
ADD <src>... <dest>
ADD . /usr/src/app
```

> COPY의 강화버전 <br>
> COPY명령어와 매우 유사하나 몇가지 추가 기능이 있습니다. <br>
> src에 파일 대신 URL을 입력할 수 있고 src에 압축 파일을 입력하는 경우 자동으로 압축을 해제하면서 복사됩니다.

---

### RUN

```s
RUN <command>
RUN ["executable", "param1", "param2"]
RUN bundle install
```

> 가장 많이 사용하는 구문입니다. 명령어를 그대로 실행합니다. <br>
> 내부적으로 /bin/sh -c 뒤에 명령어를 실행하는 방식입니다.

---

### CMD

```s
CMD ["executable","param1","param2"]
CMD command param1 param2
CMD bundle exec ruby app.rb
```

> 도커 컨테이너가 실행되었을 때 실행되는 명령어를 정의합니다. <br>
> 빌드할 때는 실행되지 않으며 여러 개의 CMD가 존재할 경우 가장 마지막 CMD만 실행됩니다. <br>
> 한꺼번에 여러 개의 프로그램을 실행하고 싶은 경우에는 run.sh파일을 작성하여 데몬으로 실행하거나 supervisord나 forego와 같은 여러 개의 프로그램을 실행하는 프로그램을 사용합니다.

---

> 기타등등... 필요한 부분은 참고사이트 보고 다시 정리.

---

### Docker File 실행

> docker file이 있는 폴더에서

```s
$ docker build -t <docker_name> .
```

> 참고로 -t 옵션은 도커이름을 선언하는 명령어이다.

> 가장 좋은 예시는 도커 허브의 파일이다. 같이보자.

![](/file/image/docker-syntax_Image_01.png)

> 내부를 보면...

![](/file/image/docker-syntax_Image_02.png)