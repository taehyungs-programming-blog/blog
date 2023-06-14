---
title: "Gradle : task graph"
permalink: gradle/task-graph/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-18 00:00:00 -0000
last_modified_at: 2020-04-18 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [강좌](https://www.youtube.com/watch?v=Q_4B94LvED8&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=13&t=0s)

```groovy
task FirstExeTask{
    doLast{
        println 'FirstExeTask gradle task'
    }
}

task SecondExeTask(dependsOn: 'FirstExeTask') {
    doLast{
        println 'SecondExeTask gradle task'
    }
}

task ThirdExeTask(dependsOn: 'SecondExeTask') {
    doLast{
        println 'ThirdExeTask gradle task'
    }
}
```

![](/file/image/gradle-task-graph.png)

```groovy
// build.gradle
task FirstExeTask{
    doLast{
        println 'FirstExeTask gradle task'
    }
}

FirstExeTask{
    doLast{
        throw new Exception('Exception')
    }
}

task SecondExeTask(dependsOn: 'FirstExeTask'){
    doLast{
        println 'SecondExeTask gradle task'
    }
}

task ThirdExeTask(dependsOn : 'SecondExeTask'){
    doLast{
        println 'ThirdExeTask gradle task'
    }
}

task finishTask {
    doLast{
        println 'memory clear'
    }
}

FirstExeTask.finalizedBy finishTask
```

* exception이 발생해도 memory clear가 호출됨을 볼 수 있다.

![](/file/image/gradle-task-graph2.png)

