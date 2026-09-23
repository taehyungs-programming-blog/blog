---
layout: default
title: "대규모 정산 배치 성능 개선"
parent: "Backend"
nav_order: 4
permalink: "/(backend)/settlement-batch-performance/"
---
# 대규모 정산 배치 성능 개선

## 문제: 거래량보다 처리 시간이 더 빠르게 증가한다

정산 배치는 대량의 결제 거래를 읽어 수수료와 지급액을 계산하고, 그 결과를 DB에 저장한다.

```text
거래 조회
   ↓
계약·가맹점·환율 정보 조회
   ↓
수수료와 지급액 계산
   ↓
정산 결과 저장
```

기존 시스템은 거래량이 조금만 증가해도 처리 시간이 급격히 늘었다. 목표로 하는 규모까지 거래가 증가하면 배치를 하루 종일 실행해도 끝나지 않을 것으로 예상됐다.

신규 시스템은 Spring Batch로 전환했지만 프레임워크를 도입하는 것만으로 성능 문제가 해결되지는 않았다. 병목은 크게 두 가지였다.

1. 거래마다 반복되는 대량의 I/O
2. 단일 스레드가 순서대로 처리하는 구조

## Spring Batch의 기본 처리 구조

Spring Batch의 Chunk 방식은 일반적으로 다음 세 단계로 구성된다.

```text
ItemReader → ItemProcessor → ItemWriter
```


| 구성요소            | 역할               |
| --------------- | ---------------- |
| `ItemReader`    | 처리할 거래를 읽는다.     |
| `ItemProcessor` | 거래를 정산 결과로 변환한다. |
| `ItemWriter`    | 계산 결과를 저장한다.     |


예를 들어 Chunk Size가 1,000이면 거래 1,000건을 읽고 계산한 뒤 한 묶음으로 저장한다. 그러나 Processor 내부에서 거래마다 DB를 조회하면 Chunk를 사용하더라도 조회는 여전히 1,000번 발생한다.

```text
Chunk Size: 1,000

거래 읽기:       1회 또는 페이지 단위
계약 개별 조회:  1,000회
결과 개별 저장:  1,000회
```

따라서 프레임워크보다 Reader, Processor, Writer 내부의 I/O 방식을 먼저 살펴봐야 한다.

---

# 1. I/O 횟수 줄이기

## 개선 1: 변경되지 않는 설정을 미리 캐싱한다

기존에는 거래를 처리할 때마다 가맹점의 계약 설정을 DB에서 조회했다.

```kotlin
fun calculate(transaction: Transaction): Settlement {
    val contract = contractRepository.findByMerchantId(
        transaction.merchantId,
    )

    return settlementCalculator.calculate(transaction, contract)
}
```

거래가 100만 건이면 계약 조회도 최대 100만 번 발생할 수 있다.

```text
거래 1 → 계약 조회
거래 2 → 계약 조회
거래 3 → 계약 조회
...
거래 1,000,000 → 계약 조회
```

하지만 배치가 시작된 후에는 이번 실행에 적용할 계약 정보가 이미 확정돼 있다. 따라서 배치 시작 시 활성 계약을 한 번에 읽어 캐시에 저장한다.

```kotlin
class SettlementStepExecutionListener(
    private val contractRepository: ContractRepository,
    private val contractCache: ContractCache,
) : StepExecutionListener {

    override fun beforeStep(stepExecution: StepExecution) {
        val contracts = contractRepository.findAllActiveContracts()

        contractCache.putAll(
            contracts.associateBy { it.merchantId },
        )
    }
}
```

계산할 때는 DB가 아니라 메모리의 캐시를 조회한다.

```kotlin
fun calculate(transaction: Transaction): Settlement {
    val contract = contractCache.get(transaction.merchantId)
    return settlementCalculator.calculate(transaction, contract)
}
```

```text
개선 전: 거래 N건 → DB 조회 N번
개선 후: 배치 시작 시 DB 조회 1번 → 메모리 조회 N번
```

### 캐싱할 때 확인할 점

모든 데이터를 무조건 캐싱할 수 있는 것은 아니다.

- 배치 실행 중 설정이 변경돼도 되는가?
- 전체 설정이 메모리에 들어갈 수 있는 크기인가?
- 캐시 누락 시 실패할지 DB를 다시 조회할지 결정했는가?
- 여러 배치 인스턴스가 같은 기준 시점의 설정을 사용하는가?

정산처럼 재현 가능성이 중요한 시스템에서는 “배치 시작 시점의 설정”을 명확하게 고정하는 것이 중요하다.

## 개선 2: 건별 조회를 Bulk 조회로 바꾼다

모든 정보를 배치 시작 시 캐싱하기 어려울 수도 있다. 이때는 Processor가 처리할 묶음에 필요한 데이터를 한 번에 조회한다.

### 개선 전: 거래마다 조회

```kotlin
class TransactionProcessor(
    private val merchantRepository: MerchantRepository,
) : ItemProcessor<Transaction, SettlementResult> {

    override fun process(transaction: Transaction): SettlementResult {
        val merchant = merchantRepository.findById(transaction.merchantId)
        return calculate(transaction, merchant)
    }
}
```

거래 100만 건이면 Processor 내부의 추가 I/O도 100만 번 발생한다.

### 개선 후: 처리 단위를 묶어서 조회

```kotlin
class TransactionBatchProcessor(
    private val merchantRepository: MerchantRepository,
    private val settlementCalculator: SettlementCalculator,
) : ItemProcessor<Transactions, List<SettlementResult>> {

    override fun process(batch: Transactions): List<SettlementResult> {
        val merchantIds = batch.values
            .map { it.merchantId }
            .distinct()

        val merchantsById = merchantRepository
            .findAllByIds(merchantIds)
            .associateBy { it.id }

        return batch.values.map { transaction ->
            val merchant = merchantsById[transaction.merchantId]
                ?: error("가맹점 정보 없음")

            settlementCalculator.calculate(transaction, merchant)
        }
    }
}
```

Chunk Size가 1,000이라면 I/O 횟수는 다음처럼 줄어든다.

```text
거래 건수: 1,000,000
Chunk Size: 1,000

건별 조회: 1,000,000회
Bulk 조회: 약 1,000회
```

즉, 조회 횟수가 데이터 개수 `N`이 아니라 청크 반복 횟수 `N / Chunk Size`에 가까워진다.

### Bulk 크기도 무한히 키울 수는 없다

Chunk Size가 클수록 I/O 횟수는 감소하지만 다음 비용은 증가한다.

- 메모리 사용량
- SQL의 `IN` 조건 크기
- 한 트랜잭션이 점유하는 시간
- 실패 시 재처리해야 하는 범위

따라서 실제 DB와 데이터 크기를 측정해 적절한 Chunk Size를 찾아야 한다.

## 개선 3: JDBC Batch Insert로 저장한다

기존에는 정산 결과마다 INSERT 쿼리를 한 번씩 실행했다.

```kotlin
results.forEach { result ->
    jdbcTemplate.update(
        insertSql,
        result.id,
        result.amount,
        result.fee,
    )
}
```

결과 1,000건을 저장하면 애플리케이션과 DB 사이의 통신도 1,000번 발생한다.

JDBC Batch Insert는 여러 건의 파라미터를 묶어 DB에 전달한다.

```kotlin
jdbcTemplate.batchUpdate(
    """
    INSERT INTO settlement_result (id, amount, fee)
    VALUES (?, ?, ?)
    """.trimIndent(),
    results,
) { statement, result ->
    statement.setString(1, result.id)
    statement.setBigDecimal(2, result.amount)
    statement.setBigDecimal(3, result.fee)
}
```

```text
개별 INSERT
1,000건 → DB 통신 1,000회

Batch Insert
1,000건 → 묶음 단위로 DB 통신
```

Batch Insert가 SQL 문법상 반드시 한 개의 INSERT문으로 변환된다는 뜻은 아니다. 중요한 점은 JDBC 드라이버가 여러 실행을 묶어 전달해 네트워크 왕복과 DB 호출 비용을 줄인다는 것이다.

---

# 2. 병렬 처리로 대기 시간을 줄이기

I/O 횟수를 줄여 한 건의 처리 속도를 높여도 단일 스레드에는 물리적인 한계가 있다. 서로 독립적인 작업은 동시에 수행해 전체 시간을 줄일 수 있다.

## 개선 1: 독립적인 외부 API를 병렬 호출한다

가맹점, 환율, 수수료 정보를 각각 다른 API에서 가져온다고 가정하자. 각 요청에 10ms가 걸리고 순서대로 호출하면 총 30ms가 필요하다.

```kotlin
val merchant = merchantApi.getInfo(merchantId) // 10ms
val rate = exchangeApi.getRate(currency)       // 10ms
val feeRule = feeApi.getRule(merchantId)       // 10ms

// 약 30ms
```

세 요청이 서로의 결과에 의존하지 않는다면 동시에 시작할 수 있다.

```kotlin
val merchantFuture = apiExecutor.submit<Merchant> {
    merchantApi.getInfo(merchantId)
}

val rateFuture = apiExecutor.submit<BigDecimal> {
    exchangeApi.getRate(currency)
}

val feeRuleFuture = apiExecutor.submit<FeeRule> {
    feeApi.getRule(merchantId)
}

val merchant = merchantFuture.get()
val rate = rateFuture.get()
val feeRule = feeRuleFuture.get()

// 가장 늦은 요청을 기준으로 약 10ms
```

```text
순차 호출: 10ms + 10ms + 10ms = 약 30ms
병렬 호출: max(10ms, 10ms, 10ms) = 약 10ms
```

`Future.get()`에서 Blocking이 발생하지만 잘못된 것은 아니다. 계산을 계속하려면 세 응답이 모두 필요하므로 결과를 기다려야 한다. 중요한 것은 대기 시간을 순서대로 더하지 않고 겹치는 것이다.

### 병렬 호출의 주의점

병렬화하면 외부 시스템에 순간적으로 더 많은 요청을 보낼 수 있다.

- 스레드 풀 크기 제한
- API 타임아웃
- 호출 실패와 재시도 정책
- 외부 시스템의 처리 한도
- 전체 배치 동시성과 API 호출 동시성의 곱

예를 들어 배치 스레드 20개가 각각 API 3개를 동시에 호출하면 최대 60개의 요청이 발생할 수 있다. 무제한 병렬화는 오히려 외부 시스템과 자신의 스레드 풀을 고갈시킬 수 있다.

## 개선 2: Multi-threaded Step으로 거래를 병렬 처리한다

Spring Batch의 Multi-threaded Step을 사용하면 여러 스레드가 Chunk를 동시에 처리할 수 있다.

```text
단일 스레드
Thread 1: Chunk 1 → Chunk 2 → Chunk 3 → Chunk 4

멀티 스레드
Thread 1: Chunk 1
Thread 2: Chunk 2
Thread 3: Chunk 3
Thread 4: Chunk 4
```

하지만 여러 스레드가 같은 Reader에서 데이터를 가져오면 안전하지 않을 수 있다.

```text
Thread 1과 Thread 2가 같은 거래를 읽음 → 중복 정산
두 스레드가 커서를 잘못 공유함       → 일부 거래 누락
```

Reader에 동기화를 적용하면 중복과 누락은 막을 수 있지만, 읽는 순간에는 한 스레드만 접근할 수 있어 병렬 처리 효과가 줄어든다.

```kotlin
@Synchronized
fun read(): Transaction? {
    return reader.read()
}
```

## 개선 3: 모듈러 연산으로 처리 대상을 미리 나눈다

여러 스레드가 같은 Reader를 경쟁하지 않도록 각 스레드가 읽을 데이터 범위를 처음부터 분리한다.

거래의 숫자 ID를 스레드 수로 나눈 나머지를 사용할 수 있다.

```text
스레드 수: 4

Thread 0: transaction_id % 4 = 0
Thread 1: transaction_id % 4 = 1
Thread 2: transaction_id % 4 = 2
Thread 3: transaction_id % 4 = 3
```

각 스레드는 자신의 나머지 값에 해당하는 데이터만 조회한다.

```sql
SELECT *
FROM transaction
WHERE MOD(transaction_id, :threadCount) = :threadIndex
ORDER BY transaction_id;
```

예를 들어 거래 ID가 1부터 12까지라면 다음과 같이 나뉜다.


| 스레드      | 처리할 거래 ID |
| -------- | --------- |
| Thread 0 | 4, 8, 12  |
| Thread 1 | 1, 5, 9   |
| Thread 2 | 2, 6, 10  |
| Thread 3 | 3, 7, 11  |


각 거래는 정확히 한 스레드에만 포함된다. 따라서 스레드 간 경합 없이 중복과 누락을 방지할 수 있다.

### 모듈러 분할의 전제

모듈러 연산을 사용하려면 다음 조건을 확인해야 한다.

- 분할 키가 안정적이고 모든 거래에 존재하는가?
- 키 값이 특정 나머지에 편중되지 않는가?
- 재실행할 때 동일한 분할 규칙을 재현할 수 있는가?
- `MOD(column, ...)` 조건이 DB 조회 병목을 만들지 않는가?

데이터 분포가 불균형하면 특정 스레드에 작업이 몰릴 수 있다. 또한 DB 종류와 실행 계획에 따라 모듈러 조건이 인덱스를 효율적으로 사용하지 못할 수 있으므로 실제 실행 계획과 처리량을 측정해야 한다.

## 각 개선이 줄이는 비용


| 개선 방법               | 줄이는 비용                |
| ------------------- | --------------------- |
| 설정 캐싱               | 반복적인 설정 DB 조회         |
| Bulk 조회             | Processor의 건별 DB 왕복   |
| JDBC Batch Insert   | Writer의 건별 DB 왕복      |
| 외부 API 병렬 호출        | 독립적인 네트워크 대기 시간의 합    |
| Multi-threaded Step | 단일 스레드의 처리 시간 한계      |
| 모듈러 데이터 분할          | Reader 공유로 인한 경합과 동기화 |


## 전체 처리 흐름

```text
배치 시작
   ↓
변경되지 않는 설정을 한 번에 조회해 캐싱
   ↓
모듈러 기준으로 스레드별 처리 대상 분리
   ↓
각 스레드가 거래를 Chunk 단위로 읽음
   ↓
필요한 연관 데이터를 Bulk 조회
   ↓
독립적인 외부 API는 제한된 동시성으로 병렬 호출
   ↓
메모리에서 정산 금액 계산
   ↓
결과를 JDBC Batch Insert로 저장
```
