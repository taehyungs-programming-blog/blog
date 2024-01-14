---
layout: default
title: "04. DataManager"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## csv to json

* Menu에 Tools아래 ParseExcel을 추가하려 한다

```csharp
public class DataTransformer : EditorWindow
{
#if UNITY_EDITOR
	[MenuItem("Tools/ParseExcel %#K")]
	public static void ParseExcelDataToJson()
	{
		ParseExcelDataToJson<TestDataLoader, TestData>("Test");

		Debug.Log("DataTransformer Completed");
	}
```

* 내부 함수는 간단하기에 생략.

---

## json read

```csharp
namespace Data
{
	// 이런 데이터를
	[Serializable]
	public class TestData
	{
		public int Level;
		public int Exp;
		public List<int> Skills;
		public float Speed;
		public string Name;
	}

    // List로 갖겠다
	[Serializable]
	public class TestDataLoader : ILoader<int, TestData>
	{
		public List<TestData> tests = new List<TestData>();

		public Dictionary<int, TestData> MakeDict()
		{
			Dictionary<int, TestData> dict = new Dictionary<int, TestData>();
			foreach (TestData testData in tests)
				dict.Add(testData.Level, testData);

			return dict;
		}
	}
}
```

```csharp
public class DataManager
{
	public Dictionary<int, Data.TestData> TestDic { get; private set; } = new Dictionary<int, Data.TestData>();

	public void Init()
	{
		TestDic = LoadJson<Data.TestDataLoader, int, Data.TestData>("TestData").MakeDict();
	}

	private Loader LoadJson<Loader, Key, Value>(string path) where Loader : ILoader<Key, Value>
	{
		TextAsset textAsset = Managers.Resource.Load<TextAsset>(path);
		return JsonConvert.DeserializeObject<Loader>(textAsset.text);
	}
}
```