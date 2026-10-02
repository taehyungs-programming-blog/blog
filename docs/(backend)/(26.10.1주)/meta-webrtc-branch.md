---
layout: default
title: "Escaping the Fork: Meta의 WebRTC 현대화 (50+ 사용 사례)"
parent: "Backend"
nav_order: 15
permalink: "/(backend)/meta-webrtc-branch/"
---

# Escaping the Fork: Meta의 WebRTC 현대화 (50+ 사용 사례)

## 1. 배경: 포킹 트랩

- Messenger, Instagram, Cloud Gaming, Meta Quest 등 50+ 사용 사례가 WebRTC 기반 실시간 통신을 사용
- 성능 최적화를 위해 오픈소스 WebRTC를 내부 포크
- 시간이 지나며 업스트림과 멀어져 문제가 누적
- 커뮤니티 업그레이드 적용 불가
- 유지보수 비용 증가
- 보안 취약점 누적

## 2. 제약 조건

### 2.1 A/B 테스트

- 레거시/신규 버전을 동시에 운영하며 동적으로 전환해야 한다.
- 이유: 새 WebRTC가 실제로 더 나은지 측정하려면 같은 앱에서 사용자를 나눠 비교해야 한다. 앱을 두 종류로 배포하면 사용자 집단이 달라져 비교가 부정확하다.
- 그래서 앱 하나에 두 버전을 넣고, 서버 설정으로 사용자 50%는 legacy, 50%는 latest를 쓰게 한 뒤 CPU 사용량·크래시율을 비교한다.

### 2.2 ODR 위반 (심볼 충돌)

ODR(One Definition Rule): 같은 이름의 함수·클래스는 바이너리에 하나만 정의되어야 한다는 C++ 규칙.

```cpp
// legacy WebRTC
namespace webrtc {
class PeerConnection { /* ... */ }; // 정의 1
}

// latest WebRTC
namespace webrtc {
class PeerConnection { /* ... */ }; // 정의 2
}
```

- 두 버전을 한 바이너리에 정적 링크하면 `webrtc::PeerConnection` 이름이 겹친다.
  링커 에러가 나거나, 엉뚱한 버전의 코드가 호출되어 크래시가 난다.
  겹치는 심볼이 수천 개다.

### 2.3 모노레포

- 브랜치가 없는 환경이라 "우리 패치들을 새 업스트림 위에 반복 적용" 하기 어렵다. (해법은 4장)

## 3. 해법 1: 심(Shim) 레이어 + 이중 스택

### 3.1 심 레이어 설계

- 앱과 WebRTC 사이에 프록시 라이브러리를 삽입 (가능한 가장 낮은 계층에서 shimming)
- 버전과 무관한 통합 API를 노출하고, 런타임에
  `webrtc_legacy::` 또는 `webrtc_latest::`로 디스패치

Before (앱이 WebRTC를 직접 호출):

```cpp
webrtc::PeerConnection pc;
pc.CreateOffer();
```

심 내부 (개념 예시):

```cpp
void PeerConnection::CreateOffer() {
  if (use_latest) { // 런타임 설정 (A/B 그룹)
    latest_pc_->CreateOffer(); // webrtc_latest::PeerConnection
  } else {
    legacy_pc_->CreateOffer(); // webrtc_legacy::PeerConnection
  }
}
```

- 앱은 어느 버전이 도는지 모른다.

### 3.2 심볼 충돌 해결

C++ 네임스페이스를 버전별로 자동 재작성한다.

```cpp
namespace webrtc_legacy {
class PeerConnection { /* ... */ };
}
namespace webrtc_latest {
class PeerConnection { /* ... */ };
}
```

이름이 달라져 2.2의 충돌이 사라진다. 네임스페이스 밖의 요소는 따로 처리한다.

- 전역 C 함수: 버전별 식별자로 변경
  - 예: `rtc_init()` → `rtc_init_legacy()` / `rtc_init_latest()`
- 매크로: 네임스페이스와 무관하게 전체에 적용되므로 불필요한
  include를 제거하거나 드물게 쓰이는 매크로는 이름을 변경
  - 예: 양쪽에 `#define MAX_PACKET 1200`이 있으면 충돌
- `rtc_base` 같은 내부 모듈: 양쪽이 하나를 공유하도록 조정

### 3.3 하위 호환: `using` 선언

기존 코드가 `webrtc::Foo`를 수천 곳에서 쓴다.

- 초기 방식: 사용처마다 새 네임스페이스 심볼을 전방 선언해 연결
- 유지보수 부담이 큼
- 개선: C++ `using` 선언으로 네임스페이스를 한 번에 연결

```cpp
namespace webrtc {
using webrtc_shim::Foo;
// webrtc::Foo가 webrtc_shim::Foo를 가리킴
}
```

이름만 연결하므로 코드도 바이너리 크기도 늘지 않는다.

### 3.4 방향성 어댑터와 컨버터

두 버전의 API가 조금씩 다르다.

```cpp
namespace legacy {
struct Options { int bitrate; bool fec; bool dtx; };
}
namespace latest {
struct Options { int max_bitrate; bool fec; Mode mode; };
}
```

- 컨버터: 심의 `shim::Options`를 `legacy::Options` 또는
  `latest::Options`로 변환 (구조체·열거형 번역)
- 어댑터: 통합 API를 구현하고, 받은 호출을 선택된 버전의 실제 객체로 전달
- "방향성"은 shim → legacy, shim → latest 방향별로 따로 만든다는 뜻

### 3.5 심 코드 자동 생성

- 클래스가 수백 개라 손으로 쓰면 하루에 1개 정도
- 헤더를 AST로 파싱해 클래스·구조체·열거형·상수의 기본 심 코드를 생성 → 하루 3~4개로 향상
- getter/setter 위주의 단순한 클래스는 거의 수정 없이 사용
- 두 버전의 API가 다르거나 팩토리 패턴처럼 생성 방식이 다른 경우는 엔지니어가 생성된 코드를 다듬음

### 3.6 이중 스택 앱 빌드

1. 앱의 참조를 `webrtc::Foo` → `webrtc_shim::Foo`로 재배선
2. 작은 빌드 대상부터 시작해 점진적으로 확대
3. 진행하며 문제 발견: 누락된 심, 잘못된 버전의 객체 사용, 매크로/심볼 충돌
4. 소유권 이전, 객체 수명에 대한 단위 테스트 보강
   - 예: legacy로 만든 객체를 latest 함수에 넘기면 크래시 → 테스트로 검출

규모: 심 코드 10,000+ 라인, 수천 개 파일에서 수십만 라인 수정

## 4. 해법 2: 기능 브랜치 관리

### 4.1 풀려는 문제

Meta는 WebRTC에 자체 패치를 여러 개 얹어 쓴다.

- `debug-tools`: 통화 디버깅용 로그/도구 추가 (담당: 품질팀)
- `hw-av1-fixes`: 특정 기기의 AV1 하드웨어 인코더 버그 회피 (담당: 미디어팀)

WebRTC는 Chromium 릴리스(M143, M144 등)에 맞춰 새 버전이 계속 나온다. 새 버전이 나올 때마다 이 패치들을 새 버전 위에 다시 올려야 한다. 이 작업을 "리베이스"라 한다.

기존 방식(포크)의 문제:

```text
main: 업스트림 V1 + 패치 수십 개가 한 줄로 섞인 커밋 히스토리
```

- 어떤 커밋이 어느 패치의 일부인지 구분이 안 된다.
- 업그레이드 때 충돌이 나면 "이 코드가 왜 있는지" 알기 어렵다.
- 패치를 떼어내 업스트림에 기여하기도 어렵다.

### 4.2 핵심 아이디어: 패치 하나 = 브랜치 하나

- Meta 모노레포에는 브랜치 개념이 없다. 그래서 WebRTC 패치 관리용
  Git 저장소를 따로 두고 거기서 브랜치를 쓴다.
- 이 저장소는 Chromium과 같은 구조라서 Chromium 개발 도구를 그대로 쓴다.
- `gclient`: 소스와 의존성을 특정 버전으로 내려받음
- `gn`: 빌드 설정 생성
- `git cl`: 업스트림(Chromium/WebRTC)에 코드 리뷰를 올리는 도구

### 4.3 브랜치 구성 (예: Chromium M143, 태그 7499)

릴리스마다 아래 브랜치들을 만든다. 이름 끝의 `7499`가 어느 업스트림 버전 기준인지를 나타낸다.

```text
base/7499                 # 패치 없는 순수 업스트림
  ├── debug-tools/7499    # debug-tools 커밋만 포함
  └── hw-av1-fixes/7499   # hw-av1-fixes 커밋만 포함

r7499                    # 위 패치 브랜치를 모두 병합한 빌드용 브랜치
```

- `git diff base/7499 r7499`: Meta가 업스트림에서 바꾼 내용 전체
- `git log base/7499..hw-av1-fixes/7499`: 해당 패치의 커밋만 확인

### 4.4 새 릴리스로 올리는 절차 (7499 → 7559)

```bash
# 1. 새 업스트림 버전을 base로 만든다 (패치 없음)
git checkout -b base/7559 <업스트림-태그-7559>

# 2. 패치 브랜치마다 새 base를 병합한다 (충돌은 여기서 해결)
git checkout -b debug-tools/7559 debug-tools/7499
git merge base/7559
# 충돌 → debug-tools 담당자가 해결

git checkout -b hw-av1-fixes/7559 hw-av1-fixes/7499
git merge base/7559
# 충돌 → hw-av1-fixes 담당자가 해결

# 3. 해결된 패치 브랜치를 모아 릴리스 후보 브랜치를 만든다
git checkout -b r7559 base/7559
git merge debug-tools/7559
git merge hw-av1-fixes/7559
```

(원문은 "이전 브랜치를 새 브랜치로 병합"이라고만 설명한다. 위 명령어는 그 흐름을 풀어쓴 예시이다.)

핵심은 충돌이 "패치 단위"로 나뉜다는 점이다.

- 업스트림 변경이 `hw-av1-fixes` 가 건드린 코드와 겹치면 그 브랜치에서만 충돌이 난다.
- `debug-tools`는 영향이 없으면 그대로 넘어간다.
- 충돌이 난 브랜치는 그 패치의 담당 팀이 맥락을 알고 해결한다.

### 4.5 이렇게 하면 좋아지는 점

- **병렬 작업**: 브랜치별로 충돌을 따로 풀 수 있어 여러 사람이 동시에 작업한다. 하나의 히스토리에 섞여 있으면 순서대로 풀어야 한다.
- **히스토리/맥락 보존**: 패치가 왜 필요한지가 브랜치와 커밋에 남는다.
  포크에서는 수년 사이에 사라진 정보다.
- **LLM 자동화**: 충돌 범위가 패치 하나로 좁고 그 패치의 의도가 브랜치에 담겨 있어, LLM이 충돌을 풀기 쉽다. (AI 유지보수의 기반)
- **업스트림 기여**: `hw-av1-fixes`처럼 범용 수정은 브랜치 그대로 `git cl` 로 WebRTC에 리뷰를 올릴 수 있다. 받아들여지면 다음 릴리스부터 업스트림에 포함되므로 그 브랜치는 더 이상 관리할 필요가 없다.

> 시간이 지날수록 유지할 패치가 줄어든다.

### 4.6 이중 스택과의 관계

- 3장의 심 레이어는 "앱 안에서 두 버전을 동시에 쓰는 방법"이다.
- 4장의 브랜치 관리는 latest 쪽 소스를 최신으로 유지하는 방법이다.
- 이 브랜치 관리 덕분에 latest 스택을 M120에서 M145까지 계속 올릴 수 있었다.

## 5. 결과

- M120에서 WebRTC latest 출시, 현재 145까지 진행 ("수년 뒤처짐" → 최신 안정 Chromium 릴리스를 바로 반영)
- Chromium 25개 릴리스분을 따라잡은 것이며, 이후에는 릴리스마다
  4장의 절차로 올림
- CPU 사용량 최대 10% 감소 (업스트림 최적화 반영)
- 크래시율 최대 3% 개선 (업스트림 버그 수정 반영)
- 앱별 압축 바이너리 100~200 KB 감소
- 레거시 스택의 deprecated 라이브러리(예: Msrsete) 제거로 보안 개선
- 사용자 참여도 증가로 이어짐
