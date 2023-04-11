---
layout: default
title: "05. Data & Config"
parent: "(컨텐츠 제작 1 - 서버 연결)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 📥](https://github.com/Arthur880708/CSharp_GameServer_Connect_To_Server/tree/17)

## Skill Data

```json
{
    "skills": [
        {
            "id": "1",
            "name": "평타",
            "cooldown": "0.2",
            "damage": "10",
            "skillType": "SkillAuto"
        },
        {
            "id": "2",
            "name": "화살 공격",
            "cooldown": "0.2",
            "damage": "5",
            "skillType": "SkillProjectile",
            "projectile": {
                "name": "화살",
                "speed": "20.0",
                "range": "10",
                "prefab": "Creature/Arrow"
            }
        }
    ]
} 
```

```csharp
public interface ILoader<Key, Value>
{
    Dictionary<Key, Value> MakeDict();
}

public class DataManager
{
    public static Dictionary<int, Data.Stat> StatDict { get; private set; } = new Dictionary<int, Data.Stat>();
    public static Dictionary<int, Data.Skill> SkillDict { get; private set; } = new Dictionary<int, Data.Skill>();

    public static void LoadData()
    {
        StatDict = LoadJson<Data.StatData, int, Data.Stat>("StatData").MakeDict();
        SkillDict = LoadJson<Data.SkillData, int, Data.Skill>("SkillData").MakeDict();
    }

    static Loader LoadJson<Loader, Key, Value>(string path) where Loader : ILoader<Key, Value>
    {
        string text = File.ReadAllText($"{ConfigManager.Config.dataPath}/{path}.json");
        return Newtonsoft.Json.JsonConvert.DeserializeObject<Loader>(text);
    }
}
```

```csharp
[Serializable]
public class SkillData : ILoader<int, Skill>
{
    public List<Skill> skills = new List<Skill>();

    public Dictionary<int, Skill> MakeDict()
    {
        Dictionary<int, Skill> dict = new Dictionary<int, Skill>();
        foreach (Skill skill in skills)
            dict.Add(skill.id, skill);
        return dict;
    }
}
```

---

## Config Data

```csharp
[Serializable]
public class ServerConfig
{
    public string dataPath;
}

public class ConfigManager
{
    public static ServerConfig Config { get; private set; }

    public static void LoadConfig()
    {
        string text = File.ReadAllText("config.json");
        Config = Newtonsoft.Json.JsonConvert.DeserializeObject<ServerConfig>(text);
    }
}
```