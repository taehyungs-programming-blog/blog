---
layout: default
title: "13. MapEditor"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/4)

---

* 두 개의 함수를 지원한다

## GenerateMap

```csharp
[MenuItem("Tools/GenerateMap %#m")]
private static void GenerateMap()
{
    GameObject[] gameObjects = Selection.gameObjects;
    // Selection.gameObjects - Unity 에디터에서 개발자가 현재 선택한 모든 게임 오브젝트(game objects)의 배열을 반환하는 에디터 클래스의 속성입니다. 

    foreach (GameObject go in gameObjects)
    {
        Tilemap tm = Util.FindChild<Tilemap>(go, "Tilemap_Collision", true);
        // Q) Tilemap_Collision는 어디서 만드나? -> 아래 참조

        using (var writer = File.CreateText($"Assets/@Resources/Data/MapData/{go.name}Collision.txt"))
        {
            writer.WriteLine(tm.cellBounds.xMin);
            writer.WriteLine(tm.cellBounds.xMax);
            writer.WriteLine(tm.cellBounds.yMin);
            writer.WriteLine(tm.cellBounds.yMax);

            for (int y = tm.cellBounds.yMax; y >= tm.cellBounds.yMin; y--)
            {
                for (int x = tm.cellBounds.xMin; x <= tm.cellBounds.xMax; x++)
                {
                    TileBase tile = tm.GetTile(new Vector3Int(x, y, 0));
                    if (tile != null)
                    {
                        if (tile.name.Contains("O"))
                            writer.Write(Define.MAP_TOOL_NONE);
                        else
                            writer.Write(Define.MAP_TOOL_SEMI_WALL);
                    }
                    else
                        writer.Write(Define.MAP_TOOL_WALL);
                }
                writer.WriteLine();
            }
        }
    }

    Debug.Log("Map Collision Generation Complete");
}
```

* `Tilemap_Collision`의 생성 시점

```csharp
public class GameScene : BaseScene
{
	public override bool Init()
	{
		if (base.Init() == false)
			return false;

		SceneType = EScene.GameScene;

        // GameScene의 Init시점에 BaseMap 로
		Managers.Map.LoadMap("BaseMap");

        // ...
```

```csharp
public void LoadMap(string mapName)
{
    DestroyMap();

    GameObject map = Managers.Resource.Instantiate(mapName);
    map.transform.position = Vector3.zero;
    map.name = $"@Map_{mapName}";

    Map = map;
    MapName = mapName;
    CellGrid = map.GetComponent<Grid>();

    ParseCollisionData(map, mapName);

    // ...
```

```csharp
void ParseCollisionData(GameObject map, string mapName, string tilemap = "Tilemap_Collision")
{
    GameObject collision = Util.FindChild(map, tilemap, true);
    if (collision != null)
        collision.SetActive(false);

    // Collision 관련 파일
    TextAsset txt = Managers.Resource.Load<TextAsset>($"{mapName}Collision");
    StringReader reader = new StringReader(txt.text);

    MinX = int.Parse(reader.ReadLine());
    MaxX = int.Parse(reader.ReadLine());
    MinY = int.Parse(reader.ReadLine());
    MaxY = int.Parse(reader.ReadLine());

    int xCount = MaxX - MinX + 1;
    int yCount = MaxY - MinY + 1;
    _collision = new ECellCollisionType[xCount, yCount];

    for (int y = 0; y < yCount; y++)
    {
        string line = reader.ReadLine();
        for (int x = 0; x < xCount; x++)

    // ...
```

---

## CreateObjectTile

* 아래처럼 csv를 읽어 data로 만든다

```csharp
[MenuItem("Tools/Create Object Tile Asset %#o")]
public static void CreateObjectTile()
{
    // Monster
    Dictionary<int, Data.MonsterData> MonsterDic = LoadJson<Data.MonsterDataLoader, int, Data.MonsterData>("MonsterData").MakeDict();
    foreach (var data in MonsterDic.Values)
    {
        CustomTile customTile = ScriptableObject.CreateInstance<CustomTile>();
        customTile.Name = data.DescriptionTextID;
        customTile.DataTemplateID = data.DataId;
        customTile.ObjectType = Define.EObjectType.Creature;
        customTile.CreatureType = Define.ECreatureType.Monster;

        string name = $"{data.DataId}_{data.DescriptionTextID}";
        string path = "Assets/@Resources/TileMaps/Tiles/Dev/Monster";
        path = Path.Combine(path, $"{name}.Asset");

        if (File.Exists(path))
            continue;

        AssetDatabase.CreateAsset(customTile, path);
    }

    // Env
    Dictionary<int, Data.EnvData> Env = LoadJson<Data.EnvDataLoader, int, Data.EnvData>("EnvData").MakeDict();
    foreach (var data in Env.Values)
    {
        // ...
```