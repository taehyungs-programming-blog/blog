---
title: "(Docker) Linux설치 후 WebDav Server설치기"
date: 2020-01-19 00:00:00 -0000
---

> * [참고사이트](https://alexonepath.github.io/category/docker/guide/container-1-home.html)

---

### 목차

* [구축해 볼 것](#구축해-볼-것)
* [WebDAV](#WebDAV)
* [](#)
* [](#)

---

### 구축해 볼 것

* WebDav(Web Distributed Authoring and Versioning) : 웹서버를 저장소로 사용 (저장된 문서, 파일 읽기 및 편집가능.)
* Transmission : 토렌트
* Plex : 동영상 스트리밍 서버

---

### WebDAV

> dockerfile 생성

```dockerfile
FROM ubuntu:18.04
MAINTAINER <your email>

ENV USERNAME ""
ENV USERPWD ""

RUN apt-get update && apt install -y nginx nginx-extras apache2-utils

RUN echo 'server {\n\
listen 80 default_server;\n\
listen [::]:80 default_server;\n\
charset utf-8;\n\
location /webdav {\n\
alias /var/webdav;\n\
dav_methods     PUT DELETE MKCOL COPY MOVE;\n\
dav_ext_methods   PROPFIND OPTIONS;\n\
create_full_put_path  on;\n\
dav_access    user:rw group:rw all:rw;\n\
autoindex     on;\n\
auth_basic "restricted";\n\
auth_basic_user_file /etc/nginx/users.pass;\n\
send_timeout  36000s;\n\
proxy_connect_timeout  36000s;\n\
proxy_read_timeout  36000s;\n\
proxy_send_timeout  36000s;\n\
proxy_request_buffering off;\n\
}}\n'\
>>  /etc/nginx/sites-available/webdav

RUN ln -s /etc/nginx/sites-available/webdav /etc/nginx/sites-enabled/webdav
RUN rm -rf /etc/nginx/sites-enabled/default

RUN mkdir -p /var/webdav
RUN chown www-data /var/webdav && chmod -R 777 /var/webdav

COPY entrypoint.sh /
RUN chmod +x entrypoint.sh

CMD /entrypoint.sh && nginx -g "daemon off;"
```

> d 생성

```sh
#!/bin/bash

if [[ -n "$USERNAME" ]] && [[ -n "$USERPWD" ]]
then
    htpasswd -bc /etc/nginx/users.pass $USERNAME $USERPWD
	echo Done.
fi
```

> dockerfile build!

```s
$ docker build --tag <image_name>:1.0 .
# 꼭 허브에 푸쉬 하지 않아도 됨.
$ docker login
$ docker push <image_name>:1.0
```

> 컨테이너 실행

```s
# volume 매핑을 위한 디렉토리 생성
$ sudo mkdir -p ./webdav && sudo chmod 777 ./webdav
```

```s
# 실행시 사용자 생성
$ sudo docker run -d --name <container_name> \
--restart=always \
-p 20000:80 \
-v /Users/taehyung/Documents/docker/docker-web-server/webdav-data:/var/webdav \
-e USERNAME=<user id> \
-e USERPWD=<user pw> \
<image_name>:1.0
```

> 실행 후 docker process확인

![](/file/image/docker-linux-webdav_Image_01.png)

```s
# 사용자 생성
$ htpasswd -bc users.passwd YOUR_NAME YOUR_PWD
$ htpasswd -b users.passwd YOUR_NAME YOUR_PWD
```

```s
# 실행중인 webdav에 사용자 추가
$ docker exec ${CONTAINER_NAME} htpasswd -b /etc/nginx/users.pass YOUR_NAME YOUR_PWD
```

> 접속해 보자.

> http://localhost:20000/webdav

![](/file/image/docker-linux-webdav_Image_02.png)

> 잘 접속됨을 확인