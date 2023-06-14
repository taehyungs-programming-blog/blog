---
title: "Jenkins : Window Batch Good Example : MSBuild"
permalink: jenkins/windowbatch-goodexample/                # link 직접 지정
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
# 폴더이동
cd /d "D:\folder"
# 환경변수 설정
set PATH=%PATH%;C:\Program Files (x86)\CMake\bin;
# 외부 배치파일 실행
call "D:\build.bat"
# 파일삭제
del /f "D:\Release.gitignore"
# 폴더삭제
rmdir /s /q "D:\bin"
# 폴더복사
xcopy /e /h /k /Y D:\\Release*.* D:\output
# 특정 파일명 검색후 복사
for /r . %%f in (./*.pdb) do @xcopy "%%f" D:\folder /Y
```

```s
MSBuild.exe target.sln -target:projectname -p:Configuration=Release
```