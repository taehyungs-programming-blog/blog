---
title: "Gradle : 특정 폴더에 파일만 복사하기"
permalink: gradle/example/copyfiles/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-29 00:00:00 -0000
last_modified_at: 2020-04-29 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

```groovy
def copy_symbol_files = {
    delete files('D:/output-folder')

    copy {
        includeEmptyDirs = false
        from fileTree("D:/build").include("**/*.pdb").files
        into "D:/output-folder"
    }

    copy {
        from "D:/build/build.exe"
        into "D:/output-folder"
    }
}

task buildTask{
    println ' /********************* file copy_symbol_files() start *********************/ '
    copy_symbol_files()
    println ' /********************* file copy_symbol_files() end *********************/ '
}
```