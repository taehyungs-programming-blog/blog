---
layout: default
title: "Verify with Wallet 확장 정리"
parent: "Backend"
nav_order: 13
permalink: "/(backend)/verify-with-wallet/"
---

# Verify with Wallet 확장 정리

## 0. 용어

2026년 9월 29일 오전 9:33

| 용어 | 설명 |
| --- | --- |
| mDoc | 스마트폰에 저장된 정부 발급 모바일 신분증 (모바일 운전면허 등) |
| Verify with Wallet | Apple 지갑 속 mDoc으로 앱이 신원을 확인하게 해주는 API |
| Entitlement | 앱이 쓸 수 있는 권한 목록. Apple 승인 필요 |
| Data element | 신분증의 개별 항목 (이름, 생년월일, 주소, 사진 등) |
| ISO/IEC 18013-5 | 모바일 운전면허 데이터 형식/검증 국제 표준 |
| IACA | 발급 기관(DMV)의 루트 인증서 (신뢰의 시작점) |
| MSO | 발급 기관이 데이터에 서명해 만든 무결성 증명 객체 |
| HPKE | 공개키 기반 하이브리드 암호화 방식 (RFC 9180) |
| CBOR | JSON과 비슷하지만 바이너리로 더 작은 데이터 포맷 |

## 1. 배경 / 문제

### 상황

- 신원 인증 조직이 차량 호출, 배달, 배송 등 여러 앱과 마켓플레이스에서 공통으로 쓰는 신원 인증 플랫폼을 운영한다.
- 기존 방식은 신분증 촬영 + 셀피였다. 동작은 하지만 실물 신분증을 찾아야 하고, 사진 품질 때문에 실패하는 경우가 있다.
- Apple이 Verify with Wallet을 내놓으면서, 지갑에 저장된 정부 발급 신분증으로 몇 초 만에 인증할 수 있게 되었다.

### 핵심 난제: 서비스마다 필요한 정보가 다르다

같은 "신원 인증"이어도 서비스마다 필요한 항목이 다르다.

예시:

- 차량호출 기사 등록: 이름, 생년월일, 면허 유효기간, 사진
- 배달 앱 주류 수령: 생년월일(만 21세 이상 여부)만 필요
- 배송 앱 고액 배송: 이름, 사진

그런데 Apple은 **use case별로 데이터 요소마다 entitlement 승인**을 요구한다. 그래서 다음 두 가지를 동시에 만족해야 했다.

- 앱마다 요청 항목을 다르게 가져갈 수 있는 유연성
- 필요 없는 항목은 절대 받지 않는 엄격한 통제

## 2. 목표

- Apple Verify with Wallet을 공용 Identity Verification Platform에 통합
- 실물 신분증 없이 몇 초 만에 인증
- use case별 데이터 범위(Scope) 엄격 제한
- ISO/IEC 18013-5 준수, 정부 발급 자격증명 검증
- 필요한 데이터 요소만 요청

## 3. 아키텍처 (3계층)

```text
[앱] (use case: 주류 배달)
  │
  ▼
[Identity Verification Service]
  1. use case 설정 조회
  2. PassKit 요청 객체로 변환
  │
  ▼
[Apple 지갑 UI] ← 사용자 승인 (Face ID)
  요청: 생년월일만
  │ 암호화된 응답
  ▼
[Cryptographic Validation]
  │ 검증 결과
  ▼
[Identity Verification Service]
```

### 3.1 App Configuration 계층

- 각 앱의 Entitlements 파일에는 **쓸 가능성이 있는 모든** 데이터 요소가 들어 있다.
- 실제로 이번 요청에서 무엇을 물어볼지는 앱이 정하지 않고
  **서버 설정**이 결정한다.

예시: 배달 앱 entitlement에 `family_name`, `given_name`,
`birth_date`, `portrait`가 모두 있어도, 주류 배달 use case 설정에는
`birth_date` (또는 `age_over_21`)만 있으면 서버는 그것만 요청한다.

### 3.2 Identity Verification Service

- 중앙 오케스트레이터. 요청 흐름 전체를 관리한다.
- 내부 신원 모델(예: "성인 확인")을 Apple의 PassKit 요청 객체로 변환한다.

예시:

```yaml
- use_case: alcohol_delivery
  required_elements:
    - age_over_21
  usage_description_key: ALCOHOL_AGE_CHECK

- use_case: driver_onboarding
  required_elements:
    - family_name
    - given_name
    - birth_date
    - portrait
  usage_description_key: DRIVER_ONBOARDING
```

### 3.3 Cryptographic Validation 계층

- 응답 복호화, CBOR 디코딩, 3단계 검증을 수행한다.

## 4. 핵심 설계 결정

### 4.1 플랫폼 비종속 매핑 계층

- 비즈니스 로직("성인 확인이 필요하다")과 벤더 API(Apple PassKit 객체)를 분리한다.
- 나중에 Google Wallet 등 ISO/IEC 18013-5 호환 솔루션이 붙어도 비즈니스 로직은 그대로 두고 매핑만 추가하면 된다.

```text
비즈니스 로직: "age_over_21 필요"
  │
  ▼
[매핑 계층]
  ├── Apple Wallet → PassKit 요청 객체
  └── (추후) 다른 Wallet → 해당 벤더 요청 형식
```

### 4.2 서버 측 강제 (Scope 검증)

- 앱은 entitlement가 넓다. 앱 쪽 코드만 믿으면 실수나 변조로 필요 이상의 정보를 요청할 수 있다.
- 그래서 백엔드가 응답에 들어온 데이터 요소가 해당 use case 설정과 일치하는지 확인한다.

예시:

- 설정: `age_over_21`만 허용
- 응답에 `birth_date`, `portrait`까지 포함됨
- 결과: 서버가 범위 초과로 판단해 거절하거나 초과 항목을 폐기

### 4.3 use case별 안내 문구 (`usageDescriptionKey`)

- 지갑 승인 화면에 표시되는 사유 문구를 use case마다 다르게 지정한다.
- 앱 전체 공통 문구 하나만 쓰면 사용자가 "왜 이 정보를 주는지" 이해하기 어렵다.

예시 (문구는 가상):

- 주류 배달: "주류 배달을 위해 만 21세 이상인지 확인합니다"
- 기사 등록: "기사 등록을 위해 신원을 확인합니다"

## 5. 사용 기술

### HPKE (RFC 9180) + Identity Access Certificate

- 지갑이 보내는 응답은 전송 중 암호화된다.
- 서비스 측이 받은 Identity Access Certificate의 공개키로 암호화되므로 서비스 서버의 개인키로만 복호화할 수 있다.

### CBOR

- 신분증 데이터를 바이너리로 압축한 포맷이다.

예시로 비교하면, JSON의 `{"age_over_21": true}`는 텍스트로 전송되지만,
CBOR은 같은 내용을 더 적은 바이트의 이진 값으로 표현한다.
서버는 복호화 후 CBOR을 디코딩해서 읽는다.

### ISO/IEC 18013-5

- mDoc의 데이터 구조, 전송, 검증 절차를 정한 표준이다.
- 이 표준을 따르므로 벤더가 달라도 검증 원리가 같다.

### 신뢰 인프라 (IACA, VICAL)

- 각 주 DMV가 자기 IACA 인증서를 공개한다.
- AAMVA Digital Trust Service의 VICAL은 신뢰할 수 있는 발급 기관 인증서 목록을 모아 제공한다.
- 서비스는 이를 신뢰 앵커로 사용해 "이 신분증은 진짜 DMV가 발급했다"를 확인한다.

## 6. 확장성 기법

### 6.1 인증서 로테이션 관리

문제: DMV는 주기적으로 서명 키/인증서를 교체한다. 교체 시점에 기존 인증서만 신뢰하면 이미 발급된 신분증이 갑자기 검증 실패한다.

해결:

- 지원하는 모든 관할구역(주)의 IACA 인증서 레지스트리를 유지한다.
  (DMV 웹사이트, 중앙 신뢰 서비스에서 수집)
- **한 관할구역에 여러 개의 활성 인증서**를 허용한다.

예시:

- 캘리포니아 DMV가 2025년에 인증서 A, 2026년에 인증서 B로 교체
- 2025년에 발급받은 사용자의 신분증은 A로 서명되어 있음
- 레지스트리에 A와 B가 모두 활성 상태이므로 두 사용자 모두 검증 통과
- A를 바로 폐기했다면 2025년 발급자는 전부 실패했을 것

### 6.2 Session Transcript 바인딩 (재전송 방지)

문제: 공격자가 정상 응답을 가로채 나중에 다시 보내면 (replay)
인증이 통과될 수 있다.

해결: 응답을 특정 트랜잭션에 묶는다. 묶는 재료는 다음과 같다.

- 요청마다 새로 만드는 고유 nonce
- 클라이언트 식별자 + 공개키 해시

예시:

- 사용자 A가 요청 #1(nonce=`a76`)로 받은 정상 응답
- 공격자가 요청 #2(nonce=`c91b`) 세션에 그 응답을 재사용
- 서버가 기대하는 nonce/키 해시와 불일치 → 거절
