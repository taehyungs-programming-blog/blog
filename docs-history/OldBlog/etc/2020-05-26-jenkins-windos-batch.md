---
title: "Jenkins : Window Batch 작성하기"
permalink: jenkins/windows-batch/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-26 00:00:00 -0000
last_modified_at: 2020-05-26 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Jenkins
category:
  - build
  - batch
  - example
header:
  teaser: /file/image/etc-page-teaser.gif
---

아래 Example은 .rc 파일을 읽어서 버전관리하는 batch 파일이다.

```batch
@echo off
:: 버전관리 파일 변수 지정
:: xcopy를 사용하기 위한 PATH 재 지정, Jenkins내부에는 PATH가 지정이 안된 경우가 있음.
set PATH=%PATH%;C:\Windows\SysWOW64
cd /d "%WORKSPACE%\obs\build\"
set "clone-obsrc-file=obs.rc"
:: local에서 관리중인 rc파일을 지정
set "obsrc_local_space=D:\obsrc-file"
set "local-obsrc-file=obs-local.rc"

:: obsrc local에서 관리중인 파일을 가져온다.
del "%local-obsrc-file%"
xcopy %obsrc_local_space%\%local-obsrc-file% .
:: clone 받은 obs.rc파일을 읽는다.
for /f "eol=; tokens=1,2,3,4,5,6 delims=, " %%i in (%clone-obsrc-file%) do (
    if "%%i"=="FILEVERSION" (
        set s1=%%j,%%k,%%l,%%m
        set r11=%%j,%%k,%%l
        set mink_BUILD_CNT=%%j_%%k_%%l_
    )
)
:: 기존에 obs.rc파일을 읽는다.
for /f "eol=; tokens=1,2,3,4,5,6 delims=, " %%i in (%local-obsrc-file%) do (
    if "%%i"=="FILEVERSION" (
        set s2=%%j,%%k,%%l,%%m
        set r22=%%j,%%k,%%l
        set s22=%%m
    )
)
:: 버전의 변경이 있는지 확인한다.
if "%r11%"=="%r22%" (
    :: 버전의 변경이 없을경우 빌드카운트만 올린다.
    set /a s22+=1
) else (
    :: 버전 변경이 있는 경우 빌드카운트를 1로 초기화 한다.
    set s22=1
)
::Jenkins Batch Command는 for, if문에서 set이 잘 안먹는 경우가 있어서 여기서 선언함
set r1=%r11%,%s22%
set mink_BUILD_CNT=%mink_BUILD_CNT%%s22%
:: 빌드카운트 업데이트 obs.rc파일에 적용
set "search=%s1%"
set "BUILD_CNT=%r1%"
for /f "delims=" %%i in ('type "%clone-obsrc-file%" ^& break ^> "%clone-obsrc-file%" ') do (
    set "line=%%i"
    setlocal enabledelayedexpansion
    >>"%clone-obsrc-file%" echo(!line:%search%=%BUILD_CNT%!
    endlocal
)

:: 내부적으로 관리하는 obs.rc파일을 저장한다.
del "%obsrc_local_space%\%local-obsrc-file%"
xcopy %clone-obsrc-file% %obsrc_local_space%\%local-obsrc-file%*
del "%local-obsrc-file%"
```