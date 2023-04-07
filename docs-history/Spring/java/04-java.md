---
layout: default
title: "04. [JAVA8] optional"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```java
public class OnlineClass {
    private Integer id;
    private String title;
    private boolean closed;

    public Progress progress;

    public OnlineClass(Integer id, String title, boolean closed) {
        this.id = id;
        this.title = title;
        this.closed = closed;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public boolean isClosed() {
        return closed;
    }

    public void setClosed(boolean closed) {
        this.closed = closed;
    }
}
```

```java
import java.time.Duration;

public class Progress {
    private Duration studyDuration;
    private boolean finished;

    public Duration getFinished() {
        return studyDuration;
    }

    public void setFinished(Duration duration) {
        this.studyDuration = duration;
    }
}
```

```java
OnlineClass spring_boot = new OnlineClass(1, "spring boot", true);
Duration d = spring_boot.getProgress().getStudyDuration();
System.out.println(d);      // Error - null에 대한 호출

// 해결해보자.
```

```java
public Optional<Progress> getProgress() {
    return Optional.ofNullable(progress);
}
```

---

## optional api example

```java
List<OnlineClass> springClasses = new ArrayList<>();
springClasses.add(new OnlineClass(1, "spring boot", true));
springClasses.add(new OnlineClass(2, "spring data jap", true));
springClasses.add(new OnlineClass(3, "spring mvc", true));
springClasses.add(new OnlineClass(4, "spring core", false));
springClasses.add(new OnlineClass(5, "spring api development", false));


Optional<OnlineClass> s = springClasses.stream()
        .filter(oc -> oc.getTitle().startsWith("spring"))
        .findFirst();
s.isPresent();  // check
s.get();        // get instance(단 조건검사를 해야 안전)

// 이런식으로 조건 체크가능
s.ifPresent(oc -> System.out.println(oc.getTitle()));

// OnlineClass c = s.orElse(createNewClass());
    // orElse를 쓰면 무조건 createNewClass가 호출된다.(이미 만들어진 인스턴스의 경우 Else사용)
// OnlineClass c = s.orElseGet(() -> createNewClass());
OnlineClass c = s.orElseGet(App::createNewClass);

// Optional<OnlineClass> cc = s.filter(oc -> !oc.isClosed());
Optional<OnlineClass> cc = s.filter(OnlineClass::isClosed);

Optional<Integer> ci = s.map(OnlineClass::getId);

Optional<Optional<Progress>> pp = s.map(OnlineClass::getProgress);
Optional<Progress> p1 = pp.orElseThrow();
p1.orElseThrow();   // 꺼내기가 까다롭다.

Optional<Progress> p2 = s.flatMap(OnlineClass::getProgress);    // Good
```