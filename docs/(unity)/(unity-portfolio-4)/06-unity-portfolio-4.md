---
layout: default
title: "06. Animation에 Coroutine 적용"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## C# Coroutine 예제

* 선 결론: 
    * 왜 쓰냐? **Thread를 잡지않고 대기할때 쓴다.**
* 대략 쓰기는 이렇게 쓴다.

```csharp
public class CoroutineExample : MonoBehaviour
{
    // 코루틴 시작
    void Start()
    {
        StartCoroutine(LogMessageEverySecond());
    }

    // 1초마다 메시지를 로그에 출력하는 코루틴
    IEnumerator LogMessageEverySecond()
    {
        while (true)
        {
            yield return new WaitForSeconds(1);
            Debug.Log("1초가 지났습니다!");
        }
    }
}
```

* 좀 더 현실적 예시

```csharp
public class HealthRecovery : MonoBehaviour
{
    public float recoveryDuration = 5f; // 회복 지속 시간
    public int healthPerSecond = 10;    // 초당 회복량

    private bool isRecovering = false;  // 회복 중인지 여부

    // 특정 조건(예: 트리거)에 의해 호출되는 메소드
    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player") && !isRecovering)
        {
            StartCoroutine(RecoverHealth(other.gameObject));
        }
    }

    // 회복 코루틴
    IEnumerator RecoverHealth(GameObject player)
    {
        isRecovering = true;
        float endTime = Time.time + recoveryDuration;

        while (Time.time < endTime)
        {
            player.GetComponent<PlayerHealth>().IncreaseHealth(healthPerSecond);
            yield return new WaitForSeconds(1);
        }

        isRecovering = false;
        Debug.Log("회복이 완료되었습니다!");
    }
}

// 플레이어의 체력을 관리하는 클래스 (간단한 예시)
public class PlayerHealth : MonoBehaviour
{
    public int health = 100;

    public void IncreaseHealth(int amount)
    {
        health += amount;
        // 체력이 최대치를 넘지 않도록 제한
        health = Mathf.Min(health, 100);
        Debug.Log("현재 체력: " + health);
    }
}

```

### Q) 별도의 Thread를 쓰는 것과 뭐가 다른가?

* 차이점?
    * Coroutine: 
        * 유니티의 코루틴은 **메인 스레드에서 실행**됩니다. 
        * 코루틴은 특정 지점에서 실행을 중지하고, 다음 프레임이나 지정된 시간이 지난 후에 실행을 재개할 수 있습니다. 이는 '비동기적' 작업처럼 보이지만, 실제로는 메인 스레드의 일부로 남아 있으며 다중 스레드와는 다릅니다.
    * Multi-Thread: 
        * Multi-Thread를 사용하면 작업이 완전히 **별개의 스레드에서 동시에 실행**됩니다. 
        * **Thread관리가 필요** (안정성 떨어 진다)
* 어디쓰면 좋을까?:
    * Coroutine: 
        * 간단한 지연, 연속적인 작업, 비동기적인 로직(예: 웨이트, 애니메이션, 시퀀스)에 적합합니다. 
        * 게임 루프와 밀접하게 연관된 작업에 주로 사용됩니다.
    * Multi-Thread: 
        * 무거운 계산, 데이터 로딩, 네트워크 통신 등 메인 게임 루프에 방해가 되지 않는 백그라운드 작업에 적합합니다. 

---

## Coroutine으로 Animation 출력하기

```csharp
protected Coroutine _coWait;

protected void StartWait(float seconds)
{
    // 기존 Wait(애니)가 있다면 취소
    CancelWait();
    // 들어온 초 만큼 Wait 한다
    _coWait = StartCoroutine(CoWait(seconds));
}

IEnumerator CoWait(float seconds)
{
    yield return new WaitForSeconds(seconds);
    _coWait = null;
}

protected void CancelWait()
{
    if (_coWait != null)
        StopCoroutine(_coWait);
    _coWait = null;
}
```

* 목적은 아래의 함수를 잡고있는 것이다.

```csharp
protected override void UpdateMove()
{
    // ...

        Vector3 dir = (_target.transform.position - transform.position);
        float distToTargetSqr = dir.sqrMagnitude;
        float attackDistanceSqr = AttackDistance * AttackDistance;

        if (distToTargetSqr < attackDistanceSqr)
        {
            CreatureState = ECreatureState.Skill;

            // 스킬 발동시, 움직이지 않고 여기서 2초간 잡고있는다
                // 주의할 점은 Main Thread가 그렇다고 멈춘게 아님
            StartWait(2.0f);
        }
        else
        {
            float moveDist = Mathf.Min(dir.magnitude, Time.deltaTime * Speed);
            transform.TranslateEx(dir.normalized * moveDist);

            float searchDistanceSqr = SearchDistance * SearchDistance;
            if (distToTargetSqr > searchDistanceSqr)
            {
                _destPos = _initPos;
                _target = null;
                CreatureState = ECreatureState.Move;
            }
        }
    }
```

```csharp
protected IEnumerator CoUpdateAI()
{
    // 업데이트 틱 별로 애니메이션 업데이트에도 활용
    while (true)
    {
        switch (CreatureState)
        {
            case ECreatureState.Idle:
                UpdateIdle();
                break;
            case ECreatureState.Move:
                UpdateMove();
                break;
            case ECreatureState.Skill:
                UpdateSkill();
                break;
            case ECreatureState.Dead:
                UpdateDead();
                break;
        }

        if (UpdateAITick > 0)
            yield return new WaitForSeconds(UpdateAITick);
        else
            yield return null;
    }
}
```