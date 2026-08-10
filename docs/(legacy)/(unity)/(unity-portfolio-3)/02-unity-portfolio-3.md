---
layout: default
title: "2. Manager"
parent: "(포트폴리오 3 - 뱀서 모작)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity_Portfolio_3/tree/2)

## Manager만들기

```csharp
// SingleTone으로 생성

public static Managers Instance
{
    get
    {
        if (s_init == false)
        {
            s_init = true;

            GameObject go = GameObject.Find("@Managers");
            if (go == null)
            {
                go = new GameObject() { name = "@Managers" };
                go.AddComponent<Managers>();
            }

            DontDestroyOnLoad(go);
            s_instance = go.GetComponent<Managers>();
        }

        return s_instance;
    }
}
```

* Manager는 크게 Content, Core 두 가지로 나눌예정

* 주의할 점은 이제 Player의 MoveDir을 GameManager에서 관리할 것이며
* 관리 방법은 Action(Delegate)를 통해서 한다

```csharp
public class GameManager
{
	public PlayerController Player { get { return Managers.Object?.Player; } }

	Vector2 _moveDir;

	public event Action<Vector2> OnMoveDirChanged;

	public Vector2 MoveDir
	{
		get { return _moveDir; }
		set
		{ 
			_moveDir = value;
			OnMoveDirChanged?.Invoke(_moveDir);
		}
	}
}
```

```csharp
public class PlayerController : CreatureController
{
    Vector2 _moveDir = Vector2.zero;
    float _speed = 5.0f;

    public Vector2 MoveDir
    {
        get { return _moveDir; }
        set { _moveDir = value.normalized; }
    }

    void Start()
    {
        Managers.Game.OnMoveDirChanged += HandleOnMoveDirChanged;

	}

	void OnDestroy()
	{
        if (Managers.Game != null)
	    	Managers.Game.OnMoveDirChanged -= HandleOnMoveDirChanged;
	}

	void HandleOnMoveDirChanged(Vector2 dir)
    {
        _moveDir = dir;
    }

    // ...
```

---

## Addressable

* 우선 Package Manager에서 Addressables를 다운받자
* Window -> Asset Manager -> Group 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-3/p3-2-1.png"/>
</p>

```csharp
public class GameScene : MonoBehaviour
{
    void Start()
    {
        Managers.Resource.LoadAllAsync<GameObject>("Prefabs", (key, count, totalCount) =>
        {
            Debug.Log($"{key} {count}/{totalCount}");

			if (count == totalCount)
			{
				Managers.Resource.LoadAllAsync<TextAsset>("Data", (key3, count3, totalCount3) =>
				{
					if (count3 == totalCount3)
					{
						//StartLoaded();
						StartLoaded2();
					}
				});
			}
        });
	}

    // ...
```

---

## ObjectManager

* 크리쳐의 추가/삭제를 담당한다

```csharp
public class ObjectManager
{
	public PlayerController Player { get; private set; }
	public HashSet<MonsterController> Monsters { get; } = new HashSet<MonsterController>();
	public HashSet<ProjectileController> Projectiles { get; } = new HashSet<ProjectileController>();

	public T Spawn<T>(int templateID = 0) where T : BaseController
	{
		System.Type type = typeof(T);

		if (type == typeof(PlayerController))
		{
			// TODO : Data
			GameObject go = Managers.Resource.Instantiate("Slime_01.prefab", pooling: true);
			go.name = "Player";

			PlayerController pc = go.GetOrAddComponent<PlayerController>();
			Player = pc;

			return pc as T;
		}
		else if (type == typeof(MonsterController))
		{
			string name = (templateID == 0 ? "Goblin_01" : "Snake_01");
			GameObject go = Managers.Resource.Instantiate(name + ".prefab", pooling: true);

			MonsterController mc = go.GetOrAddComponent<MonsterController>();
			Monsters.Add(mc);

			return mc as T;
		}

		return null;
	}

	public void Despawn<T>(T obj) where T : BaseController
	{
		System.Type type = typeof(T);

		if (type == typeof(PlayerController))
		{
			// ?
		}
		else if (type == typeof(MonsterController))
		{
			Monsters.Remove(obj as MonsterController);
			Managers.Resource.Destroy(obj.gameObject);
		}
		else if (type == typeof(ProjectileController))
		{
			Projectiles.Remove(obj as ProjectileController);
			Managers.Resource.Destroy(obj.gameObject);
		}
	}
}
```

---

### Creature의 충돌

* 출돌을 위해서 Prefab에서 Box Collider 2D, Rigidbody 2D를 붙인다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-3/p3-2-2.png"/>
</p>

---

## PoolManager

```csharp
class Pool
{
    GameObject _prefab;

    // Unity에서 지원하는 Pool Object이다
    IObjectPool<GameObject> _pool;

    Transform _root;
    Transform Root
    {
        get 
        {
            if (_root == null)
            {
                GameObject go = new GameObject() { name = $"{_prefab.name}Root" };
                _root = go.transform;
            }

            return _root;
        }
    }

    public Pool(GameObject prefab)
    {
        _prefab = prefab;
        _pool = new ObjectPool<GameObject>(OnCreate, OnGet, OnRelease, OnDestroy);
	}

    public void Push(GameObject go)
    {
        _pool.Release(go);
    }

    public GameObject Pop()
    {
        return _pool.Get();
    }

	#region Funcs

    GameObject OnCreate()
    {
        GameObject go = GameObject.Instantiate(_prefab);
        go.transform.parent = Root;
        go.name = _prefab.name;
        return go;
    }

	void OnGet(GameObject go)
	{
		go.SetActive(true);
	}

	void OnRelease(GameObject go)
	{
		go.SetActive(false);
	}

	void OnDestroy(GameObject go)
	{
		GameObject.Destroy(go);
	}
	#endregion
}

public class PoolManager
{
    Dictionary<string, Pool> _pools = new Dictionary<string, Pool>();

    public GameObject Pop(GameObject prefab)
    {
        if (_pools.ContainsKey(prefab.name) == false)
            CreatePool(prefab);

        return _pools[prefab.name].Pop();
	}

    public bool Push(GameObject go)
    {
        if (_pools.ContainsKey(go.name) == false)
            return false;

        _pools[go.name].Push(go);
        return true;
    }

    void CreatePool(GameObject prefab)
    {
        Pool pool = new Pool(prefab);
        _pools.Add(prefab.name, pool);
    }
}
```

---

## DataManager

```csharp
public interface ILoader<Key, Value>
{
	Dictionary<Key, Value> MakeDict();
}

public class DataManager
{
	public Dictionary<int, Data.PlayerData> PlayerDic { get; private set; } = new Dictionary<int, Data.PlayerData>();

	public void Init()
	{
		//PlayerDic = LoadJson<Data.PlayerDataLoader, int, Data.PlayerData>("PlayerData.json").MakeDict();
		PlayerDic = LoadXml<Data.PlayerDataLoader, int, Data.PlayerData>("PlayerData.xml").MakeDict();
	}

	Loader LoadJson<Loader, Key, Value>(string path) where Loader : ILoader<Key, Value>
	{
		TextAsset textAsset = Managers.Resource.Load<TextAsset>($"{path}");
		return JsonUtility.FromJson<Loader>(textAsset.text);
	}

	Item LoadSingleXml<Item>(string name)
	{
		XmlSerializer xs = new XmlSerializer(typeof(Item));
		TextAsset textAsset = Managers.Resource.Load<TextAsset>(name);
		using (MemoryStream stream = new MemoryStream(System.Text.Encoding.UTF8.GetBytes(textAsset.text)))
			return (Item)xs.Deserialize(stream);
	}

	Loader LoadXml<Loader, Key, Item>(string name) where Loader : ILoader<Key, Item>, new()
	{
		XmlSerializer xs = new XmlSerializer(typeof(Loader));
		TextAsset textAsset = Managers.Resource.Load<TextAsset>(name);
		using (MemoryStream stream = new MemoryStream(System.Text.Encoding.UTF8.GetBytes(textAsset.text)))
			return (Loader)xs.Deserialize(stream);
	}
}
```

```xml
<!-- 이런식으로 관리할 예정 -->

<?xml version="1.0" encoding="utf-8"?>
<PlayerDatas>
	<PlayerData level="1" maxHp="100" attack="10" totalExp="100">
	</PlayerData>
	<PlayerData level="2" maxHp="200" attack="10" totalExp="100">
	</PlayerData>
	<PlayerData level="3" maxHp="300" attack="10" totalExp="100">
	</PlayerData>
</PlayerDatas>
```