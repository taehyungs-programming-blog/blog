---
layout: default
title: "07. CreatureData관리"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `CreatureData.json` 를 파싱해 사용해야한다.
* 역시 이런데이터는 Addressable로 관리하는게 좋겠지?

```json
{
  "creatures": [
    {
      "DataId": 201000,
      "DescriptionTextID": "위자드",
      "PrefabLabel": "Hero",
      "ColliderOffsetX": 0.0,
      "ColliderOffstY": 0.33,
      "ColliderRadius": 0.5,
      "Mass": 3.0,
      "MaxHp": 1000.0,
      "MaxHpBonus": 2.0,
      "Atk": 25.0,
      "AtkRange": 4.0,
      "AtkBonus": 2.0,
      "Def": 0.0,
      "MoveSpeed": 15.0,
      "TotalExp": 200.0,
      "HpRate": 1.0,
      "AtkRate": 1.0,
      "DefRate": 1.0,
      "MoveSpeedRate": 1.0,
      "SkeletonDataID": "wizard_SkeletonData",
      "AnimatorName": null,
      "SkillIdList": [
        10002
      ],
      "DropItemId": 0
    },
    {
      "DataId": 201001,
      "DescriptionTextID": "주인공",
      // ...
```

---

## 자! 시작해 보자

```csharp
public override bool Init()
{
    // UI_TitleScene.cs
    
    StartLoadAssets();

    return true;
}
```

```csharp
void StartLoadAssets()
{
    // 여기서 Addressable 로드
    Managers.Resource.LoadAllAsync<Object>("PreLoad", (key, count, totalCount) =>
    {
        // ...

        if (count == totalCount)
        {
            Managers.Data.Init();
            /*
public void Init()
{
    // Json을 리드 후 CreatureDic 생성.
    CreatureDic = LoadJson<Data.CreatureDataLoader, int, Data.CreatureData>("CreatureData").MakeDict();
    EnvDic = LoadJson<Data.EnvDataLoader, int, Data.EnvData>("EnvData").MakeDict();
}
            */

            // ...
        }
    });
}
```

* 주로 여기서 사용된다 보면된다.

```csharp
public virtual void SetInfo(int templateID)
{
    DataTemplateID = templateID;

    // 데이터를 가져오고
    CreatureData = Managers.Data.CreatureDic[templateID];
    gameObject.name = $"{CreatureData.DataId}_{CreatureData.DescriptionTextID}";

    // 사용
    Collider.offset = new Vector2(CreatureData.ColliderOffsetX, CreatureData.ColliderOffstY);
    Collider.radius = CreatureData.ColliderRadius;

    // ...
```
