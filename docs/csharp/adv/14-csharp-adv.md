---
layout: default
title: "14. Stream Decorator"
parent: (C# Adv)
grand_parent: "(C#)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Decoator Pattern

* **Decoator Pattern** - Run Time에 기능을 추가하고싶다.

```csharp
using System;

class Fighter       // 전투기
{
    public virtual void Fire()
    {
        Console.WriteLine("Fire Missile");
    }
}

class Program
{
    public static void Main()
    {
        // Fighter에 아이템을 추가하려한다면??
        Fighter fg = new Fighter();
        fg.Fire();
    }
}
```

```csharp
using System;

class Fighter
{
    public virtual void Fire()
    {
        Console.WriteLine("Fire Missile");
    }
}
// 상속을 사용한 기능 추가
class RightMissileFighter : Fighter
{
    public override void Fire()
    {
        base.Fire(); // 기존 기능 사용
        Console.WriteLine("Fire Right Missile"); // 추가 기능
    }
}

class Program
{
    public static void Main()
    {
        Fighter fg = new Fighter();
        fg.Fire();

        // 아이템 획득
        // 그런데 이게 맞나? 새로운 객체를 생성했는데??
            // -> 해결해보자.
        Fighter fg2 = new RightMissileFighter();
        fg2.Fire();
    }
}
```

```csharp
using System;

class Fighter
{
    public virtual void Fire()
    {
        Console.WriteLine("Fire Missile");
    }
}

class RightMissile
{
    private Fighter fighter = null;
    
    // Fighter에 RighMissle을 추가하게 된다
    public RightMissile(Fighter fg) { fighter = fg; }

    public void Fire()
    {
        fighter.Fire(); // 기존 객체의 기능 사용
        Console.WriteLine("Fire Right Missile"); 
    }
}

class Program
{
    public static void Main()
    {
        Fighter fg = new Fighter();
        fg.Fire();

        // 아이템 획득
        RightMissile fg2 = new RightMissile(fg);
        fg2.Fire();
    }
}
```

```csharp
// 좀 더 기능을 확장해 보자.

using System;

interface Item
{
    void Fire();
}

class Fighter : Item
{
    public virtual void Fire()
    {
        Console.WriteLine("Fire Missile");
    }
}

class RightMissile : Item
{
    private Item fighter = null;

    public RightMissile(Item fg) { fighter = fg; }

    public void Fire()
    {
        fighter.Fire(); // 기존 객체의 기능 사용
        Console.WriteLine("Fire Right Missile");
    }
}

class LeftMissile : Item
{
    private Item fighter = null;

    public LeftMissile(Item fg) { fighter = fg; }

    public void Fire()
    {
        fighter.Fire(); 
        Console.WriteLine("Fire Left Missile");
    }
}

class Program
{
    public static void Main()
    {
        Fighter fg = new Fighter();
        fg.Fire();

        // 아이템 획득
        RightMissile fg2 = new RightMissile(fg);
        fg2.Fire();

        LeftMissile fg3 = new LeftMissile(fg2);
        fg3.Fire();
    }
}
```

---

## Stream Decorator

```csharp
using System;
using System.IO;
using System.IO.Compression;
using System.Net;

class Program
{
    public static void Main()
    {
        byte[] buff = new byte[1024 * 1024]; // 1M

        FileStream fs = new FileStream("d:\\a.dat", FileMode.Create);

        // 데코레이터로 작동한다.
        GZipStream gs = new GZipStream(fs, CompressionLevel.Fastest);

        BufferedStream bs = new BufferedStream(gs);

        
        //fs.Write(buff, 0, buff.Length);
        bs.Write(buff, 0, buff.Length);
        bs.Close();
        gs.Close();
        fs.Close();
    }
}
```