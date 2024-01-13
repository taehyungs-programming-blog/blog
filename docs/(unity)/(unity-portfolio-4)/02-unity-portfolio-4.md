---
layout: default
title: "02. Resource Manager"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Resource Manager의 필요성?

* 우선 아래 좀 독특한 방법으로 Resource를 관리하는데 왜 이렇게 하는지 이해하는 것이 중요하다
* App을 배포시 모든 파일을 App에 포함하면 배포가 어려워진다.
    * 심사기간이 있기 때문
* 따라서 업데이트 할 부분만 별도로 나눠서 Resource로 관리하는게 중요하다
* 업데이트할 부분만 별도로 나눠서 관리하는 패키지가 Addressable이다.

---

## 만들어 보자

* Windows -> Asset Manager -> Addresable -> Group
    * 자! 이제 Resource를 Group으로 관리하게 된다.

```csharp
public class TitleScene : BaseScene
{
	// ...

	void StartLoadAssets()
    {
        // TitleScene에서 PreLoad인 label의 Resource를 모두 로드
        Managers.Resource.LoadAllAsync<Object>("PreLoad", (key, count, totalCount) =>
        {
			Debug.Log($"{key} {count}/{totalCount}");

			if (count == totalCount)
			{
				
			}
		});
    }

// ...
```

```csharp
public class ResourceManager
{
    // ...

	#region Load Resource
	// ...
	#endregion

	#region Addressable
	private void LoadAsync<T>(string key, Action<T> callback = null) where T : UnityEngine.Object
	{
		// Cache
		if (_resources.TryGetValue(key, out Object resource))
		{
			callback?.Invoke(resource as T);
			return;
		}

		string loadKey = key;
        // sprite만 이름을 별도로 처리한다 (버그...)
		if (key.Contains(".sprite"))
			loadKey = $"{key}[{key.Replace(".sprite", "")}]";

		var asyncOperation = Addressables.LoadAssetAsync<T>(loadKey);
		asyncOperation.Completed += (op) =>
		{
			_resources.Add(key, op.Result);
			_handles.Add(key, asyncOperation);
			callback?.Invoke(op.Result);
		};
	}

	public void LoadAllAsync<T>(string label, Action<string, int, int> callback) where T : UnityEngine.Object
	{
		var opHandle = Addressables.LoadResourceLocationsAsync(label, typeof(T));
		opHandle.Completed += (op) =>
		{
			int loadCount = 0;
			int totalCount = op.Result.Count;

			foreach (var result in op.Result)
			{
				if (result.PrimaryKey.Contains(".sprite"))
				{
					LoadAsync<Sprite>(result.PrimaryKey, (obj) =>
					{
						loadCount++;
						callback?.Invoke(result.PrimaryKey, loadCount, totalCount);
					});
				}
				else
				{
					LoadAsync<T>(result.PrimaryKey, (obj) =>
					{
						loadCount++;
						callback?.Invoke(result.PrimaryKey, loadCount, totalCount);
					});
				}
			}
		};
	}

	// ...
	#endregion
}
```

