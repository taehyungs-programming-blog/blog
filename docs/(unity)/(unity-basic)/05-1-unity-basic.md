---
layout: default
title: "5. Collision, RayCast"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Collision 기본동작 확인

* Collision을 확인하기 위해서 Plane을 하나 추가하자
    * Hierarchy 우클릭 -> 3D Object -> Plane
* Plane에 색상이 없어 구분이 어려우니 Material을 하나 추가해 색상까지 넣는다.
    * Project -> Assets -> 우클릭 -> Create -> Material
    * Albedo에서 색을 선택하면 된다.
* Plane에 생성한 Material을 드래그 앤 드롭해 넣는다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-1.png"/>
</p>

* Unity Chan을 추가하고, Rigidbody와 Capsule Collider Component를 추가한다.
    * Rigidbody - 물리엔지을 적용해 달라
    * Collider Component - 충돌을 감지하게 해달라

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-2.png"/>
</p>

* 참고로 Edit Collider를 클릭해 Collsion 범위를 설정할수 있다.
* 이제 Play를 눌러 어떻게 동작하는지 확인한다.
    * 참고로 말하자면 Plane에 Mesh Collider가 없다면 그냥 Plane을 뚫고 지나가 버린다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-3.png"/>
</p>

---

## 물체와 충돌이 일어나면 어떻게 되나 확인해 보자.

* Cube를 하나 놓고 Unity Chan과 충돌 시켜본다.
    * (TIPS) 메인카메라 누른상태에서 Control + Shift + f - 메인카메라 위치변경

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-4.png"/>
</p>

* 엄청 과도하게 반응함을 볼 수 있는데, Unity의 물리엔진의 영향이다.(is Kinematic)
* 영향을 적게하려면 Unity-Chan의 Rigidbody -> Constrains -> Freeze Rotation X, Y, Z를 모두 키면 되는데 반응이 이상하긴 매한가지이다.
* 이후에 정확히 어떻게 처리하는지 알려준다. 일단 이런 충돌이 가능하다고만 알고 있자.

---

## 충돌여부를 코드에서 확인하는 방법?

```csharp
public class TestCollision : MonoBehaviour
{
    /*
        OnCollisionEnter 조건
            * 나한테 RigidBody 있음 (단, IsKinematic Off)
            * 나한테 Collider 있음 (단, IsTrigger Off)
            * 상대에게 Collider 있음 (단, IsTrigger Off)
    */
    private void OnCollisionEnter(Collision collision)  // collision : 나랑 Collsion된 object의 정보가 들어간다.
    {
        Debug.Log($"Collision @ {collision.gameObject.name}");
    }

    /*
        OnTriggerEnter 조건
            * 둘 다 Collider 있음
            * 둘 중 하나 IsTrigger On
            * 둘 중 하나 RigidBody 있음
    */
    private void OnTriggerEnter(Collider other)
    {
        Debug.Log("Trigger");
    }
```

---

## Raycasting

* 인게임 화면에서 마우스 클릭이벤트를 어떻게 처리해야할까?

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        Debug.DrawRay(transform.position, Vector3.forward * 100, Color.red);
        
        if (Physics.Raycast(transform.position, Vector3.forward, 10/* 거리 *//))
        {
            Debug.Log("Ray Casted")
        }
    }

    // ...
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-5.png"/>
</p>

* 아직 뭔지는 잘 모르겠으나 빨간 선이 생겼음을 알 수 있다.

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        // player가 바라보는 좌표를 world좌표계로 전환
        Vector3 look = transform.TransformDirection(Vector3.forward);
        Debug.DrawRay(transform.position, look, Color.red);
        
        RaycastHit hit;
        if (Physics.Raycast(transform.position, Vector3.forward, out hit ,10))
        {
            Debug.Log($"RayCast {hit.collider.gameObject.name}")
        }

        // Raycast에 맞는 오브젝트가 여러개 일시
        RaycastHit[] hit;
        hit = Physics.RaycastAll(transform.position, Vector3.forward, out hit ,10)
        foreach(RaycastHit h in hit)
        {
            Debug.Log($"RayCast {h.collider.gameObject.name}")
        }
    }

    // ...
```

---

### Screen, Viewport?

* 인게임화면의 마우스좌표를 받아와 보자.

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        // Screen 좌표(해상도)
        Debug.Log(Input.mousePosition);

        // Viewport 좌표(-1 ~ 1)
        Debug.Log(Camera.main.ScreenToViewportPoint(Input.mousePosition));
    }

    // ...
```

---

### 다시 RayCast

* 인게임화면에서 클릭시 처리를 어떻게 하는지 다시 얘기해 보자.

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        if(InputGetMouseButtonDown(0))  // 마우스 클릭시에
        {
            Vector3 mouseWorldPos = Camera.main.ScreenToWorldPoint(new Vector3(Input.mousePosition.x, Input.mousePosition.y, Camera.main.nearClipPlane));

            Vector3 dir = mouseWorldPos - Camera.main.transform.position;
            dir = dir.normalized;

            // 1초동안 Ray를 그려준다
            Debug.DrawRay(Camera.main.transform.position, dir * 100, Color.red, 1.0f);

            RaycastHit hit;
            if (Physics.Raycast(Camera.main.transform.position, dir, out hit, 100.0f))
            {
                Debug.Log($"RayCast {hit.collider.gameObject.name}")
            }
        }

    // ...
```

* ??? 그런데 인게임에서 클릭 이벤트 처리는 많이 사용될꺼 같은데 저 코드를 매번 친다고??
* 사실 Unity에 준비가 되어있다.

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        if(InputGetMouseButtonDown(0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            Debug.DrawRay(Camera.main.transform.position, ray.direction * 100.0f, Color.red, 1.0f);

            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, 100.0f))
            {
                Debug.Log($"RayCast {hit.collider.gameObject.name}")
            }

            // ...
```

---

## LayerMast

* Raycast 작업은 연산량이 많기에 모든 object에 Raycast를 적용시 성능이 나빠진다.
* 딱 필요한 object에 한해서 Raycast를 하고자 하는데 **Layer**를 통해 설정하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-6.png"/>
</p>

* Layer -> Add Layer ... 선택

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-5-7.png"/>
</p>

* 0 ~ 7은 Unity가 사용중이고 8부터 개발자가 선언할 수 있다.

```csharp
public class TestCollision : MonoBehaviour
{
    // ...

    void Update()
    {
        if(InputGetMouseButtonDown(0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            Debug.DrawRay(Camera.main.transform.position, ray.direction * 100.0f, Color.red, 1.0f);

            // Layer 순번은 bit flag로 선택이 가능(아래는 8번 layer를 선택)
            int mask = (1 << 8);
            // int mask = (1 << 8) | (1 << 9);

            // 혹은 mask정보를 이름으로 받을수 있다.
                // 뭘 쓰든 상관없음.
            LayerMask mask = LayerMask.GetMask("Monster");

            RaycastHit hit;
            if (Physics.Raycast(ray, out hit, 100.0f, mask))
            {
                Debug.Log($"RayCast {hit.collider.gameObject.name}")

                // 테그정보로도 출력가능
                Debug.Log($"RayCast print Tag {hit.collider.gameObject.tag}")
            }

            // ...
```