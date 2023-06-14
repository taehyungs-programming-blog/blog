---
title: "Gradle : 도메인 객체"
permalink: gradle/object/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-18 00:00:00 -0000
last_modified_at: 2020-04-18 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
---

* [강좌](https://www.youtube.com/watch?v=hbZJPhceVg4&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=13)

## Project 객체

* 프로젝트의 환경 구성, 의존관계, 태스크 등의 내용을 제어 및 참조
* build.gradle과 대응
* Project 객체의 생명주기
    * 빌드 수행을 위한 Settings 객체 생성
    * settings.gradle 스크립트 파일이 있을 경우 Settings 객체 비교
    * 구성된 Settings 객체를 이용하여 Project 객체 계층 구조 생성
    * 멀티 프로젝트일 경우 부모 프로젝트 부터 Project 객체 생성 후 자식 프로젝트의 Project 객체 생성

* Project Object
    * TaskContainer
    * DependencyHandler
    * ArtifactHandler
    * RepositoryHandler
    * Gradle
    * ConfigurationContainer

```groovy
// build.gradle
defaultTasks = ['exeTask001', 'exeTask002']

project.description = 'Project Object Description'
project.version = 'exeTask v1.0'
task exeTask001{
    doLast{
        println 'Project Name : ' + project.name
        println 'Project description : ' + project.description
        println 'Project group : ' + project.group
        println 'Project path : ' + project.path
        println 'Project projectDir : ' + project.projectDir
        println 'Project status : ' + project.status
        println 'Project state : ' + project.state
        println 'Project version : ' + project.version
    }
}

task exeTask002{
    doLast{
        println project.description
    }
}
```

![](/file/image/gradle-object-01.png)

```groovy
// build.gradle
defaultTasks = ['exeTask001', 'exeTask002']

gradle.allprojects{
    project->
        project.beforeEvaluate{
            println project.name + ' : check start'
        }
        project.afterEvaluate{
            println project.name + ' : check end'
        }
}

project.description = 'Project Object Description'
project.version = 'exeTask v1.0'
task exeTask001{
    doLast{
        println 'Project Name : ' + project.name
        println 'Project description : ' + project.description
        println 'Project group : ' + project.group
        println 'Project path : ' + project.path
        println 'Project projectDir : ' + project.projectDir
        println 'Project status : ' + project.status
        println 'Project state : ' + project.state
        println 'Project version : ' + project.version
    }
}

task exeTask002{
    doLast{
        println project.description
    }
}
```

```groovy
// settings.gradle
gradle.allprojects{
    project->
        project.beforeEvaluate{
            println project.name + ' : check start'
        }
        project.afterEvaluate{
            println project.name + ' : check end'
        }
}
```

![](/file/image/gradle-object-02.png)

---

* [강좌](https://www.youtube.com/watch?v=42UQL2DvkXs&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=14)

## task 객체

* Action 객체를 생성하여 구성
* 내부적으로 execute() API를 호출
* 예외처리를 통한 빌드 수행 제어

```groovy
// build.gradle
task exeTask{
    doLast{
        println 'exeTask name : ' + name
        println 'exeTask path : ' + path
        println 'exeTask description : ' + description
        println 'exeTask group : ' + group
    }
}

exeTask.group 'Task Object'
exeTask.description = 'Task Object Script Code'

task exeAnoTask{
    doLast{
        println 'dependsOn Task'
    }
}

exeTask.dependsOn exeAnoTask
```

![](/file/image/gradle-object-03.png)

```groovy
// build.gradle
task exeTask{
    doLast{
        println 'exeTask name : ' + name
        println 'exeTask path : ' + path
        println 'exeTask description : ' + description
        println 'exeTask group : ' + group
    }
}

exeTask.group 'Task Object'
exeTask.description = 'Task Object Script Code'

task exeAnoTask{
    doLast{
        println 'dependsOn Task'
    }
}

exeTask.doFirst{
    println 'Task Start --->'
}

exeTask.doLast{
    println '<--- Task End'
}

exeTask.dependsOn exeAnoTask
```

![](/file/image/gradle-object-04.png)

---

* [강좌](https://www.youtube.com/watch?v=AGcnkEuey1A&list=PL7mmuO705dG2pdxCYCCJeAgOeuQN1seZz&index=15)

## Gradle 객체

```groovy
// build.gradle
task exeTask{
    doLast{
        println 'Gradle Instance Properties=>'
        println 'gradleHomeDir : ' + gradle.gradleHomeDir
        println 'gradleUserHomeDir : ' + gradle.gradleUserHomeDir
        println 'gradleVersion : ' + gradle.gradleVersion
        println 'rootProject : ' + gradle.rootProject
        println 'startParameter : ' + gradle.startParameter
        println 'taskGraph : ' + gradle.taskGraph
    }
}
```

![](/file/image/gradle-object-05.png)

```groovy
// init.gradle
class addListenerExe implements TaskExecutionListener{
    void beforeExecute(Task task){
        println 'beforeExecute : ' + task.name
    }
    void afterExecute(Task task, TaskState state){
        if(state.getExecuted()){
            println 'afterExecute : ' + task.name
        }
    }
}

// TaskExecurionListner를 구현한 addListenerExe를 Gradle에 이벤트 리스터 추가
gradle.addListener(new addListenerExe())
```

![](/file/image/gradle-object-06.png)

```groovy
// init.gradle
class addListenerExe implements TaskExecutionListener{
    void beforeExecute(Task task){
        println 'beforeExecute : ' + task.name
    }
    void afterExecute(Task task, TaskState state){
        if(state.getExecuted()){
            println 'afterExecute : ' + task.name
        }
    }
}

gradle.addListener(new addListenerExe())

settingsEvaluated{
    println 'settingsEvaluated() API'
}

projectsLoaded{
    println 'projectsLoaded() API'
}

beforeProject{
    println 'beforeProject() API'
}

afterProject{
    println 'afterProject() API'
}

projectsEvaluated{
    println 'projectsEvaluated() API'
}

buildFinished{
    println 'buildFinished() API'
}
```

```groovy
// build.gradle
task exeTask{
    doLast{
        println 'Gradle Instance Properties=>'
        println 'gradleHomeDir : ' + gradle.gradleHomeDir
        println 'gradleUserHomeDir : ' + gradle.gradleUserHomeDir
        println 'gradleVersion : ' + gradle.gradleVersion
        println 'rootProject : ' + gradle.rootProject
        println 'startParameter : ' + gradle.startParameter
        println 'taskGraph : ' + gradle.taskGraph
    }
}
```

![](/file/image/gradle-object-07.png)