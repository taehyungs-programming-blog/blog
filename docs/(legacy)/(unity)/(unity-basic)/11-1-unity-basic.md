---
layout: default
title: "11. Pool Manager"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/11.1)

* Resource(Prefab)을 Scene에 올리는 과정은 엄청난 **리소스의 소모**가 일어난다.
    * 대략설명하면 미리 만들어 Scene에 올려두고 **재활용**하는 기법(Object Pooling)이다.
* Poolable - 사실 모든 Object가 Pooling이 필요하진 않다. (Camera, EventSystem은 Pool할 필요가 없음) Pooling 할지 말지를 체크

```csharp
public class ResourceManager
{
    // ...

    public GameObject Instantiate(string path, Transform parent = null)
    {
        // Prefabs를 메모리에 올린다.
        GameObject original = Load<GameObject>($"Prefabs/{path}");
        if (original == null)
        {
            Debug.Log($"Failed to load prefab : {path}");
            return null;
        }

        // Instantiate는 메모리에 올라온 Prefabs의 복사본을 만들어 생성해 주는 과정이다.
        GameObject go = Object.Instantiate(original, parent);
        go.name = original.name;
        return go;
    }

    // ...
```

* `Load<GameObject>($"Prefabs/{path}");` - 메모리 올리는 부분
* `Object.Instantiate(original, parent);` - 복사본을 만드는 부분
* 두 부분의 Object Pooling이 필요하다

```csharp
public class ResourceManager
{

    public T Load<T>(string path) where T : Object
    {
        if (typeof(T) == typeof(GameObject))
        {
            string name = path;
            int index = name.LastIndexOf('/');
            if (index >= 0)
                name = name.Substring(index + 1);

            // 이렇게 사용
            GameObject go = Managers.Pool.GetOriginal(name);
            if (go != null)
                return go as T;
        }

        return Resources.Load<T>(path);
    }

    public GameObject Instantiate(string path, Transform parent = null)
    {
        GameObject original = Load<GameObject>($"Prefabs/{path}");
        if (original == null)
        {
            Debug.Log($"Failed to load prefab : {path}");
            return null;
        }

        // 이렇게 사용
        if (original.GetComponent<Poolable>() != null)
            return Managers.Pool.Pop(original, parent).gameObject;


    // ...

    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        // Destroy시에도 Object Pool에 다시 돌려준다.
        Poolable poolable = go.GetComponent<Poolable>();
        if (poolable != null)
        {
            Managers.Pool.Push(poolable);
            return;
        }

        Object.Destroy(go);
    }

```

---

* 구현본을 보자.

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PoolManager
{
    #region Pool
    class Pool
    {
        public GameObject Origin { get; private set; }
        public Transform Root { get; set; }


        /*
            public class Poolable : MonoBehaviour
            {
                // 현재 pooling됐는지만 체크
                public bool IsUsing;
            }
        */
        Stack<Poolable> _poolStack = new Stack<Poolable>();

        public void Init(GameObject original, int count = 5)
        {
            Origin = original;
            Root = new GameObject().transform;
            Root.name = $"{original.name}_Root";

            for(int i = 0; i < count; i++)
            {
                Push(Create());
            }
        }

        Poolable Create()
        {
            GameObject go = Object.Instantiate<GameObject>(Origin);
            go.name = Origin.name;
            return go.GetOrAddComponent<Poolable>();
        }

        public void Push(Poolable poolable)
        {
            if (poolable == null)
                return;

            poolable.transform.parent = Root;
            poolable.gameObject.SetActive(false);   // 생성하자 마자 바로 키진 않음
            poolable.isUsing = false;

            _poolStack.Push(poolable);
        }

        public Poolable Pop(Transform parent)
        {
            Poolable poolable;

            if (_poolStack.Count > 0)
                poolable = _poolStack.Pop();
            else
                poolable = Create();

            poolable.gameObject.SetActive(true);    // 사용시에 킨다.
            poolable.transform.parent = parent;
            poolable.isUsing = true;
            return poolable;
        }
    }
    #endregion


    // --------------------Pool End---------------------


    Dictionary<string, Pool> _pool = new Dictionary<string, Pool>();
    Transform _root;
    public void Init()
    {
        _root = new GameObject { name = "@Pool_Root" }.transform;
        Object.DontDestroyOnLoad(_root);
    }

    public void CreatePool(GameObject original, int count = 5)
    {
        Pool pool = new Pool();
        pool.Init(original, count);
        pool.Root.parent = _root;

        _pool.Add(original.name, pool);
    }

    public void Push(Poolable poolable)
    {
        string name = poolable.gameObject.name;
        if (_pool.ContainsKey(name) == false)
        {
            GameObject.Destroy(poolable.gameObject);
            return;
        }

        _pool[name].Push(poolable);
    }

    public Poolable Pop(GameObject original, Transform parent = null)
    {
        if (_pool.ContainsKey(original.name) == false)
            CreatePool(original);

        return _pool[original.name].Pop(parent);
    }

    public GameObject GetOriginal(string name)
    {
        if (_pool.ContainsKey(name) == false)
            return null;

        return _pool[name].Origin;
    }

    public void Clear()
    {
        foreach (Transform child in _root)
            GameObject.Destroy(child.gameObject);

        _pool.Clear();
    }
}
```
