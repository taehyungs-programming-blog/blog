---
title: "Gradle : 파일관리"
permalink: gradle/files/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-19 00:00:00 -0000
last_modified_at: 2020-04-19 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [강좌](https://www.youtube.com/watch?v=GgZO3sLiKRg&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=17)


## 파일 열기

```groovy
// build.gradle
File refFile1 = file('src/refJavaFile.java')
File refFile2 = file(new File('src/refExeFile.txt'))

task exeTask{
    doLast{
        println 'Java File Path : ' + refFile1.absolutePath
        println 'Txt File Path : ' + refFile2.path
    }
}
```

![](/file/image/gradle-files-01.png)

* url경로도 참조가능함을 기억(uri, closer 모두 가능)

```groovy
File urlRef = null
URL url = new URL('file:/urlRef.html')
urlRef = file(url)
```

---

## 다중 파일 참조

* FileCollection
    * 역시 url, uri, closer 모두 사용 가능

```groovy
// build.gradle
FileCollection fileCollection = files('index.txt', 'intro.txt')

println 'collection files : ' + fileCollection[0].path
println 'collection files : ' + fileCollection[1].path

fileCollection.each{
    println it.name + " : " + it.path
}
```

![](/file/image/gradle-files-02.png)

```groovy
// build.gradle
FileCollection fileCollection = files('settings.txt',new File('intro.txt'),new File('index.txt'),new File('log.txt'))

// List형으로 변환
List fileList = fileCollection as List
fileList.each{
    println 'List collection : ' + it.path
}

// Set형으로 변환
Set fileSet = fileCollection as Set
fileSet.each{
    println 'Set collection : ' + it.path
}

// 배열로 변환
File[] fileArray = fileCollection as File[]
fileArray.each{
    println 'Array collection : ' it.path
}
```

```groovy
// build.gradle
FileCollection fc1 = files('log.txt')
FileCollection fc2 = fc1 + files('intro.txt')

if(fc2.files.size() == 2){
    println 'fc2 file count : ' + fc3.files.size()

    fc2.each{
        println 'fc2 file => ' + it.name + " : " + it.path
    }
}
```

---

## 파일 필터링

```groovy
// build.gradle
FileCollection fileCollection = files('settings.txt','login.txt',new File('intro.java'),new File('index.html'),new File('log.txt'))

println 'All File Size : ' + fileCollection.files.size()

// 텍스트 파일에 대하여 필터링
FileCollection txtFilter1 = fileCollection.filter{collectionFile->
    collectionFile.name.endsWith '.txt'
}

println 'File Filter Size : ' + txtFilter1.files.size()

// 필터링된 파일 내용 출력
txtFilter1.each{
    println 'txtFilter1 file => ' + it.name + " : " + it.path
}
```

![](/file/image/gradle-files-03.png)

---

## FileTree

* FileTree

```groovy
FileTree fileTree = fileTree('settings')

fileTree.each{
    println 'File Name : ' + it.name + ' , Path : ' + it.path
}
```

```groovy
// 클로저를 이용한 FileTree 객체 생성
FileTree fileTreeEx = fileTree('src') {
    include '**/*.java'
}

// src 디렉터리를 기준으로 참조중인 파일 내용 출력
fileTreeEx.each{
    println 'fileTree Name : ' + it.name
}
```

```groovy
// src 디렉터리를 기준으로 트겅 파일 제외
fileTreeEx = filetree('src'){
    exclude '**/action????.java'
}

// src 디렉터리를 기준으로 참조중인 파일 내용 출력
fileTreeEx.each{
    println 'fileTree Name : ' + it.name
}
```

```groovy
// match를 이용한 파일 참조
FileTree fileMatch = fileTree('src')

FileTree ft1 =fileMatch.matching(
    include '**/*.java'
)

ft1.each{
    println 'File Name : ' + it.name
}
```

---

## copy

```groovy
copy{
    from 'src/com/org/file' // 대상 파일의 경로
    into 'src/com/des/file' // 목적지 경로
}
```

```groovy
copy{
    from 'src/com/org/file' // 대상 파일의 경로
    into 'src/com/des/file' // 목적지 경로

    include '**/*.java'
    exclude '**/*Dao.java'

    includeEmptyDirs = ture     // 빈 디렉터리도 복사(기본)

    rename 'originalTest.java','destination.java'
    rename '(.*)Test.java','changeName.java'
}
```

```groovy
// 파일 내부내용을 편집해서 복사도 가능
copy{
    from('src/com/org/file'){
        include '**/original.java'
        rename 'original.java','editCloser.java'
    }
    into 'src/com/edit/file'

    filter{line->
        line.replaceAll 'com.org.file','com.edit.file'
        // com.org.file라는 내용이 있으면 com.edit.file로 변화
    }

    filter{line->
        line.replaceAll 'original','editCloser'
    }
}
```

### copy task로 복사하기

```groovy
task copyTask(type: Copy) {
    from('src/com/org/file'){
        include '**/original.java'
        rename 'original.java','editCloser.java'
    }

    into 'src/com/edit/file'

    filter{line->
        line.repalceAll 'com.org.file','com.edit.file'
    }
    filter{line->
        line.replaceAll 'original','editCloser'
    }
}
```

---

## delete

```groovy
delete 'src/com/org/file/original.java'

task delFile(type: Delete){
    delete 'src/com/org/file/original.java'
    followSynlinks = true
}
```

