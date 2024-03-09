---
layout: default
title: "16. Damage 표현하기"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/6)

---

## Damage Font

* DOTween lib를 쓴다

```csharp
private void DoAnimation()
{
    // 새로운 애니메이션 시퀀스 seq를 생성
    Sequence seq = DOTween.Sequence();

    // 객체의 크기를 (0, 0, 0)으로 초기화
    transform.localScale = new Vector3(0, 0, 0);

    // 크기 및 위치 변경 애니메이션
    // 앞선 애니메이션 이후, 객체의 크기를 0.3초 동안 원래 크기(1.0배)로 복원하는 애니메이션을 시퀀스에 추가
        // Ease.InOutBounce 효과
    seq.Append(transform.DOScale(1.3f, 0.3f).SetEase(Ease.InOutBounce))
        // 텍스트의 투명도를 0.3초 동안 0으로 변경하여 완전히 투명
        .Join(transform.DOMove(transform.position + Vector3.up, 0.3f).SetEase(Ease.Linear))
        // 크기 조정 및 페이드 아웃 애니메이션:
        .Append(transform.DOScale(1.0f, 0.3f).SetEase(Ease.InOutBounce))
        .Join(transform.GetComponent<TMP_Text>().DOFade(0, 0.3f).SetEase(Ease.InQuint))
        // 애니메이션 완료 후 처리
        .OnComplete(() =>
        {
            Managers.Resource.Destroy(gameObject);
        });
}
```

---

## 맞았을때 깜빡이는 효과

* `HurtFlashEffect`