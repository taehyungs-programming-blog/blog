---
layout: default
title: "22. Serialization - 4"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-34-PacketGenerator-4)

🍁 패킷 생성후 자동으로 복사되게 만들어 보자.

```csharp
// 추가적으로 PDL.xml의 경로를 args로 받게 수정해보자.

static void Main(string[] args)
{
    string pdlPath = "../PDL.xml";

    // ...

    // 아규먼트 체크
    if (args.Length >= 1)
        pdlPath = args[0];

    // ...
```

```xml
<!-- 또 하나 추가적으로 PacketGenerator 빌드시 netcoreapp3.1밑에 안떨어지게 수정 -->
<!-- PacketGenerator.csproj에 아래옵션을 추가하자 -->

<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>netcoreapp3.1</TargetFramework>
    <!-- 이걸 추가하면 된다. -->
	<AppendTargetFrameworkToOutputPath>false</AppendTargetFrameworkToOutputPath>
  </PropertyGroup>

    <!-- 아래옵션은 빌드 후 결과물을 bin\에 넣어달라는 명령 -->
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|AnyCPU'">
    <OutputPath>bin\</OutputPath>
  </PropertyGroup>

  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|AnyCPU'">
    <OutputPath>bin\</OutputPath>
  </PropertyGroup>
```

🍁 이제 본 작업. bat을 이용해 PDL.xml의 경로와 자동복사를 만들어보자.

```bat
START ../PacketGenerator/bin/PacketGenerator.exe ../PacketGenerator/PDL.xml
:: /Y 덮어써주세요
echo f | XCOPY /Y GenPackets.cs "../DummyClient/Packet"
echo f | XCOPY /Y GenPackets.cs "../Server/Packet"
```

{% endraw %}