---
layout: default
title: "27. Inventory"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/12)

---

* 우선 역시 `InventoryManager`를 통해 Inventory를 관리한다.

```csharp
public class InventoryManager
{
	public readonly int DEFAULT_INVENTORY_SLOT_COUNT = 30;

    // 내 모든 Item
	public List<Item> AllItems { get; } = new List<Item>();

	// 
	Dictionary<int /*EquipSlot*/, Item> EquippedItems = new Dictionary<int, Item>();
	List<Item> InventoryItems = new List<Item>();
	List<Item> WarehouseItems = new List<Item>();

    // ...
```

```csharp
public class Item
{
    // ...
```

---

* 데이터 로딩

```csharp
[Serializable]
public class ItemSaveData
{
	public int InstanceId;
	public int DbId;
	public int TemplateId;
	public int Count;
	public int EquipSlot;
	public int EnchantCount;
}
```

```csharp
public bool LoadGame()
{
    // ...

    // Item
    {
        Managers.Inventory.Clear();

        foreach (ItemSaveData itemSaveData in data.Items)
        {
            Managers.Inventory.AddItem(itemSaveData);
        }
    }

    // ...
```

