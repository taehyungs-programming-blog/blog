---
layout: default
title: "2. 파일 다루기"
parent: (AWS)
grand_parent: C#
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 한글이 가능하게 설치

🍓 Putty로 접속해도 상관은 없음

```bash
$ sudo yum -y install ibus-hangul
```

🍓 참고로 리눅스 자체에서 한글이 입력되게 하기 위해선 아래와 같이 한글(hangul)로 변경해 줘야함.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-2-1.png"/>
</p>

* `dev` - 장치 파일
* `home` - 사용자 홈 디렉터리
* `root` - root 계정의 홈 디렉터리
* `tmp` - 임시 파일
* `usr` - 기본 실행 파일, 라이브러리 등

```bash
$ ls -F /
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/aws/aws-2-2.png"/>
</p>

🍓 `/` - 는 디렉터리를 의미하며 , `@` - 실행파일 연결링크(심볼릭)를 의미한다.

* `ls` 옵션
    * `-a` - 숨김 파일 모두 표현
    * `-l` - 파일 상세 정보 출력
    * `-F` - 파일의 종류 표현
    * `-R` - 하위 디렉터리 목록 표

---

## 폴더 생성/제거

```bash
$ mkdir {폴더명}
$ mkdir -p {경로/폴더명}  # 경로로 이동후 생성

$ rmdir {폴더명}
$ rmdir -p {경로/폴더명}
```

---

## 파일내용 출력

```bash
# 일반출력(쭉 보여줌)
$ cat {파일명}

# space(다음화면) / enter(다음행) / q(종료)
$ more {파일명}

# 스크롤로 파일 출력
$ less {파일명}
```

---

## 바로가기 만들기

```bash
# 하드 링크(완전히 동일한 값(참조를 만든다 생각))
$ ln {원본} {링크}

# 심볼릭 링크
$ ln -s {원본} {링크}
```

---

## 파일내용 검색

```bash
$ grep {옵션} {검색내용} {파일}
# -n : 줄번호 출력
# -i : 대소문자 구분
```
