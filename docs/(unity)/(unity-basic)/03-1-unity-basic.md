---
layout: default
title: "3. Transform"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Tips) 

* Unity Code를 생성시 아래와 같은 주석이 항상붙는다
* 크게 상관은 없지만 제거해 보자.

```csharp
public class HelloUnity : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Hello Unity");
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
```

* `D:\Program Files\Unity\Hub\Editor\2019.4.40f1\Editor\Data\Resources\ScriptTemplates`
* `81-C# Script-NewBehaviourScript.cs.txt`을 열어 주석을 제거한후 저장하자

---

## Asset 다운로드

* Window -> Asset Store (버그가 있어서 chrome으로 접속을 추천)
* Unity-Chan! 다운, Import

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-3-1.png"/>
</p>

* Art, Audio만 받아도 좋다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-3-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-3-3.png"/>
</p>

---

## Unity Chan을 움직여 보자.

```csharp
public class PlayerController : MonoBehaviour
{
    [SerializeField]       // private으로 변수를 지정하나 Unity에서는 수정이 가능.
    float _speed = 10.0f;

    void Start()
    {
        
    }

    void Update()
    {
        /*
        if (Input.GetKey(KeyCode.W))
            transform.position += new Vector3(0.0f, 0.0f, 1.0f) * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.S))
            transform.position -= new Vector3(0.0f, 0.0f, 1.0f) * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.A))
            transform.position += new Vector3(1.0f, 0.0f, 0.0f) * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.D))
            transform.position -= new Vector3(1.0f, 0.0f, 0.0f) * Time.deltaTime * _speed;
        */

        /*
        // 코드 가독성 개선
        if (Input.GetKey(KeyCode.W))
            transform.position += Vector3.forward * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.S))
            transform.position += Vector3.back * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.A))
            transform.position += Vector3.left * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.D))
            transform.position += Vector3.right * Time.deltaTime * _speed;
        */

        /*
        // 캐릭터가 바라보는 방향을 기준으로 수정
        if (Input.GetKey(KeyCode.W))
            transform.position += transform.TransformDirection(Vector3.forward * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.S))
            transform.position += transform.TransformDirection(Vector3.back * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.A))
            transform.position += transform.TransformDirection(Vector3.left * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.D))
            transform.position += transform.TransformDirection(Vector3.right * Time.deltaTime * _speed);
        */

        // 코드 간결화
        if (Input.GetKey(KeyCode.W))
            transform.Translate(Vector3.forward * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.S))
            transform.Translate(Vector3.back * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.A))
            transform.Translate(Vector3.left * Time.deltaTime * _speed);
        if (Input.GetKey(KeyCode.D))
            transform.Translate(Vector3.right * Time.deltaTime * _speed);
    }
}
```

* `x` 를 누르면 캐릭터기준 바라보는 방향으로 좌표계를 보여준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-3-4.png"/>
</p>

---

## Unity Chan을 회전시켜 보자.

```csharp
public class PlayerController : MonoBehaviour
{
    [SerializeField]       // private으로 변수를 지정하나 Unity에서는 수정이 가능.
    float _speed = 10.0f;
    float _yAngle = 0.0f;

    void Start()
    {
        
    }

    void Update()
    {
        /*
        // 절대 회전각도 지정
        _yAngle += Time.deltaTime * 100;
        transform.eulerAngles = new Vector3(0.0f, _yAngle, 0.0f);
        */

        /*
        // 축을 기준으로 얼만큼 회전을 하겠다.
        _yAngle += Time.deltaTime * 100;
        transform.Rotate(new Vector3(0.0f, _yAngle, 0.0f));
        */

        // Quaternion으로 처리가능.
        _yAngle += Time.deltaTime * 100;
        transform.rotation = Quaternion.Euler(new Vector3(0.0f, _yAngle, 0.0f));

        if (Input.GetKey(KeyCode.W))
            transform.rotation = Quaternion.LookRotation(Vector3.forward);  // World 좌표계 기준 Forward임
        if (Input.GetKey(KeyCode.S))
            transform.rotation = Quaternion.LookRotation(Vector3.back);
        if (Input.GetKey(KeyCode.A))
            transform.rotation = Quaternion.LookRotation(Vector3.left);
        if (Input.GetKey(KeyCode.D))
            transform.rotation = Quaternion.LookRotation(Vector3.right);
    }
```

좀 더 부드럽게 회전하게 해보자.

```csharp
if (Input.GetKey(KeyCode.W))
    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.forward), 0.2f);
    // 20%만큼 Quaternion.LookRotation(Vector3.forward)로 회전해 달라
    transform.position += Vector3.forward * Time.deltaTime * _speed;
if (Input.GetKey(KeyCode.S))
    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.back), 0.2f);
    transform.position += Vector3.back * Time.deltaTime * _speed;
if (Input.GetKey(KeyCode.A))
    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.left), 0.2f);
    transform.position += Vector3.left * Time.deltaTime * _speed;
if (Input.GetKey(KeyCode.D))
    transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.right), 0.2f);
    transform.position += Vector3.right * Time.deltaTime * _speed;
```

---

## Input Manager

* 키보드 입력을 받는 부분을 하나로 모으자.
* 사실 중요한 부분은 왜 이렇게 구현할까? 인데
    * Update() 함수에 직접적으로 Input 관련 처리를 넣고 싶지 않음
    * 공용으로 관리되는 매니저들은 Managers를 통해 접근하게 하고 싶음.

```csharp
public class InputManager
{
    public Action KeyAction = null;
    public void OnUpdate()
    {
        if (Input.anyKey == false)
            return;

        if (KeyAction != null)
            KeyAction.Invoke();
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
    void Start()
    {
        
    }

    void Update()
    {
        _input.OnUpdate();
    }

    // ...
```

```csharp
public class Players : MonoBehaviour
{
    float _speed = 10.0f;

    void Start()
    {
        Managers.Input.KeyAction -= OnKeyboard; // 혹시나 다른곳에서 += OnKeyboard을 했을까봐 미리 빼줌.

        // OnUpdate이 호출시 OnKeyboard도 호출되게 Action처리
        Managers.Input.KeyAction += OnKeyboard;
    }

    // ...

    void OnKeyboard()
    {
        if (Input.GetKey(KeyCode.W))
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.forward), 0.2f);
        transform.position += Vector3.forward * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.S))
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.back), 0.2f);
        transform.position += Vector3.back * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.A))
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.left), 0.2f);
        transform.position += Vector3.left * Time.deltaTime * _speed;
        if (Input.GetKey(KeyCode.D))
            transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(Vector3.right), 0.2f);
        transform.position += Vector3.right * Time.deltaTime * _speed;
    }
```