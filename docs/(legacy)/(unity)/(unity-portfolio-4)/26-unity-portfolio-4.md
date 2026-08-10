---
layout: default
title: "26. Item System"
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

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/11)

---

* TODO: Item이 떨어지면 어떻게 처리할 것인가?
    * ItemHolder
    * ItemData
        * ItemDataLoader가 어떻게 동작하나 확인
    * DropTable
        * DropTableDataLoader

```csharp
private static List<LoaderData> ParseExcelDataToList<LoaderData>(string filename) where LoaderData : new()
{
    List<LoaderData> loaderDatas = new List<LoaderData>();

    string[] lines = File.ReadAllText($"{Application.dataPath}/@Resources/Data/ExcelData/{filename}Data.csv").Split("\n");

    for (int l = 1; l < lines.Length; l++)
    {
        string[] row = lines[l].Replace("\r", "").Split(',');
        if (row.Length == 0)
            continue;
        if (string.IsNullOrEmpty(row[0]))
            continue;

        LoaderData loaderData = new LoaderData();
        var fields = GetFieldsInBase(typeof(LoaderData));

        for (int f = 0; f < fields.Count; f++)
        {
            FieldInfo field = loaderData.GetType().GetField(fields[f].Name);
            Type type = field.FieldType;

            // 이곳
            if (field.HasAttribute(typeof(NonSerializedAttribute)))
                continue;
```

```csharp
public class MonsterData : CreatureData
{
    public int DropItemId;

    [NonSerialized]
    public DropTableData DropTable;
}
```

* GetRandomReward
* ItemHolder
* Item이 어떻게 떨어지나 확인

