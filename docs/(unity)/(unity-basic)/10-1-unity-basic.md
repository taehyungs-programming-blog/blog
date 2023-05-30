---
layout: default
title: "10. Sound Manager"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Sound 다뤄보기

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/10.1)

* 사실 이미 Main Camera에 Sound가 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-10-1-1.png"/>
</p>

* Sound에는 Source / Clip / Listner 이렇게 세 종류가 있는데.
    * **Source** - 음원 재생기
    * **Clip** - 소리의 음원
    * **Listner** - 음원을 듣는 녀석
* Main Camera에 Listner가 있는걸로 봐선 음원을 들을준비는 되어있다는 말이다.
* Cube중 하나에 Audio Source를 추가해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-10-1-2.png"/>
</p>

* C# SoundManager 생성, TestSound로 음원을 넣는 테스트를 해보자.
* Cube에 TestSound 를 붙이고 Enter되면 소리가 나게 해보자.

```csharp
public class TestSound : MonoBehaviour
{
    public AudioClip audioClip;

    private void OnTriggerEnter(Collider other)
    {
        AudioSource audio = GetComponent<AudioSource>();
        audio.PlayOneShot(audioClip);
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-10-1-3.png"/>
</p>

---

## Manager기능추가

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/10.2)

```csharp
public class SoundManager
{
    AudioSource[] _audioSources = new AudioSource[(int)Define.Sound.MaxCount];

    public void Init()
    {
        GameObject root = GameObject.Find("@Sound");
        if(root == null)
        {
            root = new GameObject { name = "@Sound" };
            Object.DontDestroyOnLoad(root);

            string[] soundNames = System.Enum.GetNames(typeof(Define.Sound));
            for(int i = 0; i < soundNames.Length - 1; i++)
            {
                GameObject go = new GameObject { name = soundNames[i] };
                _audioSources[i] = go.AddComponent<AudioSource>();
                go.transform.parent = root.transform;
            }
            _audioSources[(int)Define.Sound.Bgm].loop = true;
        }

    }

    public void Play(Define.Sound type, string path, float pitch = 1.0f)
    {
        if (path.Contains("Sounds/") == false)
            path = $"Sounds/{path}";

        if(type == Define.Sound.Bgm)
        {
            AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }
        }
        else 
        {
            AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }

            AudioSource audioSource = _audioSources[(int)Define.Sound.Effect];
            audioSource.pitch = pitch;
            audioSource.PlayOneShot(audioClip);
        }
    }
}
```

---

## 3D Sound

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/10.3)

* bgm을 처리해보자.

```csharp
public class SoundManager
{
    // ...

    public void Play(Define.Sound type = Define.Sound.Effect, string path = "", float pitch = 1.0f)
    {
        if (path.Contains("Sounds/") == false)
            path = $"Sounds/{path}";

        if(type == Define.Sound.Bgm)
        {
            AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }

            AudioSource audioSource = _audioSources[(int)Define.Sound.Bgm];
            if (audioSource.isPlaying)
                audioSource.Stop();
            audioSource.pitch = pitch;
            audioSource.clip = audioClip;
            audioSource.Play();
        }
        else 
        {
            AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }

            AudioSource audioSource = _audioSources[(int)Define.Sound.Effect];
            audioSource.pitch = pitch;
            audioSource.PlayOneShot(audioClip);
        }
    }
}
```

---

* Effect의 경우 자주 발생하는데 매번 `AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);` 매니저에서 로드해오는 것이 부담이다. 개선해보자.

```csharp
public class SoundManager
{
    AudioSource[] _audioSources = new AudioSource[(int)Define.Sound.MaxCount];

    Dictionary<string, AudioClip> _audioClips = new Dictionary<string, AudioClip>();

    // ...

    public void Play(Define.Sound type = Define.Sound.Effect, string path = "", float pitch = 1.0f)
    {
        if (path.Contains("Sounds/") == false)
            path = $"Sounds/{path}";

        if(type == Define.Sound.Bgm)
        {
            AudioClip audioClip = Managers.Resource.Load<AudioClip>(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }

            AudioSource audioSource = _audioSources[(int)Define.Sound.Bgm];
            if (audioSource.isPlaying)
                audioSource.Stop();
            audioSource.pitch = pitch;
            audioSource.clip = audioClip;
            audioSource.Play();
        }
        else 
        {
            AudioClip audioClip = GetOrAddAudioClip(path);
            if (audioClip == null)
            {
                Debug.Log($"AudioChip Missing {path}");
                return;
            }

            AudioSource audioSource = _audioSources[(int)Define.Sound.Effect];
            audioSource.pitch = pitch;
            audioSource.PlayOneShot(audioClip);
        }
    }

    AudioClip GetOrAddAudioClip(string path)
    {
        AudioClip audioClip = null;
        if (_audioClips.TryGetValue(path, out audioClip) == false)
        {
            audioClip = Managers.Resource.Load<AudioClip>(path);
            _audioClips.Add(path, audioClip);
        }       
        return audioClip;
    }
}
```

* 단점은 SoundManager는 삭제되지 않는 오브젝트인데 Dictionary로 모든 Resource를 저장하면 시간이 지남에 따라 메모리를 많이 먹게된다.
* Clear도 넣어주자.

```csharp
// Scene의 이동마다 Clear하자
public void Clear()
{
    foreach(AudioSource audioSource in _audioSources)
    {
        audioSource.clip = null;
        audioSource.Stop();
    }
    _audioClips.Clear();
}
```

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/10.4)

* 현재 상태의 문제가 두 가지 있다.
    * 멀리서 오는 소리의 경우 작게 나게 하고싶다.
    * 플레이 도중 종료되는 소리의 처리가 불가능 하다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-10-2-1.png"/>
</p>

* Audio Source의 Spatial Blend를 1로 놓으면 3D Sound 컨트롤이 가능해 지는데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-10-2-2.png"/>
</p>

* 3D Sound는 거리에 따라 소리를 어떻게 처리할지에 관한 사항이다