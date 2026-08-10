---
layout: default
title: "[구현] XML Parser"
parent: "(C++) 상세 구현"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-36)

---

👺 우선 현재코드의 문제는

```cpp
// Create Table
{
    // TABLE을 한 번 날려주고
    // TABLE을 아에 새로 만들게 된다
        // 서버가 가동될때마다 이럴 순 없고
        // 기존의 DB정보를 갖고있어야 한다 -> 이걸 XML을 통해하겠음.
    auto query = L"									\
        DROP TABLE IF EXISTS [dbo].[Gold];			\
        CREATE TABLE [dbo].[Gold]					\
        (											\
            [id] INT NOT NULL PRIMARY KEY IDENTITY, \
            [gold] INT NULL							\
        );";

    DBConnection* dbConn = GDBConnectionPool->Pop();
    ASSERT_CRASH(dbConn->Execute(query));
    GDBConnectionPool->Push(dbConn);
}
```

👺 참고로 말하자면 아래와 같은 sql파일을 생성해서 기존 DB정보를 담아야하는데 ...<br>
👺 매번 sql을 만들기도 힘들고, 버전관리도 힘들다. 따라서 XML이나 JSON을 쓰면 오히려 쉽다. 왜 쉬운지는 아래서 참고하자.

```sql
CREATE TABLE [dbo] [Version]
(
    [version] FLOAT NOT NULL
);

CREATE TABLE [dbo] [Gold]
(
    [id] INT NOT NULL PRIMARY KEY IDENTITY,

);
```

👺 결론부터 보자면 xml은 아래와 같이 구성이 된다.

```xml
<?xml version="1.0" encoding="utf-8"?>
<GameDB>
	<Table name="Gold" desc="골드 테이블">
		<Column name="id" type="int" notnull="true" />
		<Column name="gold" type="int" notnull="false" />
		<Column name="name" type="nvarchar(50)" notnull="false" />
		<Column name="createDate" type="datetime" notnull="false" />

        <!-- id라는 column을 기준으로 index를 걸겠다는 말. -->
		<Index type="clustered">
			<PrimaryKey/>
			<Column name="id" />
		</Index>
	</Table>

    <!-- InsertGold를 하는 함수도 자동으로 생성할 예정 -->
	<Procedure name="spInsertGold">
		<Param name="@gold" type="int"/>
		<Param name="@name" type="nvarchar(50)"/>
		<Param name="@createDate" type="datetime"/>
		<Body>
            <!-- CDATA : []안의 내용은 xml 문법이 아니라 DB문법이라고 알린다. -->
			<![CDATA[
			INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(@gold, @name, @createDate);
			]]>
		</Body>		
	</Procedure>
	
    <!-- GetGold를 하는 함수도 자동으로 생성할 예정 -->
	<Procedure name="spGetGold">
		<Param name="@gold" type="int"/>
		<Body>
			<![CDATA[
			SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (@gold)
			]]>
		</Body>
	</Procedure>
	
</GameDB>
```

👺 이제 저 xml을 어떻게 파싱할지 구현해 보자.<br>
👺 사실 xml Parser를 모두 구현하기 보다는 구현이 잘 된 [OpenSource(rapid-xml)](http://rapidxml.sourceforge.net/)를 쓰도록하자

```cpp
// xml 파일을 읽기 위해서 FileUtils생성

namespace fs = std::filesystem;

Vector<BYTE> FileUtils::ReadFile(const WCHAR* path)
{
	Vector<BYTE> ret;

	fs::path filePath{ path };

	const uint32 fileSize = static_cast<uint32>(fs::file_size(filePath));
	ret.resize(fileSize);

	basic_ifstream<BYTE> inputStream{ filePath };
	inputStream.read(&ret[0], fileSize);

	return ret;
}

String FileUtils::Convert(string str)
{
	const int32 srcLen = static_cast<int32>(str.size());

	String ret;
	if (srcLen == 0)
		return ret;

	const int32 retLen = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, NULL, 0);
	ret.resize(retLen);
	::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, &ret[0], retLen);

	return ret;
}
```

```cpp
bool XmlParser::ParseFromFile(const WCHAR* path, OUT XmlNode& root)
{
	Vector<BYTE> bytes = FileUtils::ReadFile(path);
	_data = FileUtils::Convert(string(bytes.begin(), bytes.end()));

	if (_data.empty())
		return false;

	_document = MakeShared<XmlDocumentType>();
	_document->parse<0>(reinterpret_cast<WCHAR*>(&_data[0]));
	root = XmlNode(_document->first_node());    // First Node가 <GameDB>가 된다.
	return true;
}
```

```cpp
// 실사용은 이렇게
XmlNode root;
XmlParser parser;
if (parser.ParseFromFile(L"GameDB.xml", OUT root) == false)
    return 0;

Vector<XmlNode> tables = root.FindChildren(L"Table");
for (XmlNode& table : tables)
{
    String name = table.GetStringAttr(L"name");
    String desc = table.GetStringAttr(L"desc");

    Vector<XmlNode> columns = table.FindChildren(L"Column");
    for (XmlNode& column : columns)
    {
        String colName = column.GetStringAttr(L"name");
        String colType = column.GetStringAttr(L"type");
        bool nullable = !column.GetBoolAttr(L"notnull", false);
        String identity = column.GetStringAttr(L"identity");
        String colDefault = column.GetStringAttr(L"default");
        // Etc...
    }

    Vector<XmlNode> indices = table.FindChildren(L"Index");
    for (XmlNode& index : indices)
    {
        String indexType = index.GetStringAttr(L"type");
        bool primaryKey = index.FindChild(L"PrimaryKey").IsValid();
        bool uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();

        Vector<XmlNode> columns = index.FindChildren(L"Column");
        for (XmlNode& column : columns)
        {
            String colName = column.GetStringAttr(L"name");
        }
    }
}

Vector<XmlNode> procedures = root.FindChildren(L"Procedure");
for (XmlNode& procedure : procedures)
{
    String name = procedure.GetStringAttr(L"name");
    String body = procedure.FindChild(L"Body").GetStringValue();

    Vector<XmlNode> params = procedure.FindChildren(L"Param");
    for (XmlNode& param : params)
    {
        String paramName = param.GetStringAttr(L"name");
        String paramType = param.GetStringAttr(L"type");
        // TODO..
    }
}
```

👺 파싱한 xml 데이터를 기반으로 DB에 명령을 보내는 부분이 있는데 코드가 엄청길어서 별도로 정리 안함. 필요하다면 찾아볼 것.