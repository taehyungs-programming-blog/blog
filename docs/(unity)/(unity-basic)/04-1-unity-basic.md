---
layout: default
title: "4. Prefab"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Prefab

* Scene에서 object를 새로 만들어 Project에 넣어보자.
    * Tank 안에 Cube를 넣는다
* 이렇게 저장된 object를 prefab이라 한다.
    * 여기서 유의할 점은 Cube의 좌표계는 Tank좌표계를 따른다는 것이다.
    * 혹시 Tank를 회전했는데 Cube의 각이 변하지 않아 당황해 하지 말자.
* ??? 그냥 object아니냐?
    * 정확히 말하면 이 object를 저장할수 있는데 이 저장된 object가 prefab이 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-4-1.png"/>
</p>

* prefab을 더블클릭하면 편집페이지로 이동한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-4-2.png"/>
</p>

* Prefab내에 Prefab을 추가할수 있음(Nested Prefab)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-4-3.png"/>
</p>

---

## Resource Manager

* 하고자하는 것! - 코드로 Prefab을 만들어 보자.

```csharp
public class PrefabTestScripts : MonoBehaviour
{
    public GameObject prefab;

    void Start()
    {
        Object.Instantiate(prefab);
    }
}
```

* Unity에서 내가 원하는 Prefab추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-4-4.png"/>
</p>

```csharp
public class PrefabTestScripts : MonoBehaviour
{
    public GameObject prefab;
    void Start()
    {
        var tank = Object.Instantiate(prefab);

        Destroy(tank, 3.0f/*3초 후 삭제*/);
    }
}
```

* Prefab을 public으로 사용하기 싫다면??

```csharp
public class PrefabTestScripts : MonoBehaviour
{
    GameObject prefab;
    
    void Start()
    {
        prefab = Resources.Load<GameOBject>("Prefabs/Tank");    // 단, 경로는 Assests/Resources 아래 있어야함.
        var tank = Object.Instantiate(prefab);

        Destroy(tank, 3.0f/*3초 후 삭제*/);
    }
}
```

* 이런 Prefab을 ResourceManager를 통해서 해보자.

```csharp
public class ResourceManager
{
    public T Load<T>(string path) where T : Object
    {
        return Resources.Load<T>(path);
    }

    public GameObject Instantiate(string path, Transform parent = null)
    {
        GameObject prefab = Load<GameObject>($"Prefabs/{path}");

        if(prefab == null)
        {
            Debug.Log($"Failed to load prefab : {path}");
            return null;
        }

        return Object.Instantiate(prefab, parent);
    }

    public void Destroy(GameObject go)
    {
        if (go == null)
            return;

        Object.Destroy(go);
    }
}
```

```csharp
public class Managers : MonoBehaviour
{
    static Managers Instance;
    public static Managers GetInstance() { Init(); return Instance; }

    InputManager _input = new InputManager();
    public static InputManager Input { get { return Instance._input; } }

    // 전역에서 관리하게 변경
    ResourceManager _resource = new ResourceManager();
    public static ResourceManager Resource { get { return Instance._resource; } }

    // ...
```

```csharp
var tank = Managers.Resource.Instantiate("Tank")
Destroy(tank);
```