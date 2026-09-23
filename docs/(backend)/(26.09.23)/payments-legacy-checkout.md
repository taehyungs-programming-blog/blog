---
layout: default
title: "가맹점 변경 없이 레거시 결제창 전면 재작성하기"
parent: "Backend"
nav_order: 6
permalink: "/(backend)/payments-legacy-checkout/"
---
# 가맹점 변경 없이 레거시 결제창 전면 재작성하기

> 아래 코드와 요청 값은 원문의 설계를 이해하기 쉽게 단순화한 예시이며, 실제 운영 코드와 설정값은 아니다.

## 개편 배경

 결제창은 국내외 카드사와 여러 간편결제사를 연결한다. 겉으로는 정상 동작했지만 20년 동안 기능을 덧붙이면서 새로운 결제수단이나 기능을 추가하기 어려운 구조가 되었다.

### 하나의 메서드에 모든 분기가 모여 있다

결제수단과 카드사별 처리를 하나의 Java 메서드에서 분기했다. 관련 코드만 2,500줄이 넘었고, 공통 기능도 컴포넌트로 분리하지 않아 결제방식마다 비슷한 코드를 반복했다.

```text
결제 요청
   ├─ 카드
   │   ├─ 신한카드
   │   ├─ KB카드
   │   └─ ...
   ├─ 계좌이체
   └─ 가상계좌
```

기능 하나를 추가하려면 거대한 조건문 곳곳을 수정해야 하므로 영향 범위를 파악하기 어렵고 변경 위험도 컸다.

예를 들어 카드 결제에 현금영수증 제한 규칙을 추가한다고 가정하자. 레거시 구조에서는 카드 분기뿐 아니라 계좌이체와 가상계좌 분기에도 비슷한 조건이 복사되어 있을 수 있다.

```java
if (payMethod.equals("CARD")) {
    if (cashReceiptYn.equals("Y")) {
        // 카드에서는 현금영수증 사용 불가
    }
} else if (payMethod.equals("BANK")) {
    if (cashReceiptYn.equals("Y")) {
        // 계좌이체용 현금영수증 처리
    }
}
```

개발자는 같은 규칙이 어느 분기에 얼마나 복사되어 있는지 찾아야 한다. 하나라도 빠뜨리면 결제수단에 따라 서로 다른 결과가 발생한다.

### 오래된 기술과 강한 결합

시스템은 Struts와 WebLogic을 사용해 신규 개발자의 학습 비용이 높았다. 또한 JavaScript, JSP, Java 코드에 비즈니스 로직이 흩어져 있었고 JSP가 직접 DB를 조회하기도 했다.

따라서 한 기능을 변경할 때 프론트엔드와 백엔드 코드를 모두 추적하고 함께 수정해야 했다. 두 영역이 독립적으로 개발되기 어려운 구조였다.

## 파라미터를 도메인 모델로 변환한다

기존 가맹점은 수백 개의 평면적인 HTML Form 파라미터를 사용했다. 같은 값도 맥락에 따라 다르게 동작했고, 모든 값은 하나의 거대한 가변 DTO에 저장되어 시스템 전역에서 변경됐다.

어떤 시점의 값을 이해하려면 DTO를 변경하는 모든 코드를 실행 순서대로 추적해야 했다. 사실상 전역변수와 같은 문제였다.

### 값과 기능을 구분한다

신규 시스템은 입력을 두 종류로 분류해 `SessionCreationRequest`로 변환한다.

- 값 파라미터: 가맹점 ID, 주문번호, 금액처럼 그 자체로 의미가 있는 값
- 기능 파라미터: 현금영수증, 즉시할인, 할부처럼 결제 동작을 제어하는 값

```kotlin
data class SessionCreationRequest(
    val merchantId: String,
    val orderId: String,
    val amount: BigDecimal,
    val cashReceiptFeature: FeatureCashReceipt?,
    val cardDiscountFeature: FeatureCardDiscount?,
    val cardInstallmentFeature: FeatureCardInstallment?,
)
```

여러 원시 파라미터로 표현되던 기능을 하나의 Feature 객체로 묶으면 유효성 검증과 기본값 처리가 객체 안에 모인다. 코어 로직은 오래된 파라미터 이름이나 조합 규칙을 알 필요가 없다.

예를 들어 레거시 요청에서 현금영수증은 두 개의 문자열로 전달될 수 있다.

```text
CASH_RECEIPT_YN=Y
CASH_RECEIPT_TP=I
```

기존 코드에서는 이 두 값을 사용하는 곳마다 `Y`와 `I`의 의미를 해석했다. 신규 구조에서는 요청을 받는 경계에서 한 번만 도메인 객체로 바꾼다.

```kotlin
data class FeatureCashReceipt(
    val enabled: Boolean,
    val type: CashReceiptType?,
) {
    companion object {
        fun from(enabled: String?, type: String?): FeatureCashReceipt {
            if (enabled != "Y") return FeatureCashReceipt(false, null)

            val receiptType = when (type) {
                "I" -> CashReceiptType.INCOME_DEDUCTION
                "E" -> CashReceiptType.EXPENSE_PROOF
                else -> throw IllegalArgumentException("지원하지 않는 현금영수증 유형")
            }
            return FeatureCashReceipt(true, receiptType)
        }
    }
}
```

이후 코어 로직은 `CASH_RECEIPT_TP`의 `I`가 무엇을 뜻하는지 몰라도 된다. `INCOME_DEDUCTION`이라는 명확한 도메인 값만 사용한다.

### 외부 형식은 Converter에서 격리한다

레거시 Form 요청과 JSON 기반 OpenAPI 요청은 서로 다른 Converter를 거쳐 동일한 내부 모델로 바뀐다.

```text
레거시 Form ── LegacyConverter ──┐
                                 ├─ SessionCreationRequest → 코어 로직
OpenAPI JSON ─ OpenAPIConverter ─┘
```

`LegacyConverter`는 일관되지 않은 파싱, 한글 인코딩, 복잡한 검증과 기본값을 담당한다. `OpenAPIConverter`는 현대적인 요청 스펙을 같은 도메인 모델로 변환한다.

예를 들어 기존 가맹점과 신규 가맹점이 같은 10,000원 결제를 서로 다른 형태로 요청할 수 있다.

**기존 가맹점의 Form 요청**

```text
MID=shop_1&OID=order_100&AMOUNT=10000&CASH_RECEIPT_YN=Y&CASH_RECEIPT_TP=I
```

**신규 OpenAPI 요청**

```json
{
  "merchantId": "shop_1",
  "orderId": "order_100",
  "amount": 10000,
  "cashReceipt": {
    "enabled": true,
    "type": "INCOME_DEDUCTION"
  }
}
```

두 요청은 각 Converter를 지난 뒤 같은 객체가 된다.

```text
SessionCreationRequest(
  merchantId = "shop_1",
  orderId = "order_100",
  amount = 10000,
  cashReceiptFeature = 현금영수증(소득공제)
)
```

따라서 결제 처리 코드는 요청이 20년 전 Form에서 왔는지 최신 JSON에서 왔는지 구분하지 않는다.

이 구조의 핵심은 경계에서 호환성을 흡수하는 것이다.

- 기존 가맹점은 연동 방식을 바꾸지 않아도 된다.
- 레거시 제약이 새로운 OpenAPI의 발전을 막지 않는다.
- 새로운 입력 방식이 생기면 Converter만 추가하면 된다.
- 외부 인터페이스의 변경이 코어 비즈니스 로직으로 전파되지 않는다.

## 결제 흐름을 3단계로 표준화한다

신규 결제창은 사용자 행동과 서버 요청을 세 단계로 정의했다.

1. `ENTRY`: 첫 화면을 그리는 데이터를 요청한다.
2. `PREPARE`: 사용자가 선택한 결제수단에 필요한 데이터를 요청한다.
3. `CONFIRM`: 인증을 마친 뒤 결제 성공 화면으로 이동한다.

명확한 퍼널을 만들면 프론트엔드와 백엔드의 API 계약이 단순해지고, 각 단계의 성공률을 관찰해 장애도 탐지할 수 있다.

카드 결제를 예로 들면 실제 흐름은 다음과 같다.

```text
1. ENTRY
   고객이 결제창 진입
   → 서버가 주문 금액과 사용 가능한 카드 목록 반환

2. PREPARE
   고객이 신한카드와 3개월 할부 선택
   → 서버가 카드사 인증 주소와 암호화된 인증 데이터 반환

3. CONFIRM
   고객이 카드사 인증 완료
   → 서버가 인증 결과를 확인하고 결제 완료 화면으로 이동
```

이렇게 단계를 고정하면 결제수단이 카드에서 간편결제로 바뀌어도 전체 API 흐름은 유지되고 단계별 내부 처리만 달라진다.

## Bridge로 프론트엔드의 비즈니스 로직을 제거한다

결제창은 카드사나 간편결제사로 사용자를 보내기 위해 Redirect 또는 Form POST를 수행한다. 목적지와 전송 방식은 결제수단마다 다르지만, 이를 프론트엔드가 판단하면 결제 비즈니스 로직이 다시 화면 코드에 스며든다.

신규 시스템은 서버 응답을 Bridge 인터페이스로 통일했다.

```json
{
  "destination": "https://card.example.com/auth",
  "method": "POST",
  "payload": {
    "merchantId": "toss_123",
    "amount": 10000,
    "encryptedData": "..."
  }
}
```

서버는 목적지, 전송 방식, 데이터를 결정한다. 프론트엔드는 응답에 적힌 방식대로 데이터를 중계하기만 한다.

```text
백엔드: 어디로, 어떤 방식으로, 무엇을 보낼지 결정
프론트엔드: Bridge 응답을 브라우저에서 실행
```

결제수단별 연동이 복잡해져도 프론트엔드는 그 내용을 알 필요가 없으므로 두 영역의 책임이 분리된다.

예를 들어 신한카드는 Form POST, 어떤 간편결제사는 Redirect를 요구한다고 가정하자. 프론트엔드가 결제수단별 조건문을 갖는 대신 Bridge 실행기 하나만 둔다.

```javascript
function executeBridge(bridge) {
  if (bridge.method === "GET") {
    window.location.href = buildUrl(bridge.destination, bridge.payload);
    return;
  }

  submitHiddenForm(bridge.destination, bridge.payload);
}
```

```text
신한카드 선택
→ { method: "POST", destination: "신한카드 인증 주소", payload: {...} }
→ 프론트엔드가 숨은 Form을 만들어 제출

간편결제 선택
→ { method: "GET", destination: "간편결제 인증 주소", payload: {...} }
→ 프론트엔드가 해당 주소로 이동
```

새 카드사를 추가해도 서버가 알맞은 Bridge 응답만 만들면 된다. 프론트엔드에는 카드사 이름을 기준으로 한 분기가 추가되지 않는다.

## Canary와 자동 롤백으로 점진 전환한다

수만 개 가맹점의 요청을 한 번에 신규 시스템으로 바꾸면 장애가 전체 결제로 확산될 수 있다. 이를 막기 위해 가맹점과 세부 기능 단위로 신규 시스템 적용 여부를 제어했다.

```text
결제 요청
   ↓ 가맹점 × 기능별 라우팅
   ├─ 신규 결제창
   └─ 레거시 결제창
```

각 결제 요청을 어느 시스템에서 처리할지 결정하고, `ENTRY-PREPARE-CONFIRM` 퍼널을 실시간으로 관찰했다. 예를 들어 `PREPARE` 대비 `CONFIRM` 비율이 정상 범위 아래로 떨어지면 사용자가 결제를 끝내지 못한다고 판단한다.

이상 징후가 감지되면 문제가 생긴 결제수단의 신규 트래픽을 즉시 중단하고 레거시 시스템으로 자동 복귀한다. 세밀한 트래픽 제어와 자동 롤백이 서비스 연속성을 유지하는 안전장치다.

예를 들어 신규 시스템을 `가맹점 A의 신한카드 결제`에만 먼저 적용할 수 있다.

```text
가맹점 A + 신한카드 + 즉시할인 미사용 → 신규 시스템
가맹점 A + 신한카드 + 즉시할인 사용   → 레거시 시스템
가맹점 A + 다른 카드                  → 레거시 시스템
다른 가맹점                           → 레거시 시스템
```

10분 동안 `PREPARE`가 1,000건이고 평소라면 `CONFIRM`이 850건 정도인데, 신규 시스템 적용 후 300건으로 떨어졌다고 가정하자. 모니터링 시스템은 완료율이 `85% → 30%`로 급락한 것을 감지하고 `가맹점 A + 신한카드` 라우팅을 레거시로 되돌린다. 전체 가맹점이나 다른 카드사의 결제에는 영향을 주지 않는다.

## 사용자 경험과 대규모 트래픽을 최적화한다

### 첫 화면의 Preflight를 제거한다

`ENTRY`는 결제창 첫 화면의 체감 속도를 결정한다. 일반적인 CORS 요청은 Preflight와 본 요청으로 네트워크 왕복이 두 번 발생하며, 불안정한 모바일 환경에서는 이 비용이 더 커질 수 있다.

신규 시스템은 `ENTRY` API를 Simple Request로 구성해 Preflight를 피하고 첫 화면 렌더링 시간을 줄였다. 수십 밀리초의 차이라도 결제 시작 단계에서는 사용자 경험에 영향을 줄 수 있다는 판단이다.

예를 들어 Preflight에 40ms, 실제 `ENTRY` 요청에 220ms가 걸린다면 일반 요청은 약 260ms와 두 번의 네트워크 왕복이 필요하다.

```text
일반 CORS 요청: OPTIONS 40ms → POST 220ms → 총 260ms
Simple Request:                 POST 220ms → 총 220ms
```

모바일 네트워크에서는 왕복 지연이 더 커질 수 있으므로, 첫 화면에 꼭 필요하지 않은 커스텀 헤더나 복잡한 Content-Type을 피해서 Simple Request 조건을 맞춘 것이다.

### 다층 캐시를 설계한다

신규 시스템은 처음부터 병목을 관찰하고 개선할 수 있도록 Multi-Layer Cache를 설계했다. 평소보다 20배가 넘는 피크 트래픽에서도 캐시 적중률이 높아져 평균 응답시간이 오히려 줄어드는 결과를 얻었다.

트래픽 증가가 항상 응답시간 증가로 이어지는 것은 아니다. 읽기 비중이 높은 데이터에 적절한 캐시 계층을 두면 요청이 몰릴수록 재사용 효과가 커질 수 있다.

예를 들어 모든 요청이 DB에서 가맹점 설정과 카드사 정보를 조회하는 대신 여러 계층에서 결과를 재사용할 수 있다.

```text
요청
 ├─ 애플리케이션 로컬 캐시 적중 → 즉시 반환
 ├─ 로컬 캐시 미스 → 공유 캐시 조회
 └─ 공유 캐시도 미스 → DB 조회 후 두 캐시에 저장
```

대규모 할인 행사로 같은 가맹점의 결제 요청이 짧은 시간에 몰리면 첫 요청만 DB를 조회하고 뒤의 요청은 캐시를 사용한다. 트래픽이 20배가 되어도 DB 조회가 같은 비율로 늘지 않으므로 평균 응답시간을 안정적으로 유지할 수 있다.

## 개편 결과

레거시에서는 이해하기조차 어려웠던 즉시할인 기능을 서버 개발자 2명과 프론트엔드 개발자 1명이 한 달 만에 다시 개발했다. 기존 동작을 보존하면서 새로운 요구사항과 운영용 어드민까지 구현했다.

핵심 성과는 단순히 개발 기간을 줄인 것이 아니다. 조건문과 전역 상태에 묻혀 있던 기능을 도메인 객체와 명확한 인터페이스로 표현해 이후 요구사항도 빠르게 반영할 수 있는 기반을 만든 것이다.
