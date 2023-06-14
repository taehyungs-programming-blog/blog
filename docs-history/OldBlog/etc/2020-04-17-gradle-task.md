---
title: "Gradle : task"
permalink: gradle/task/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-17 00:00:00 -0000
last_modified_at: 2020-04-17 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

## 1

* [강의](https://www.youtube.com/watch?v=bfZ88Omjf-E&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=8)

```groovy
// build.gradle
def strMsg = {println 'Hello Gradle!'}

task hello{}
hello.leftShift(strMsg)
```

![](/file/image/gradle-task01.png)

```groovy
// build.gradle
task gradleTask1{
    println 'This is Gradle Task One'
}

task gradleTask2{
    doLast{
        println 'This is Gradle Task Two'
    }
}
```

![](/file/image/gradle-task02.png)

* Task에서 doLast로 선언된 부분이 출력됨. -> 실행단계에서 출력
* gradleTask1은 설정단계에서 출력

```groovy
// build.gradle
task goodTask{
    doLast{
        println description + 'This is Good!'
    }
}

goodTask.description = 'Task Execution->'
```

![](/file/image/gradle-task03.png)

```groovy
// build.gradle
goodTask.description = 'Task Execution->'

task badTask{
    doLast{
        println description + 'This is Good!'
    }
}
```

* 해당 Task를 정의하기 전이라 에러가 발생한다.

![](/file/image/gradle-task04.png)

```groovy
// build.gradle
task prevTask

prevTask.description = '<Task Execution>'

prevTask{
    doLast{
        println description + 'This is Good!'
    }
}
```

* task를 미리 정의하면 문제를 해결할 수 있다.

![](/file/image/gradle-task05.png)

---

## 2

* [강의](https://www.youtube.com/watch?v=1Z5PpI3g7ss&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=9)

```groovy
// build.gradle
task exeTask1{
  doLast{
    String strOutput = 'Have a Good Day'
    println '1. string change : ' + strOutput.toUpperCase()
    println '2. string change : ' + strOutput.toLowerCase()
  }
}

task exeTask2{
  doLast{
    10.times{println "$it"}   // 반복문
  }
}
```

* gradle의 기능이라기 보단 groovy언어 자체의 기능이다

![](/file/image/gradle-task06.png)

```groovy
// build.gradle

3.times{
counter->
    task "exeTask$counter"{
        doLast{
        println "task counter : $counter"
        }
    }
}

exeTask1.dependsOn exeTask0,exeTask2
```

![](/file/image/gradle-task07.png)

```groovy
// build.gradle
task exeTask(description : "This is gradle description") {
  doLast{
    println description
  }
}

// 위와 같은 표현이다
// task (exeTask,description : "This is gradle description"){
//  doLast{
//    println description
//  }
//}
```

![](/file/image/gradle-task08.png)

```groovy
// build.gradle
task exeTask{
  doLast{
    println 'exeTask task'
  }
}

exeTask.doFirst{
  println '>>> exeTask doFirst'
}

exeTask.doLast{
  println '>>> exeTask doLast - END : $exeTask.name'
}
```

![](/file/image/gradle-task09.png)

```groovy
// build.gradle
task userInfo{
  ext.userName = "John"
  ext.userAge = "20"
  ext.userGen =  "Man"
}

task exeTask{
  doLast{
    println "Name : " + userInfo.userName
    println "Age : " + userInfo.userAge
    println "Gen : " + userInfo.userGen
  }
}
```

![](/file/image/gradle-task10.png)

```groovy
defaultTasks 'exeTask1','exeTask2','exeTask3'

task exeTask1{
  doLast{
    println 'This is exeTask1 Project'
  }
}

task exeTask2{
  doLast{
    println 'This is exeTask2 Project'
  }
}

task exeTask3{
  doLast{
    println 'This is exeTask3 Project'
  }
}
```

![](/file/image/gradle-task11.png)

```groovy
// build.gradle
def confMap = ['imgConf' : 'img.freelec.co.kr' , 'smsConf':'sms.freelec.co.kr']

confMap.each { svDomain, domainAddr->
  task "exeTask${svDomain}"{
    doLast{
      println domainAddr
    }
  }
}
```

![](/file/image/gradle-task12.png)

---

## 3

* [강의](https://www.youtube.com/watch?v=faKRLf2T3VY&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=10)

```groovy
task exeTask{
  doLast{
    println 'Gradle Build Success !!!'
  }
}

exeTask.onlyIf{
  buildType == 'partial-build'
}
```

* 그냥 빌드하면 에러출력

![](/file/image/gradle-task13.png)

![](/file/image/gradle-task14.png)

```groovy
// build.gradle
task exeTask{
  doLast{
    println 'exeTask Build SUCCESS'
  }
}

exeTask {
  doLast{
    if(process == 'error'){
      throw new StopExecutionException()
    }
  }
}

exeTask{
  doLast{
    println '-- Build END--'
  }
}
```

* process를 ok로 둔다면 정상빌드

![](/file/image/gradle-task15.png)

* process를 error로 둔다면 throw가 되며 Build END가 출력안됨.

![](/file/image/gradle-task16.png)

---

## 4

* [강의](https://www.youtube.com/watch?v=m0WEUVrZ_zU&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=11)


```groovy
task exeTaskBefore{
  doLast{
    println 'exeTaskBefore ---- 1'
  }
}

task exeTaskAfter{
  doLast{
    println 'exeTaskAfter ---- 1'
  }
}

exeTaskAfter.mustRunAfter exeTaskBefore
```

* exeTaskBefore를 수행 후 exeTaskAfter를 실행해 달라

![](/file/image/gradle-task17.png)

```groovy
task exeTaskBefore{
  doLast{
    println 'exeTaskBefore ---- 1'
  }
}

task exeTaskAfter{
  doLast{
    println 'exeTaskAfter ---- 1'
  }
}

exeTaskAfter.shouldRunAfter exeTaskBefore
```

* mustRun과 뭔 차이지?? : 동일하다고 생각하자, 예외문제 뭐 등등 있는데 ... 아직은 이해하기 이르다

![](/file/image/gradle-task18.png)