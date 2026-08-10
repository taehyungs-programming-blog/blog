---
layout: default
title: "20. Serialization - 2"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-32-PacketGenerator-2)

🍁 Packet Generator에 List를 넣을수 있게 해보자.

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

{% endraw %}