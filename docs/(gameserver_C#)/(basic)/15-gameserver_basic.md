---
layout: default
title: "15. Packet Generator"
parent: "(기초)"
grand_parent: "(GameServer C# 🎯)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Step. 1 - Packet Generator 생성

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-32-PacketGenerator-2)

* Packet Generator에 List를 넣을수 있게 해보자.

```xml
<?xml version="1.0" encoding="utf-8" ?>
<PDL>
  <packet name="PlayerInfoReq">
    <long name="playerId"/>
    <string name="name"/>
    <!-- List를 넣고자 한다. -->
    <list name="skill">
      <int name="id"/>
      <short name="level"/>
      <float name="duration"/>
    </list>
  </packet>
</PDL>
```

```csharp
public static Tuple<string, string, string> ParseMembers(XmlReader r)
{
    // ...

        string memberType = r.Name.ToLower();
        switch(memberType)
        {
            case "bool":
            case "byte":
            case "short":
            case "ushort":
            case "int":
            case "float":
            case "long":
            case "double":
                memberCode += string.Format(PacketFormat.memberFormat, memberType, memberName);
                readCode += string.Format(PacketFormat.readFormat, memberName, ToMemberType(memberType), memberType);
                writeCode += string.Format(PacketFormat.writeFormat, memberName, memberType);
                break;
            case "string":
                memberCode += string.Format(PacketFormat.memberFormat, memberType, memberName);
                readCode += string.Format(PacketFormat.readStringFormat, memberName);
                writeCode += string.Format(PacketFormat.writeStringFormat, memberName);
                break;
            case "list":
                Tuple<string, string, string> t = ParseList(r);
                memberCode += t.Item1;
                readCode += t.Item2;
                writeCode += t.Item3;
                break;
            default:
                break;
        }
    }
```

```csharp
public static Tuple<string, string, string> ParseList(XmlReader r)
{
    string listName = r["name"];
    if (string.IsNullOrEmpty(listName))
    {
        Console.WriteLine("List without name");
        return null;
    }

    Tuple<string, string, string> t = ParseMembers(r);

    string memberCode = string.Format(PacketFormat.memberListFormat,
        FirstCharToUpper(listName),
        FirstCharToLower(listName),
        t.Item1,
        t.Item2,
        t.Item3);

    string readCode = string.Format(PacketFormat.readListFormat,
        FirstCharToUpper(listName),
        FirstCharToLower(listName));

    string writeCode = string.Format(PacketFormat.writeListFormat,
        FirstCharToUpper(listName),
        FirstCharToLower(listName));

    return new Tuple<string, string, string>(memberCode, readCode, writeCode);
}
```

```csharp
// {0} 리스트 이름 [대문자]
// {1} 리스트 이름 [소문자]
// {2} 멤버 변수들
// {3} 멤버 변수 Read
// {4} 멤버 변수 Write
public static string memberListFormat =
@"public class {0}
{{
	{2}

	public void Read(ReadOnlySpan<byte> s, ref ushort count)
	{{
		{3}
	}}

	public bool Write(Span<byte> s, ref ushort count)
	{{
		bool success = true;
		{4}
		return success;
	}}	
}}
public List<{0}> {1}s = new List<{0}>();";

// ...

public static string readListFormat =
@"this.{1}s.Clear();
ushort {1}Len = BitConverter.ToUInt16(s.Slice(count, s.Length - count));
count += sizeof(ushort);
for (int i = 0; i < {1}Len; i++)
{{
	{0} {1} = new {0}();
	{1}.Read(s, ref count);
	{1}s.Add({1});
}}";

// ...

// {0} 리스트 이름 [대문자]
// {1} 리스트 이름 [소문자]
public static string writeListFormat =
@"success &= BitConverter.TryWriteBytes(s.Slice(count, s.Length - count), (ushort)this.{1}s.Count);
count += sizeof(ushort);
foreach ({0} {1} in this.{1}s)
	success &= {1}.Write(s, ref count);";
```

---

## Step. 2 - 

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-33-PacketGenerator-3)

* 패킷을 좀 더 자동화 해보자.

```csharp
public enum PacketID
{
    // 패킷별로 ID를 할당하고
	PlayerInfoReq = 1,
	Test = 2,
}

// 각 패킷 생성
class PlayerInfoReq : IPacket
// ...
```

```csharp
class PacketFormat
{
    // {0} 패킷 이름/번호 목록
    // {1} 패킷 목록
    public static string fileFormat =
@"using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using ServerCore;

public enum PacketID
{{
{0}
}}

{1}
";
```

```csharp
class Program
{
    static string genPackets;
    static ushort packetId;
    static string packetEnums;

    static void Main(string[] args)
    {
        XmlReaderSettings settings = new XmlReaderSettings()
        {
            IgnoreComments = true,
            IgnoreWhitespace = true
        };

        using (XmlReader r = XmlReader.Create("PDL.xml", settings))
        {
            r.MoveToContent();

            while(r.Read())
            {
                if (r.Depth == 1 && r.NodeType == XmlNodeType.Element)
                    ParsePacket(r);
            }

            // fileformat을 지정
            string fileText = string.Format(PacketFormat.fileFormat, packetEnums, genPackets);

            File.WriteAllText("GenPackets.cs", fileText);
        }
    }

    public static void ParsePacket(XmlReader r)
    {
        if (r.NodeType == XmlNodeType.EndElement)
            return;

        if (r.Name.ToLower() != "packet")
        {
            Console.WriteLine("Invalidate Packet");
            return;
        }

        string packetName = r["name"];
        if (string.IsNullOrEmpty(packetName))
        {
            Console.WriteLine("Packet Empty");
            return;
        }

        Tuple<string, string, string> t = ParseMembers(r);
        genPackets += string.Format(PacketFormat.packetFormat,
            packetName, t.Item1, t.Item2, t.Item3);

        // packet ID를 여기서 할당
        packetEnums += string.Format(PacketFormat.packetEnumFormat, packetName, ++packetId)
            + Environment.NewLine + "\t";
    }
```

---

## Step. 3 - 

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-34-PacketGenerator-4)

* 패킷 생성후 자동으로 복사되게 만들어 보자.

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

* 이제 본 작업. bat을 이용해 PDL.xml의 경로와 자동복사를 만들어보자.

```bat
START ../PacketGenerator/bin/PacketGenerator.exe ../PacketGenerator/PDL.xml
:: /Y 덮어써주세요
echo f | XCOPY /Y GenPackets.cs "../DummyClient/Packet"
echo f | XCOPY /Y GenPackets.cs "../Server/Packet"
```

---

## Step. 4 - 

* [Get Code](https://github.com/Arthur880708/CSharp_GameServer_Basic_Examples/tree/Tag-35-PacketGenerator-5)

* 현재코드의 문제를 살펴보자.

```csharp
class ClientSession : PacketSession
{
    //  ...

    public override void OnRecvPacket(ArraySegment<byte> buffer)
    {
        int pos = 0;

        ushort size = BitConverter.ToUInt16(buffer.Array, buffer.Offset); 
        pos += 2;
        ushort id = BitConverter.ToUInt16(buffer.Array, buffer.Offset + pos);
        pos += 2;

        // Session에서 Packet 수신시 Switch Case문으로 되어있는데
        // Case가 늘어날수록 부하가 늘어나는 구조이다. -> 개선!
        switch ((PacketID)id)
        {
            case PacketID.PlayerInfoReq:
                {
                    PlayerInfoReq p = new PlayerInfoReq();
                    p.Read(buffer);
                    Console.WriteLine($"PlayerInfoReq: {p.playerId} , PlayerName: {p.name}");

                    foreach(PlayerInfoReq.SkillInfo skill in p.skills)
                    {
                        Console.WriteLine($"SKill: {skill.id}, {skill.level}");
                    }

                }
                break;
```

* 그냥 코드를 보는게 나을듯 하여 더이상 정리안함. 코드를 참조할 것.