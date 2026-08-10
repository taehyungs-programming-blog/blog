---
layout: default
title: "17. Resource"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/Arthur880708/DirextX12-Example/tree/14)
* 우선 본 강좌에서는 Cube와 Sphere를 생성할 예정이고 Cube, Sphere를 위해서 Resource를 어떻게 관리할지? 가 사실 핵심이다.

## object 클래스

* 게임에 사용되는 각종 object를 object 클래스를 생성하여 관리해 보자.
* 목적은 파일로 저장/로드가 가능하게 하여 좀 더 편안하게 관리 + object라는 상위 클래스로 관리하면 미래에 관리가 수월하기 때문

```cpp
#pragma once

enum class OBJECT_TYPE : uint8
{
    NONE,
    GAMEOBJECT, // PREFAB
    COMPONENT,
    MATERIAL,
    MESH,
    SHADER,
    TEXTURE,
    END
};

enum
{
    OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
    Object(OBJECT_TYPE type);
    virtual ~Object();          // 최상위 클래스이기에 virtual로 소멸자를 갖게하자

    OBJECT_TYPE GetType() { return _objectType; }

    // 이름선언
    void SetName(const wstring& name) { _name = name; }
    const wstring& GetName() { return _name; }

    // TODO : Instantiate(Object 복사기능, 이후 추가 할 예정)

protected:
    // Resources에서는 Object의 Load, Save를 마음대로 할 수 있다.
    friend class Resources;

    // 파일에 저장/로드기능
    virtual void Load(const wstring& path) { }
    virtual void Save(const wstring& path) { }

protected:
    OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
    wstring _name;
};
```

* 참고로 `object`로 파생되는 클래스는 `GameObject`, `Component`, `Material`, `Shader`, `Texture`
* 다른건 이해가 되는데 `GameObject`를 하나의 Object로 봐야하나? `GameObject`내에는 Textrue, Shader등이 포함될텐데?? 
    * -> 여러 리소스를 나름대로 커스터마이징한 하나의 Object라 생각하자(언리얼의 블루프린트와 동일한 개념!)

---

## Resource 클래스

* object 자체를 관리할 Resource라는 클래스를 생성한다.

```cpp
class Resources
{
    DECLARE_SINGLE(Resources);

public:
    // object가 어떤 클래스일지 모르니 Template로 생성
    template<typename T>
    shared_ptr<T> Load(const wstring& key, const wstring& path);

    template<typename T>
    bool Add(const wstring& key, shared_ptr<T> object);

    template<typename T>
    shared_ptr<T> Get(const wstring& Key);

    template<typename T>
    OBJECT_TYPE GetObjectType();

    // Cube, Sphere 구현은 아래서 참조
    shared_ptr<Mesh> LoadCubeMesh();
    shared_ptr<Mesh> LoadSphereMesh();

private:
    // Resource의 보관은 Map의 형태로 한다.
    using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;

    // 각 object에 따라 배열을 새로 할당하여 관리
    array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;
    /* 사용은 이렇게 -> _resources[static_cast<uint8>(objectType)] */
};
```

```cpp
// Resources.h

template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return static_pointer_cast<T>(findIt->second);

    shared_ptr<T> object = make_shared<T>();
    object->Load(path);
    keyObjMap[key] = object;

    return object;
}

template<typename T>
bool Resources::Add(const wstring& key, shared_ptr<T> object)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return false;

    keyObjMap[key] = object;
    return true;
}

template<typename T>
shared_ptr<T> Resources::Get(const wstring& key)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return static_pointer_cast<T>(findIt->second);

    return nullptr;
}

template<typename T>
inline OBJECT_TYPE Resources::GetObjectType()
{
    // std::is_same_v<T, GameObject> -> C++11 Template기법
    if (std::is_same_v<T, GameObject>)
        return OBJECT_TYPE::GAMEOBJECT;
    else if (std::is_same_v<T, Material>)
        return OBJECT_TYPE::MATERIAL;
    else if (std::is_same_v<T, Mesh>)
        return OBJECT_TYPE::MESH;
    else if (std::is_same_v<T, Shader>)
        return OBJECT_TYPE::SHADER;
    else if (std::is_same_v<T, Texture>)
        return OBJECT_TYPE::TEXTURE;
    else if (std::is_convertible_v<T, Component>)
        return OBJECT_TYPE::COMPONENT;
    else
        return OBJECT_TYPE::NONE;
}
```

---

## 정점으로 Cube, Sphere 만드는 방법

* Cube는 쉽다

```
               ------------------
              /                 / |
             /                 /  |
            /                 /   |
(-w, h, -d) ------------------- (w, h, -d)
            |                 |   |
            |                 |   |
            |                 |  /
            |                 | /
(-w, -h, -d) ------------------ (w, -h, -d)
```

* 원점을 중심으로 높이를 h, 너비를 w, 깊이를 d라 했을때 정점정보는 위 그림과 같이 표현할 수 있다.
* 문제는 Sphere인데 ... 대략 아래와 같은 방법으로 만든다고 생각하고 차라리 코드를 보는게 쉽다

```
     o (꼭지점) -> 북극
    / \
   o   o
  / \ / \

    ...
  
  \ / \ /
   o   o
    \ /
     o -> 남극

* 북극, 남극을 만들고 하나하나 늘려가며 가운데서 만나는 형태로 구현
```

* 코드에 들어가기 전 `Vertex` 구조체를 약간 수정하겠음

```cpp
struct Vertex
{
    Vertex() {}
    Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
        : pos(p), uv(u), normal(n), tangent(t)
    {
    }
    Vec3 pos;
    Vec2 uv;

    // Vec3 color -> 삭제하고,

    // Lighting 에 사용될 정보이기에 지금은 무시해도 좋다
    Vec3 normal;
    Vec3 tangent;
};
```

* `Vertex` 구조체가 수정되면 쉐이더도 동일하게 수정이 필요해진다.

```cpp
void Shader::Init(const wstring& path)
{
    // ...
    D3D12_INPUT_ELEMENT_DESC desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    // ...
```

```
struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;

    // normal, tangent는 당장사용하지 않기에 넣지는 않음
};
```

### Cube SourceCode

```cpp
shared_ptr<Mesh> Resources::LoadCubeMesh()
{
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Cube");
    if (findMesh)
        return findMesh;

    float w2 = 0.5f;
    float h2 = 0.5f;
    float d2 = 0.5f;
    vector<Vertex> vec(24);

    // 앞면
    vec[0] = Vertex(Vec3(-w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[1] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[2] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[3] = Vertex(Vec3(+w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(1.0f, 0.0f, 0.0f));

    // 뒷면
    vec[4] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[5] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[6] = Vertex(Vec3(+w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[7] = Vertex(Vec3(-w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // 윗면
    vec[8] = Vertex(Vec3(-w2, +h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[9] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[10] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
    vec[11] = Vertex(Vec3(+w2, +h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));

    // 아랫면
    vec[12] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[13] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[14] = Vertex(Vec3(+w2, -h2, +d2), Vec2(0.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));
    vec[15] = Vertex(Vec3(-w2, -h2, +d2), Vec2(1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f));

    // 왼쪽면
    vec[16] = Vertex(Vec3(-w2, -h2, +d2), Vec2(0.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[17] = Vertex(Vec3(-w2, +h2, +d2), Vec2(0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[18] = Vertex(Vec3(-w2, +h2, -d2), Vec2(1.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));
    vec[19] = Vertex(Vec3(-w2, -h2, -d2), Vec2(1.0f, 1.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f));

    // 오른쪽면
    vec[20] = Vertex(Vec3(+w2, -h2, -d2), Vec2(0.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[21] = Vertex(Vec3(+w2, +h2, -d2), Vec2(0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[22] = Vertex(Vec3(+w2, +h2, +d2), Vec2(1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
    vec[23] = Vertex(Vec3(+w2, -h2, +d2), Vec2(1.0f, 1.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

    vector<uint32> idx(36);
    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // 뒷면
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // 윗면
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // 아랫면
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // 왼쪽면
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // 오른쪽면
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;
    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);
    Add(L"Cube", mesh);
    return mesh;
}
```

### Sphere SourceCode

```cpp
shared_ptr<Mesh> Resources::LoadSphereMesh()
{
    shared_ptr<Mesh> findMesh = Get<Mesh>(L"Sphere");
    if (findMesh)
        return findMesh;
    float radius = 0.5f; // 구의 반지름
    uint32 stackCount = 20; // 가로 분할
    uint32 sliceCount = 20; // 세로 분할
    vector<Vertex> vec;
    Vertex v;

    // 북극
    v.pos = Vec3(0.0f, radius, 0.0f);
    v.uv = Vec2(0.5f, 0.0f);
    v.normal = v.pos;

    // normal, tangent는 우선 무시
    v.normal.Normalize();
    v.tangent = Vec3(1.0f, 0.0f, 1.0f);
    vec.push_back(v);
    float stackAngle = XM_PI / stackCount;
    float sliceAngle = XM_2PI / sliceCount;
    float deltaU = 1.f / static_cast<float>(sliceCount);
    float deltaV = 1.f / static_cast<float>(stackCount);

    // 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
    for (uint32 y = 1; y <= stackCount - 1; ++y)
    {
        float phi = y * stackAngle;
        // 고리에 위치한 정점
        for (uint32 x = 0; x <= sliceCount; ++x)
        {
            float theta = x * sliceAngle;
            v.pos.x = radius * sinf(phi) * cosf(theta);
            v.pos.y = radius * cosf(phi);
            v.pos.z = radius * sinf(phi) * sinf(theta);
            v.uv = Vec2(deltaU * x, deltaV * y);
            // normal, tangent는 우선 무시
            v.normal = v.pos;
            v.normal.Normalize();
            v.tangent.x = -radius * sinf(phi) * sinf(theta);
            v.tangent.y = 0.0f;
            v.tangent.z = radius * sinf(phi) * cosf(theta);
            v.tangent.Normalize();
            vec.push_back(v);
        }
    }

    // 남극
    v.pos = Vec3(0.0f, -radius, 0.0f);
    v.uv = Vec2(0.5f, 1.0f);
    v.normal = v.pos;
    v.normal.Normalize();
    v.tangent = Vec3(1.0f, 0.0f, 0.0f);
    vec.push_back(v);
    vector<uint32> idx(36);

    // 북극 인덱스
    for (uint32 i = 0; i <= sliceCount; ++i)
    {
        //  [0]
        //   |  \
        //  [i+1]-[i+2]
        idx.push_back(0);
        idx.push_back(i + 2);
        idx.push_back(i + 1);
    }

    // 몸통 인덱스
    uint32 ringVertexCount = sliceCount + 1;
    for (uint32 y = 0; y < stackCount - 2; ++y)
    {
        for (uint32 x = 0; x < sliceCount; ++x)
        {
            //  [y, x]-[y, x+1]
            //  |       /
            //  [y+1, x]
            idx.push_back(1 + (y) * ringVertexCount + (x));
            idx.push_back(1 + (y) * ringVertexCount + (x + 1));
            idx.push_back(1 + (y + 1) * ringVertexCount + (x));
            //       [y, x+1]
            //       /    |
            //  [y+1, x]-[y+1, x+1]
            idx.push_back(1 + (y + 1) * ringVertexCount + (x));
            idx.push_back(1 + (y) * ringVertexCount + (x + 1));
            idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
        }
    }

    // 남극 인덱스
    uint32 bottomIndex = static_cast<uint32>(vec.size()) - 1;
    uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        //  [last+i]-[last+i+1]
        //  |      /
        //  [bottom]
        idx.push_back(bottomIndex);
        idx.push_back(lastRingStartIndex + i);
        idx.push_back(lastRingStartIndex + i + 1);
    }

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);
    Add(L"Sphere", mesh);
    return mesh;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx/directx-16-1.png"/>
</p>