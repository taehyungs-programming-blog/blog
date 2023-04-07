---
layout: default
title: "06. [JAVA8] concurrency"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Excutors

* Thread 관리를 JAVA에게 넘긴다.

```java
public class Main {
    public static void main(String[] args) {
        // ExecutorService s = Executors.newSingleThreadExecutor();
        ExecutorService s = Executors.newFixedThreadPool(2);
        s.submit(() -> {
            System.out.println("Thread " + Thread.currentThread().getName());
        });
        s.shutdown();       // 동작을 마친후 shutdown
        // s.shutdownNow(); // 그냥 바로 shutdown
    }
}
```

```java
public class Main {
    public static void main(String[] args) {
        ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
        executorService.schedule(getRunnable("Hello"), 3, SECONDS);

        executorService.shutdown();
    }

    private static Runnable getRunnable(String message) {
        return () -> System.out.println(message + Thread.currentThread().getName());
    }
}
```

---

## Callable과 Future

```java
public class Main {
    public static void main(String[] args) {
        ExecutorService executorService = Executors.newSingleThreadExecutor();

        Callable<String> hello = () -> {
          Thread.sleep(2000L);
          return "Hello";
        };

        Future<String> submit = executorService.submit(hello);
        System.out.println(submit.isDone());    // 종료체크
        System.out.println("Started!");

        String result = submit.get();   // Get을 호출시 Thread대기

        System.out.println(submit.isDone());
        System.out.println("End!");
        executorService.shutdown();
    }
}
```

```java
public class Main {
    public static void main(String[] args) throws InterruptedException, ExecutionException {
        ExecutorService executorService = Executors.newFixedThreadPool(4);

        // 여러 명령을 동시에 보낼수 있음.

        Callable<String> hello = () -> {
          Thread.sleep(2000L);
          return "Hello";
        };

        Callable<String> hello2 = () -> {
            Thread.sleep(2000L * 2);
            return "Hello2";
        };

        Callable<String> hello3 = () -> {
            Thread.sleep(2000L * 3);
            return "Hello3";
        };

        // invokeAll - 여러 명령의 결과를 대기한다.
        List<Future<String>> futures = executorService.invokeAll(Arrays.asList(hello, hello2, hello3));
        
        // invokeAny - 가장 빨리 리턴되는것을 출력
        String s = executorService.invokeAny(Arrays.asList(hello, hello2, hello3));
        for(Future<String> f : futures) {
            System.out.println(f.get());
        }

        executorService.shutdown();
    }
}
```

---

## CompletableFuture

```java
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        ExecutorService executorService = Executors.newFixedThreadPool(4);
        Future<String> future = executorService.submit(() -> "hello");

        future.get();
        // future의 문제점 - get을 받기전 future의 결과물로 어떠한 연산도 할수 없음(중간에 연산을 넣지 못함)
    }
}
```

```java
// 대략적 사용법 - 리턴값이 없는경우
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<Void> future = CompletableFuture.runAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
        });
        future.get();
    }
}
```

```java
// 리턴값이 있는경우.
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<String> future = CompletableFuture.supplyAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
            return "hello";
        });
        System.out.println(future.get());
    }
}
```

* 지금까지로 봐선 Future랑 뭐가 다른지 모르겠음

```java
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<String> future = CompletableFuture.supplyAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
            return "hello";
        }).thenApply((s) -> {
            // callback을 get호출전 어떠한 연산을 넣을 수 있다.
            System.out.println(Thread.currentThread().getName());
            return s.toUpperCase();
        });
        System.out.println(future.get());
    }
}
```

```java
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<String> hello = CompletableFuture.supplyAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
            return "Hello";
        });


        // hello이후 world를 실행하려 한다.
        // CompletableFuture<String> future =  hello.thenCompose(s -> getWorld(s));
        CompletableFuture<String> future = hello.thenCompose(Main::getWorld);
        System.out.println(future.get());
    }

    private static CompletableFuture<String> getWorld(String message) {
        return CompletableFuture.supplyAsync(() -> {
            System.out.println("World " + Thread.currentThread().getName());
            return "World";
        });
    }
}
```

```java
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<String> hello = CompletableFuture.supplyAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
            return "Hello";
        });

        CompletableFuture<String> world = CompletableFuture.supplyAsync(() -> {
            System.out.println("World " + Thread.currentThread().getName());
            return "World";
        });

        // hello와 world를 동시에 실행하고 싶다
        CompletableFuture<String> future = hello.thenCombine(world, (h, w) -> {
           return h + " " + w;
        });
        System.out.println(future.get());
    }
}
```

```java
public class Main {
    public static void main(String[] args) throws ExecutionException, InterruptedException {
        CompletableFuture<String> hello = CompletableFuture.supplyAsync(() -> {
            System.out.println("Hello " + Thread.currentThread().getName());
            return "Hello";
        });

        CompletableFuture<String> world = CompletableFuture.supplyAsync(() -> {
            System.out.println("World " + Thread.currentThread().getName());
            return "World";
        });

        // hello와 world를 동시에 실행하고 싶다 - 2
        List<CompletableFuture> futures = Arrays.asList(hello, world);
        CompletableFuture[] futuresArray = futures.toArray(new CompletableFuture[futures.size()]);

        CompletableFuture<List<Object>> results = CompletableFuture.allOf(futuresArray)
                .thenApply(v -> {
                    return futures.stream()
                            // .map(f -> f.join())
                            .map(CompletableFuture::join)
                            .collect(Collectors.toList());
                });
        
        results.get().forEach(System.out::println);
    }
}
```
