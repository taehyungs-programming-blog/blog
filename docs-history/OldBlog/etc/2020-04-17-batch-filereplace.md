---
title: "Windows Batch : *.rc파일 버전 병경하기"
permalink: batch/example-file-replace/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

```s
# my.rc
VS_VERSION_INFO     VERSIONINFO
  FILEVERSION       1,0,0,1     # 이 버전명을 변경하고자 한다.
  PRODUCTVERSION    1,0,0,1
 FILEFLAGSMASK VS_FFI_FILEFLAGSMASK
#ifdef _DEBUG
 FILEFLAGS VS_FF_DEBUG
#else
 FILEFLAGS 0x0L
#endif

# ...
```

```s
@echo off 
set "textFile=my.rc"
set "ver=111"

for /f "eol=; tokens=1,2,3,4,5,6 delims=, " %%i in (%textFile%) do (
    if "%%i"=="FILEVERSION" (
        echo %%m
    set s1=%%j,%%k,%%l,%%m
    set r1=%%j,%%k,%%l,%ver%
    )
)

set "search=%s1%"
set "replace=%r1%"

for /f "delims=" %%i in ('type "%textFile%" ^& break ^> "%textFile%" ') do (
    set "line=%%i"
    setlocal enabledelayedexpansion
    >>"%textFile%" echo(!line:%search%=%replace%!
    endlocal
)
```

* [참고사이트 : rc파일 버전변경](https://stackoverflow.com/questions/26008974/increment-the-version-in-rc-file-using-batch-script)
* [참고사이트 : 문자열 치환 예제2](https://stackoverflow.com/questions/23075953/batch-script-to-find-and-replace-a-string-in-text-file-without-creating-an-extra/23076141)
* [참고사이트 : batch file for문 옵션](https://www.snoopybox.co.kr/1366)
* [참고사이트 : 문자열 치환 예제1](http://blog.naver.com/PostView.nhn?blogId=xenecs&logNo=220941200392&beginTime=0&jumpingVid=&from=search&redirect=Log&widgetTypeCall=true)
