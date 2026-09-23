---
layout: default
title: "거대한 공통 쿼리에서 비즈니스 로직 분리하기"
parent: "Backend"
nav_order: 1
---
# 거대한 공통 쿼리에서 비즈니스 로직 분리하기

## 문제: 하나의 SQL이 너무 많은 일을 한다

정산 시스템은 거래 한 건을 처리할 때 다음과 같은 일을 수행한다.

1. 거래를 조회한다.
2. 가맹점의 계약 조건을 조회한다.
3. 해외 결제라면 환율을 적용한다.
4. 결제수단과 계약에 맞는 수수료를 계산한다.
5. VAT를 계산한다.
6. 최종 지급액을 계산한다.

예를 들어 다음 거래를 정산한다고 가정하자.

- 결제 금액: 100,000원
- 결제수단: 해외 체크카드
- 가맹점 등급: VIP
- 해외 체크카드 수수료율: 2.2%
- 계약에 따라 VAT 포함 여부가 달라짐

개념적인 계산 과정은 다음과 같다.

```text
환산 금액 = 결제 금액 × 환율
수수료 = 환산 금액 × 적용 수수료율
지급액 = 환산 금액 - 수수료 - VAT
```

레거시 시스템에서는 조회와 계산이 하나의 거대한 SQL에 들어 있었다.

```sql
SELECT
    CASE
        WHEN transaction.currency = 'USD'
            THEN transaction.amount * exchange_rate.rate
        ELSE transaction.amount
    END AS converted_amount,
    CASE
        WHEN merchant.grade = 'VIP'
             AND transaction.payment_method = 'CHECK_CARD'
            THEN transaction.amount * 0.022
        WHEN transaction.payment_method = 'CARD'
            THEN transaction.amount * 0.03
        ELSE 0
    END AS fee,
    CASE
        WHEN contract.vat_included = 'Y' THEN /* VAT 포함 계산 */
        ELSE /* VAT 별도 계산 */
    END AS vat
FROM transaction
JOIN merchant ON merchant.id = transaction.merchant_id
JOIN contract ON contract.merchant_id = merchant.id
LEFT JOIN exchange_rate ON exchange_rate.currency = transaction.currency;
```

실제 시스템에는 할부, 취소, 부분 취소, 결제수단, 프로모션, 가맹점별 예외 계약 같은 조건이 계속 추가된다. 그러면 `JOIN`, 서브쿼리, `UNION ALL`, 중첩된 `CASE WHEN`과 `DECODE`가 얽혀 SQL 자체가 하나의 거대한 프로그램이 된다.

## 왜 문제가 되는가

### 비즈니스 규칙을 파악하기 어렵다

“해외 체크카드 수수료율은 얼마인가?”라는 질문에 답하려면 가맹점 조인, 계약 서브쿼리, 해외 결제 조건, 결제수단 조건과 예외 조건을 모두 따라가야 한다. 비즈니스 규칙보다 SQL 실행 구조를 먼저 해석해야 한다.

### 작은 변경의 영향 범위가 크다

해외 체크카드 수수료율 하나를 바꾸더라도 다음을 확인해야 한다.

- VIP 우대 조건과 충돌하지 않는가?
- 국내 체크카드에도 적용되지 않는가?
- 환전 전 금액과 환전 후 금액 중 무엇을 기준으로 하는가?
- VAT 계산에도 영향을 주는가?

모든 거래가 같은 쿼리를 사용하므로 작은 변경도 여러 거래 유형을 다시 검증해야 한다.

### 단위 테스트가 어렵다

수수료 공식 하나를 검증하려 해도 거래, 가맹점, 계약, 환율 등의 테스트 데이터를 DB에 준비하고 전체 쿼리를 실행해야 한다. 작은 규칙을 검증하는 데 통합 테스트 수준의 준비가 필요하다.

## 해결: 역할과 변경 이유에 따라 분리한다

핵심은 클래스를 많이 만드는 것이 아니다. **서로 다른 이유로 변경되는 로직을 한곳에 섞지 않는 것**이다.

```text
SettlementService
├── TransactionRepository    거래 조회
├── ContractRepository       계약 조회
├── ExchangeRateConverter    환전
├── FeeCalculator            수수료 계산
├── VatCalculator            VAT 계산
└── SettlementRepository     결과 저장
```

각 구성요소는 한 가지 역할을 담당하고, 그 역할의 요구사항이 바뀔 때만 수정된다.


| 요구사항 변경      | 수정 대상                   |
| ------------ | ----------------------- |
| 거래 DB 구조 변경  | `TransactionRepository` |
| 계약 조회 방식 변경  | `ContractRepository`    |
| 환율 적용 공식 변경  | `ExchangeRateConverter` |
| 수수료 정책 변경    | `FeeCalculator`         |
| VAT 계산 방식 변경 | `VatCalculator`         |
| 전체 처리 순서 변경  | `SettlementService`     |


### 데이터 조회와 계산을 구분한다

Repository는 DB 접근을 담당한다.

```kotlin
interface ContractRepository {
    fun findByMerchantId(merchantId: Long): Contract
}
```

계산 클래스는 이미 조회된 값을 전달받아 계산만 한다.

```kotlin
class FeeCalculator {
    fun calculate(amount: Money, contract: Contract): Money =
        amount
            .times(contract.feeRate)
            .plus(contract.fixedFee)
}
```

전체 흐름은 서비스가 조립한다.

```kotlin
class SettlementService(
    private val transactionRepository: TransactionRepository,
    private val contractRepository: ContractRepository,
    private val exchangeRateRepository: ExchangeRateRepository,
    private val settlementCalculator: SettlementCalculator,
    private val settlementRepository: SettlementRepository,
) {
    fun settle(transactionId: Long): Settlement {
        val transaction = transactionRepository.find(transactionId)
        val contract = contractRepository.findByMerchantId(transaction.merchantId)
        val exchangeRate = exchangeRateRepository.find(transaction.currency)

        val settlement = settlementCalculator.calculate(
            transaction = transaction,
            contract = contract,
            exchangeRate = exchangeRate,
        )

        settlementRepository.save(settlement)
        return settlement
    }
}
```

이 구조에서 계산기는 DB가 어디에 있는지 알지 못한다. 입력값을 받아 비즈니스 규칙에 따라 결과만 만든다. 따라서 수수료 정책을 바꿀 때는 수수료 계산 코드와 관련 테스트만 집중해서 변경할 수 있다.

## 각 클래스가 각각 DB를 조회하는 것은 아니다

분리했다는 말은 모든 클래스가 DB를 한 번씩 조회한다는 뜻이 아니다.


| 종류      | 예시                                                        | DB 접근           |
| ------- | --------------------------------------------------------- | --------------- |
| 데이터 접근  | `TransactionRepository`, `ContractRepository`             | 함               |
| 비즈니스 계산 | `FeeCalculator`, `VatCalculator`, `ExchangeRateConverter` | 하지 않음           |
| 흐름 조정   | `SettlementService`                                       | Repository를 호출함 |


계산기 내부에서 거래마다 Repository를 호출하면 I/O가 숨겨지고 대량 처리 시 성능 문제가 생긴다.

```kotlin
// 피해야 할 구조
class FeeCalculator(
    private val contractRepository: ContractRepository,
) {
    fun calculate(transaction: Transaction): Money {
        val contract = contractRepository.findByMerchantId(transaction.merchantId)
        return transaction.amount.times(contract.feeRate)
    }
}
```

거래 1,000건마다 위 메서드를 호출하면 계약 조회도 1,000번 발생할 수 있다.

## 배치에서는 한꺼번에 조회하고 메모리에서 계산한다

대량 정산에서는 필요한 데이터를 청크 단위로 조회한다.

```kotlin
fun process(transactions: List<Transaction>): List<Settlement> {
    val merchantIds = transactions
        .map { it.merchantId }
        .distinct()

    val contractsByMerchantId = contractRepository
        .findAllByMerchantIds(merchantIds)
        .associateBy { it.merchantId }

    return transactions.map { transaction ->
        settlementCalculator.calculate(
            transaction = transaction,
            contract = contractsByMerchantId.getValue(transaction.merchantId),
        )
    }
}
```

```text
건별 조회
거래 1,000건 → 계약 조회 최대 1,000번

Bulk 조회
거래 1,000건 → 계약 조회 1번 → 메모리 계산 1,000번
```

설정 정보처럼 배치 실행 중 변하지 않는 데이터는 배치 시작 시 한 번 조회해 캐싱할 수도 있다. 결과 역시 한 건씩 저장하는 대신 JDBC Batch Insert로 묶어서 저장한다.

```text
거래를 청크 단위로 조회
        ↓
필요한 계약·가맹점 정보를 Bulk 조회
        ↓
DB 접근 없이 메모리에서 계산
        ↓
계산 결과를 Batch Insert
```
