---
layout: default
title: "08. Bean 생명주기"
parent: "(기초)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Bean 콜백받아보기

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/19)

* 데이터베이스 커넥션 풀이나, 네트워크 소켓처럼 애플리케이션 시작 시점에 필요한 연결을 미리 해두고, 애플리케이션 종료 시점에 연결을 모두 종료하는 작업을 진행하려면, 객체의 초기화와 종료 작업이 필요하다.
* 어렵게 말했지만, 간단히 말하자면 bean 생성/종료 시점에 콜백이 필요하다

```java
package hello.spring.LifeCycle;


// 대략 이런 클래스가 있다 가정하자.
public class NetworkClient {
    private String url;
    
    public NetworkClient() {
        System.out.println("생성자 호출, url = " + url);
        // 생성과 동시에 connect, 단 url을 알고 있어야함.
        connect();
        call("초기화 연결 메시지");
    }
    
    public void setUrl(String url) {
        this.url = url;
    }
    
    //서비스 시작시 호출
    public void connect() {
        System.out.println("connect: " + url);
    }
    
    public void call(String message) {
        System.out.println("call: " + url + " message = " + message);
    }
    
    //서비스 종료시 호출
    public void disconnect() {
        System.out.println("close: " + url);
    }
}
```

```java
// 간단하게 테스트환경으로 만들어보자.
public class BeanLifeCycleTest {
    @Test
    public void lifeCycleTest() {
        ConfigurableApplicationContext ac = new
                AnnotationConfigApplicationContext(LifeCycleConfig.class);

        // bean을 생성하고
        NetworkClient client = ac.getBean(NetworkClient.class);

        // 종료해 버리기
        ac.close(); //스프링 컨테이너를 종료, ConfigurableApplicationContext 필요
    }

    @Configuration
    static class LifeCycleConfig {
        @Bean
        public NetworkClient networkClient() {
            NetworkClient networkClient = new NetworkClient();
            networkClient.setUrl("http://hello-spring.dev");
            return networkClient;
        }
    }
}
```

```
생성자 호출, url = null 
connect: null 
call: null message = 초기화 연결 메시지
```

* 띠용용??? 생각했던것과 완전히 다른경과가??

---

* 왜 그럴까?

* 스프링 빈은 객체를 생성하고, 의존관계 주입이 다 끝난 다음에야 필요한 데이터를 사용할 수 있는 준비가 완료된다. 
* 따라서 초기화 작업은 의존관계 주입이 모두 완료되고 난 다음에 호출해야 한다. (핵심)
* 그런데 개발자가 의존관계 주입이 모두 완료된 시점을 어떻게 알 수 있을까? (핵심2)
* 스프링은 의존관계 주입이 완료되면 스프링 빈에게 콜백 메서드를 통해서 초기화 시점을 알려주는 다양한 기능을 제공한다. 
* 또한 스프링은 스프링 컨테이너가 종료되기 직전에 소멸 콜백을 준다. 따라서 안전하게 종료 작업을 진행할 수 있다.

* **(결론)**
    * 스프링 빈의 이벤트 라이프사이클 
    * 스프링 컨테이너 생성 -> 스프링 빈 생성 -> 의존관계 주입 -> 초기화 콜백 -> 사용 -> 소멸전 콜백 -> 스프링 종료

---

## 스프링은 크게 3가지 방법으로 빈 생명주기 콜백을 지원한다. 

- 인터페이스(InitializingBean, DisposableBean) 
- 설정 정보에 초기화 메서드, 종료 메서드 지정 
- **@PostConstruct, @PreDestroy 애노테이션 지원** -> 사실 이거만 씀.

* 하나씩 살펴보자.

---

## 인터페이스(InitializingBean, DisposableBean) 

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/20)

```java
// 사용법은 간단하다 - InitializingBean(시작), DisposableBean(종료)
public class NetworkClient implements InitializingBean, DisposableBean {
    private String url;

    public NetworkClient() {
        System.out.println("생성자 호출, url = " + url);
    }

    public void setUrl(String url) {
        this.url = url;
    }

    //서비스 시작시 호출
    public void connect() {
        System.out.println("connect: " + url);
    }

    public void call(String message) {
        System.out.println("call: " + url + " message = " + message);
    }

    //서비스 종료시 호출
    public void disConnect() {
        System.out.println("close + " + url);
    }

    @Override
    public void afterPropertiesSet() throws Exception {
        connect();
        call("초기화 연결 메시지");
    }

    @Override
    public void destroy() throws Exception {
        disConnect();
    }
}
```

* 단, spring에 의존적이다. 
* 초기화 소멸의 메소드 이름을 변경할 수 없다.

* **더 좋은 방법을 살펴보자**

---

## 설정 정보에 초기화 메서드, 종료 메서드 지정

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/21)

```java
// 다시 코드를 원복하고
public class NetworkClient {
    private String url;
    public NetworkClient() {
        System.out.println("생성자 호출, url = " + url);
    }
    public void setUrl(String url) {
        this.url = url;
    }
    //서비스 시작시 호출
    public void connect() {
        System.out.println("connect: " + url);
    }
    public void call(String message) {
        System.out.println("call: " + url + " message = " + message);
    }
    //서비스 종료시 호출
    public void disConnect() {
        System.out.println("close + " + url);
    }
    public void init() {
        System.out.println("NetworkClient.init");
        connect();
        call("초기화 연결 메시지");
    }
    public void close() {
        System.out.println("NetworkClient.close");
        disConnect();
    }
}
```

```java
public class BeanLifeCycleTest {
    @Test
    public void lifeCycleTest() {
        ConfigurableApplicationContext ac = new
                AnnotationConfigApplicationContext(LifeCycleConfig.class);
        NetworkClient client = ac.getBean(NetworkClient.class);
        ac.close(); //스프링 컨테이너를 종료, ConfigurableApplicationContext 필요
    }

    @Configuration
    static class LifeCycleConfig {
        @Bean(initMethod = "init", destroyMethod = "close")
        public NetworkClient networkClient() {
            NetworkClient networkClient = new NetworkClient();
            networkClient.setUrl("http://hello-spring.dev");
            return networkClient;
        }
    }
}
```

* 종료 메서드 추론
    * @Bean의 destroyMethod 속성에는 아주 특별한 기능이 있다. 
    * 라이브러리는 대부분 close , shutdown 이라는 이름의 종료 메서드를 사용한다. 
    * @Bean의 destroyMethod 는 기본값이 (inferred) (추론)으로 등록되어 있다. 
    * 이 추론 기능은 close , shutdown 라는 이름의 메서드를 자동으로 호출해준다. 
    * 이름 그대로 종료 메서드를 추론해서 호출해준다. 따라서 직접 스프링 빈으로 등록하면 종료 메서드는 따로 적어주지 않아도 잘 동작한다. 
    * 추론 기능을 사용하기 싫으면 destroyMethod="" 처럼 빈 공백을 지정하면 된다.

---

## @PostConstruct, @PreDestroy 애노테이션 지원

* [Clone Code 🌎](https://github.com/EasyCoding-7/spring_basic/tree/22)

```java
public class NetworkClient {
    private String url;
    public NetworkClient() {
        System.out.println("생성자 호출, url = " + url);
    }
    public void setUrl(String url) {
        this.url = url;
    }
    //서비스 시작시 호출
    public void connect() {
        System.out.println("connect: " + url);
    }
    public void call(String message) {
        System.out.println("call: " + url + " message = " + message);
    }
    //서비스 종료시 호출
    public void disConnect() {
        System.out.println("close + " + url);
    }
    @PostConstruct
    public void init() {
        System.out.println("NetworkClient.init");
        connect();
        call("초기화 연결 메시지");
    }
    @PreDestroy
    public void close() {
        System.out.println("NetworkClient.close");
        disConnect();
    }
}
```

```java
public class BeanLifeCycleTest {
    @Test
    public void lifeCycleTest() {
        ConfigurableApplicationContext ac = new
                AnnotationConfigApplicationContext(LifeCycleConfig.class);
        NetworkClient client = ac.getBean(NetworkClient.class);
        ac.close(); //스프링 컨테이너를 종료, ConfigurableApplicationContext 필요
    }

    @Configuration
    static class LifeCycleConfig {
        @Bean
        public NetworkClient networkClient() {
            NetworkClient networkClient = new NetworkClient();
            networkClient.setUrl("http://hello-spring.dev");
            return networkClient;
        }
    }
}
```

* **정리**
    * @PostConstruct, @PreDestroy 애노테이션을 사용하자 
    * 코드를 고칠 수 없는 외부 라이브러리를 초기화, 종료해야 하면 @Bean 의 initMethod , destroyMethod 를 사용하자.