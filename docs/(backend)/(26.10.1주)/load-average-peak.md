---
layout: default
title: "주기적인 load average 스파이크의 원인: GLib 정초 타이머와 커널 샘플링"
parent: "Backend"
nav_order: 12
permalink: "/(backend)/load-average-peak/"
---

# 주기적인 load average 스파이크의 원인: GLib 정초 타이머와 커널 샘플링

WebRTC SFU(Janus 기반) 서버에서 load average가 약 1시간 23분(5001초)
간격으로 튀는 현상을 분석한 내용이다.

## 요약

- Janus는 WebRTC 연결(핸들)마다 1초 타이머를 둔다.
- GLib의 초 단위 타이머는 만료 시각을 정초에 맞춘다.
  그래서 모든 핸들의 타이머가 같은 순간에 울린다.
- 리눅스는 load를 5.001초 간격으로 샘플링한다.
  1000번째 샘플(5001초)이 정초와 겹치면서 load1이 튄다.
- 실제 자원 사용량은 늘지 않는다. 수치만 왜곡되고, load1 기반
  alert이 오탐하게 된다.

## 1. 현상

- 통화 트래픽이 있는 SFU 노드에서 `node_load1`이 5001초 주기로 급등한다.
- CPU, 메모리, 디스크 I/O, fork, context switch, 실행 중인 프로세스 수는 모두 변하지 않는다.
- 트래픽이 적은 노드는 스파이크가 없다.
- 진폭은 Janus 스레드 수(= 핸들 수)에 비례한다.
- 하드웨어 벤더나 가상화 여부와 무관하게 발생한다.
- 특정 시점의 배포 회귀가 아니라 오래전부터 있었다.

## 2. 먼저 알아둘 용어

### load1

- 리눅스의 1분 load average이며 `uptime`, `top`에서 보는 값이다.
- "R 상태(실행 중 또는 실행 대기)와 D 상태 태스크 수"를 센 값이다.
- 리눅스에서는 스레드 1개가 태스크 1개다.
- 커널이 5초마다 그 수를 한 번 세고 직전 값에 8%만 반영한다.
  (지수이동평균, 반영률 `1 - e^(-5/60) ≈ 0.08`)

### 정초

- 초 단위가 딱 떨어지는 시각이다. 예: 12:00:01.000
- 이번 문제의 본질이 이 정초다.

### 깨어 있는 스레드

- 스레드는 평소 sleep 상태이고, 타이머가 올리면 깨어나 R 상태가 된다.
- 스레드 개수가 늘어나는 것이 아니라, 존재하는 스레드 중 동시에 깨어 있는 수가 순간적으로 많아지는 것이다.

## 3. 원인

### 3-1. 핸들마다 1초 타이머가 있다

Janus는 DTLS 핸드셰이크가 끝나면 핸들마다 1초 타이머 2개를 붙인다.

```c
handle->rtcp_source = g_timeout_source_new_seconds(1);
handle->stats_source = g_timeout_source_new_seconds(1);
```

- RTCP 송신: SR/SDES/RR 생성, 패킷 손실 집계
- 통계 갱신: 초당 바이트 카운터 리셋, 미디어 끊김 판정
- 둘 다 정상이고 필수인 작업이며 1회 비용은 수십 μs 수준이다.

### 3-2. GLib이 초 단위 타이머를 정초에 맞춘다

`g_timeout_source_new_seconds`는 만료 시각을 정초 경계로 스냅한다.

(`glib/gmain.c`의 `g_timeout_set_expiration`)

아래는 복사 중 깨진 코드를 정렬 과정에 맞춰 복원한 예시다.

```c
if (timeout_source->seconds) {
  expiration_ns = current_time_ns + timeout_source->interval * G_NSEC_PER_SEC;
  expiration_ns -= perturb;
  remainder_ns = expiration_ns % G_NSEC_PER_SEC;
  if (remainder_ns >= G_NSEC_PER_SEC / 4) {
    expiration_ns += G_NSEC_PER_SEC;
  }
  expiration_ns -= remainder_ns;
  expiration_ns += perturb; // 정초 경계 + 프로세스 상수
}
```

- 핸들이 언제 생성됐든 모든 타이머가 같은 정초에 울린다.
- dispatch할 때마다 다시 스냅되므로 정렬이 영구히 유지된다.
- 밀리초 단위 타이머(`g_timeout_source_new`)는 스냅하지 않아
  분산된다.

### 3-3. 핸들마다 전용 스레드가 있다

- `event_loops` 설정이 없으면 Janus는 핸들마다 전용 GMainContext와 스레드를 만든다.
- 정초마다 수백~수천 개 스레드가 동시에 깨어난다.
- 각자 수십 μs만 일하고 다시 자므로 활동 구간은 전체 약 1ms이다.
  CPU 사용량은 늘지 않는다.

### 3-4. 리눅스 load 샘플링 간격은 5.001초다

- 커널은 `LOAD_FREQ = 5 * HZ + 1`로 샘플링한다.
- `CONFIG_HZ=1000` 이면 샘플 간격이 5.001초다.
- 샘플 시각이 1초 안에서 매번 1ms씩 밀린다.

| n번째 샘플 | 시각 | 정초 대비 |
| --- | --- | --- |
| 1 | 5.001초 | +1ms |
| 500 | 2500.500초 | +500ms |
| 1000 | 5001.000초 | 0 (일치) |

- 1000번째 샘플에서 스레드가 깨어 있는 1ms 구간과 겹친다.
- 그 순간 깨어 있던 스레드 수가 load1에 그대로 반영된다.
- 나머지 샘플은 스레드가 자는 999ms 구간에 걸려 정상 값이 나온다.

### 3-5. 일반식

작업 주기 T초, HZ=1000이면 `P_beat = T × 5001`이다.

| 작업 주기 | 스파이크 주기 |
| --- | --- |
| 1초 | 5001초 (1.39시간), 이번 현상 |
| 5초 | 25,005초 (6.95시간) |
| 10초 | 50,010초 (13.9시간) |

샘플 간격이 정확히 5.000초였다면 주기 스파이크는 없다.

항상 같은 위치에 찍혀 늘 높거나 늘 낮게 나온다.

## 4. 검증 방법

- 주기: 피크 시각 간격을 여러 주기에 걸쳐 측정해 5001초 근방인지 확인한다.
- 감소: 피크 이후 scrape 간격(15초)마다 값이 `e^(-15/60) ≈ 0.779`배로 줄어드는지 본다. 단발 impulse의 EWMA 감쇠 모양이다.
- 자원 무변동: 아래 지표가 평탄하면 측정 아티팩트다.

```promql
rate(node_forks_total[1m])
sum by (mode) (rate(node_cpu_seconds_total{mode=~"system|user|iowait"}[1m]))
node_procs_blocked
```

## 5. 영향

- 서비스에는 영향이 없다.
- load1 수치가 왜곡되어 load1 기반 alert이 주기적으로 오탐한다.
- 임계값을 올려도 스레드 수에 비례해 진폭이 커지므로 회피할 수 없다.

## 6. 해소 방안

### 모니터링 교체

- load1 대신 CPU 사용률, 세션 수, RTT 등으로 alert을 구성한다.

### 타이머에서 정초 정렬 제거

- `g_timeout_source_new_seconds(1)`을
  `g_timeout_source_new(1000 + 핸들별 jitter)`로 바꾼다.
- 동시 기동과 RTCP 송신 몰림이 함께 해소된다.
- RFC 3550의 RTCP 송신 간격 랜덤화 규정에도 맞는다.
- 미디어 경로 패치이므로 부하시험과 단계적 배포가 필요하다.

### `event_loops=N` 설정

- 핸들당 스레드 대신 N개 스레드를 공유해 스레드 수를 줄인다.
- 정초 정렬은 그대로 남는다.
- 미디어 지연 영향이 있을 수 있어 부하시험이 선행되어야 한다.

## 7. 교훈

- 연결마다 전용 스레드를 두고 고정 주기 타이머를 정렬시키는 구조는 피한다.
- 연결별 주기 작업에는 Jitter를 넣고, 스레드는 공유 이벤트 루프로 처리한다.
- 용량 산정 시 스레드 수를 자원 모델에 반영한다.
