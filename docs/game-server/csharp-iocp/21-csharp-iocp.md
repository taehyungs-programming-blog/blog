---
layout: default
title: "21. Serialization - 3"
parent: "(C# IOCP)"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code](https://github.com/EasyCoding-7/CSharp_Windows_GameServer_Tutorial/tree/Tag-33-PacketGenerator-3)

🍁 패킷을 좀 더 자동화 해보자.

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