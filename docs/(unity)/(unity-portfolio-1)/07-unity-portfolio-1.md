---
layout: default
title: "7. 디스폰"
parent: "(포트폴리오 1 - 미니RPG)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/14.10)

```csharp
public class GameManagerEx
{
    GameObject _player;
    //Dictionary<int, GameObject> _players = new Dictionary<int, GameObject>();
    HashSet<GameObject> _monsters = new HashSet<GameObject>();

    public Action<int> OnSpawnEvent;

    public GameObject GetPlayer() { return _player; }

    public GameObject Spawn(Define.WorldObject type, string path, Transform parent = null)
    {
        GameObject go = Managers.Resource.Instantiate(path, parent);

        switch (type)
        {
            case Define.WorldObject.Monster:
                _monsters.Add(go);
                if (OnSpawnEvent != null)
                    OnSpawnEvent.Invoke(1);
                break;
            case Define.WorldObject.Player:
                _player = go;
                break;
        }

        return go;
    }

    public Define.WorldObject GetWorldObjectType(GameObject go)
    {
        BaseController bc = go.GetComponent<BaseController>();
        if (bc == null)
            return Define.WorldObject.Unknown;

        return bc.WorldObjectType;
    }

    public void Despawn(GameObject go)
    {
        Define.WorldObject type = GetWorldObjectType(go);

        switch (type)
        {
            case Define.WorldObject.Monster:
                {
                    if (_monsters.Contains(go))
                    {
                        _monsters.Remove(go);
                        if (OnSpawnEvent != null)
							OnSpawnEvent.Invoke(-1);
					}   
                }
                break;
            case Define.WorldObject.Player:
                {
					if (_player == go)
						_player = null;
				}
                break;
        }

        Managers.Resource.Destroy(go);
    }
}

```