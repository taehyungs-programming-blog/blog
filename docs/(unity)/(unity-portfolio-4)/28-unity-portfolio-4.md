---
layout: default
title: "28. QuestManager + Event기반 프로그래밍 장/단점"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/12)

---

```csharp
public class QuestManager
{
	public Dictionary<int, Quest> AllQuests = new Dictionary<int, Quest>();

    // 주의할 점은 Event로 Quest를 처리하는 것!
	public void Init()
	{
		Managers.Game.OnBroadcastEvent -= OnHandleBroadcastEvent;
		Managers.Game.OnBroadcastEvent += OnHandleBroadcastEvent;
	}
    
    // ...

    void OnHandleBroadcastEvent(EBroadcastEventType eventType, int value)
	{
		foreach (Quest quest in AllQuests.Values)
		{
			if (quest.State == EQuestState.Processing)
				quest.OnHandleBroadcastEvent(eventType, value);
		}
	}
```

```csharp
[Serializable]
public class QuestSaveData
{
	public int TemplateId;
	public EQuestState State = EQuestState.None;
	public List<int> ProgressCount = new List<int>();
	public DateTime NextResetTime;
}
```

---

## Event기반 프로그래밍 장/단점

### 장점

* **비동기 처리**: 이벤트 기반 모델은 비동기 처리에 매우 적합합니다. 이벤트가 발생하면, 해당 이벤트를 처리하기 위한 핸들러(또는 콜백)가 비동기적으로 실행될 수 있어요. 이를 통해 애플리케이션이 특정 작업의 완료를 기다리지 않고 다른 작업을 계속 수행할 수 있습니다. 결과적으로, 시스템의 전반적인 응답성과 처리량이 향상됩니다.
* **낮은 결합도**: 이벤트 기반 시스템에서는 이벤트 발행자와 구독자가 서로에 대해 덜 의존적입니다. 발행자는 이벤트가 발생했음을 알리기만 하면 되고, 이벤트를 구독하는 객체가 누구인지, 또는 몇 개인지 알 필요가 없습니다. 이는 코드의 결합도를 낮추어 시스템의 유지보수성과 확장성을 개선합니다.
* **확장성**: 낮은 결합도 덕분에, 새로운 이벤트 핸들러나 구독자를 추가하는 것이 매우 간단합니다. 시스템의 기능을 확장하거나 변경하고 싶을 때, 기존의 코드를 수정하지 않고도 새로운 모듈을 추가할 수 있어요. 이는 큰 프로젝트나 시스템에서 특히 유리합니다.
* **재사용성과 유연성**: 이벤트 기반 아키텍처는 코드의 재사용성을 높일 수 있습니다. 특정 이벤트에 반응하는 로직을 재사용하여 다양한 컨텍스트에서 같은 기능을 제공할 수 있습니다. 또한, 시스템의 특정 부분을 변경하거나 업데이트해야 할 때, 다른 부분에 미치는 영향을 최소화하면서 유연하게 대응할 수 있습니다.
* **복잡한 흐름 관리**: 이벤트 기반 시스템은 복잡한 이벤트 흐름과 상태를 관리하기 위한 강력한 메커니즘을 제공합니다. 이벤트와 상태 머신을 결합하여, 복잡한 조건과 흐름을 명확하게 표현하고 관리할 수 있습니다.

### 단점? 주의할 점

* **이벤트 중복 구독 방지**: 기존에 동일한 핸들러가 구독되어 있는지 확인하고 제거해야한다.
* **무한 이벤트 호출 방지**: 이벤트 핸들러 내부에서 같은 이벤트를 다시 발생시키는 것은 매우 조심해야 하는 행위입니다. 이러한 방식은 의도치 않게 무한 루프를 발생시킬 수 있어요. 무한 이벤트 호출이 발생하지 않도록 로직을 세심하게 설계해야 합니다.
* **이벤트 실행 순서 관리**: 여러 핸들러가 하나의 이벤트에 구독되어 있을 때, 핸들러가 호출되는 순서를 보장할 수 없습니다. 특정 순서로 이벤트 핸들러가 실행되어야 한다면, 이를 관리할 수 있는 추가적인 로직을 구현해야 합니다.
* **이벤트 처리의 예외 안전성**: 이벤트 핸들러 내부에서 예외가 발생할 경우, 이벤트를 발행한 쪽으로 예외가 전파되지 않도록 주의해야 합니다. 이벤트 핸들러 내에서 모든 예외를 적절히 처리하거나, 이벤트 구독자에게 예외 처리의 책임을 명확히 해야 합니다.
* **디버깅의 어려움**: 이벤트 기반 시스템은 비동기적이고, 이벤트 핸들러 간에 명확한 호출 순서가 없기 때문에 디버깅이 어려울 수 있습니다. 특히, 이벤트의 순서나 상태가 예상과 다르게 작동할 때 원인을 찾기가 힘들 수 있습니다.