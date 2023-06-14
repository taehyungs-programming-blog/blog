---
title: "(Docker) mac기반 Docker 기본적 사용법"
date: 2020-01-19 00:00:00 -0000
---

### 참고사이트

* [docker강의(유투브)](https://www.youtube.com/watch?time_continue=6&v=Bhzz9E3xuXY&feature=emb_logo)
* [docker강의(사이트)](http://pyrasis.com/Docker/Docker-HOWTO)

---

### 목차

* [Ubuntu image 실행해 보기](#Ubuntu-image-실행해-보기)
* [image검색](#image검색)
* [컨테이너 실행](#컨테이너-실행) : run
* [컨테이너 실행2](#컨테이너-실행2) : start
* [실행중인 컨테이너 접속](#실행중인-컨테이너-접속)
* [컨테이너 자체를 삭제하기](#컨테이너-자체를-삭제하기)
* [이미지를 삭제하기](#이미지를-삭제하기)
* [(추가) 특정한 어플리케이션이 설치된 이미지를 받아와 사용이 가능할까?](#(추가)-특정한-어플리케이션이-설치된-이미지를-받아와-사용이-가능할까?)

---

> 우선 기본적으로 docker는 리눅스를 기반으로 하기에 호스트 서버는 리눅스를 추천한다.

---

> * [TOP](#목차)

### Ubuntu image 실행해 보기

> 우선 현재 설치된 이미지를 확인해 보자.

```s
$ docker images
``` 

![](/file/image/docker-mac-basic_Image_01.png)

> 이미지가 아무 것도 없음! 제일 위에 REPOSITYORY TAG IMAGE ID 등등은 메뉴임

> 우분투를 받아와 보자

```s
$ docker pull ubuntu:14.04
```

```s
$ docker images
```

![](/file/image/docker-mac-basic_Image_02.png)

> 이미지를 잘 받아온 모습을 볼 수 있다.

> 참고로 설치할 수 있는 이미지 검색은 다음과 같은 방법으로 할 수 있다.

---

> * [TOP](#목차)

### image검색

```s
$ docker search ubuntu
```

![](/file/image/docker-mac-basic_Image_03.png)

> docker허브에 올라온 이미지들이고,
>
> 앞에 아이디가 없는 것들은 공식이미지인데 ... 이런방식은 찾기가 힘들다.
>
> docker허브에서 확인해 보자.

* [docker허브 사이트](https://hub.docker.com/?utm_source=docker4mac_2.1.0.5&utm_medium=hub&utm_campaign=referral)

![](/file/image/docker-mac-basic_Image_04.png)

> 이미지를 실행한 개념을 컨테이너라 하며 지금부터 컨테이너를 실행해 본다.

---

> * [TOP](#목차)

### 컨테이너 실행

```s
$ docker run -i -t ubuntu14:04 /bin/bash
```

> 참고로 옵션은 -i : 사용자의 입/출력을 받는다.
>
> -t : 가상터미널 환경을 에뮬레이션
>
> 일반적인 케이스는 두 옵션을 모두 넣는다고 생각하자.

![](/file/image/docker-mac-basic_Image_05.png)

> docker를 종료하고 싶다면?

```s
$ exit
```

> 종료를 확인하고 싶다면?

```s
$ docker ps -a
```

![](/file/image/docker-mac-basic_Image_06.png)

---

> * [TOP](#목차)

### 컨테이너 실행

> docker를 종료하지 않고 실행된 상태에서 나오고 싶다면?

> 우선 docker를 또 다른 방법으로 실행해 보자.

```s
$ docker start <docker_name>
```

> run은 실행과 동시에 들어가고 start는 들어가진 않는다.

> 실행을 확인해 보자.

```s
$ docker ps
```

![](/file/image/docker-mac-basic_Image_07.png)

---

> * [TOP](#목차)

### 실행중인 컨테이너 접속

> 실행중인 이미지에 들어가보자.

```s
$ docker attach <docker_name>
```

> 참고로 exit로 나가게 되면 컨테이너도 종료되기에 
>
> Ctrl+P, Ctrl+Q를 통해 나가야 컨테이너가 종료되지 않는다.

---

> * [TOP](#목차)

### 실행중인 컨테이너 종료

```s
$ docker stop <image_name>
```

---

> * [TOP](#목차)

### 컨테이너 자체를 삭제하기

```s
$ docker rm <image_name>
```

### 이미지를 삭제하기

```s
$ docker rmi <image_name>
```

---

> * [TOP](#목차)

### (추가) 특정한 어플리케이션이 설치된 이미지를 받아와 사용이 가능할까?

> nginx를 예로들어서 설명

```s
$ docker pull nginx:latest
```

![](/file/image/docker-mac-basic_Image_08.png)

```s
$ docker run -d --name hello-nginx -p 8000:80 nginx:latest
```

> -d : 백그라운드에서돌리고
>
> --name : 이름을 지정한다.
>
> -p : 포트포워딩을 한다.
