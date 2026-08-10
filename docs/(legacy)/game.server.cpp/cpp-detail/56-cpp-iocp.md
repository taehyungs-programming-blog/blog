---
layout: default
title: "[구현] DB Connection"
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

* [Get This Code 🌎](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-35)

---

🦍 이번 강은 DB를 어떻게 연동하는지에 관한내용이며 필요하면 찾아보지 너무 심오하게 볼 필요는 없다.(한 번 만들어 두면 변경할 일이 잘 없음.)

---

## DB 세팅

* 목차 -> 보기 -> SQL Server 개체 탐색기
    * (만약에 뜨지않으면 Visual Studio Installer에서 DB관련 패키지를 설치해야함)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/iocp/iocp-56-1.png"/>
</p>

* 데이터베이스 우클릭 후 새 DB생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/iocp/iocp-56-2.png"/>
</p>

* 생성한 Db에서 연결 문자열을 기록해 둔다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/iocp/iocp-56-3.png"/>
</p>

* 기록된 연결문자열을 아래에 사용한다.

```cpp
// DB Connect
ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\ProjectsV13;Database=ServerDb;Trusted_Connection=Yes;"));
```

---

## 코드 구현

```cpp
// 대략 이렇게 쓰이길 원한다.

GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;")

// Create Table
{
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

// Add Data
for (int32 i = 0; i < 3; i++)
{
    DBConnection* dbConn = GDBConnectionPool->Pop();
    // 기존에 바인딩 된 정보 날림
    dbConn->Unbind();

    // 넘길 인자 바인딩
    int32 gold = 100;
    SQLLEN len = 0;

    // 넘길 인자 바인딩
    ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

    // SQL 실행
    ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold]) VALUES(?)"));

    GDBConnectionPool->Push(dbConn);
}

// Read
{
    DBConnection* dbConn = GDBConnectionPool->Pop();
    // 기존에 바인딩 된 정보 날림
    dbConn->Unbind();

    int32 gold = 100;
    SQLLEN len = 0;
    // 넘길 인자 바인딩
    ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

    int32 outId = 0;
    SQLLEN outIdLen = 0;
    ASSERT_CRASH(dbConn->BindCol(1, SQL_C_LONG, sizeof(outId), &outId, &outIdLen));

    int32 outGold = 0;
    SQLLEN outGoldLen = 0;
    ASSERT_CRASH(dbConn->BindCol(2, SQL_C_LONG, sizeof(outGold), &outGold, &outGoldLen));

    // SQL 실행
    ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold FROM [dbo].[Gold] WHERE gold = (?)"));

    while (dbConn->Fetch())
    {
        cout << "Id: " << outId << " Gold : " << outGold << endl;
    }

    GDBConnectionPool->Push(dbConn);
}
```

```cpp
class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool					Connect(int32 connectionCount,              // 몇개의 Connection을 맺을것인가
                                    const WCHAR* connectionString);     // connection 할때 같이 보내지는 string
    /*
        // 대략 이렇게 쓰인다.
        GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;")
    */
	void					Clear();

	DBConnection*			Pop();
	void					Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV					_environment = SQL_NULL_HANDLE;
	Vector<DBConnection*>	_connections;
};
```

```cpp
bool DBConnectionPool::Connect(int32 connectionCount, const WCHAR* connectionString)
{
	WRITE_LOCK;

    // SQL 핸들 할당
	if (::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_environment) != SQL_SUCCESS)
		return false;

    // 핸들에 ODBC 버전 할당
	if (::SQLSetEnvAttr(_environment, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0) != SQL_SUCCESS)
		return false;

    // conCount만큼 커넥션을 맺음.
	for (int32 i = 0; i < connectionCount; i++)
	{
		DBConnection* connection = xnew<DBConnection>();
        // sql-handle인 _environment와 connection string인 connectionString를
            // DBConnection의 Connect에 넘긴다(Connect 설명은 아래참조)
		if (connection->Connect(_environment, connectionString) == false)
			return false;

		_connections.push_back(connection);
	}

    /*
        * DBConnectionPool는 sql 핸들 할당과 connection 관리만 하게 된다. *
    */

	return true;
}
```

```cpp
// DBConnection : 실제 DB와 연동을 담당

#pragma once
#include <sql.h>
#include <sqlext.h>

class DBConnection
{
public:
	bool			Connect(SQLHENV henv, const WCHAR* connectionString);
    /*
        bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString)
        {
            // 이것 역시 대략 이렇게 쓴다고 이해만 하고 필요할때 찾아보자.

            if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
                return false;

            WCHAR stringBuffer[MAX_PATH] = { 0 };
            ::wcscpy_s(stringBuffer, connectionString);

            WCHAR resultString[MAX_PATH] = { 0 };
            SQLSMALLINT resultStringLen = 0;

            SQLRETURN ret = ::SQLDriverConnectW(
                _connection,
                NULL,
                reinterpret_cast<SQLWCHAR*>(stringBuffer),
                _countof(stringBuffer),
                OUT reinterpret_cast<SQLWCHAR*>(resultString),
                _countof(resultString),
                OUT & resultStringLen,
                SQL_DRIVER_NOPROMPT
            );

            if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
                return false;

            return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
        }
    */
	void			Clear();

	bool			Execute(const WCHAR* query);
    /*
        bool DBConnection::Execute(const WCHAR* query)
        {
            SQLRETURN ret = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
            if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
                return true;

            HandleError(ret);
            return false;
        }
    */
	bool			Fetch();
	int32			GetRowCount();
	void			Unbind();

public:
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
    /*
        bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
        {
            SQLRETURN ret = ::SQLBindParameter(_statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
            if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
            {
                HandleError(ret);
                return false;
            }

            return true;
        }
    */
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};
```

---

## 코드 개선

```cpp
// 아래와 같이 직접적으로 타입을 지정하다보니
// SQL_INTEGER, SQL_C_LONG
// 실수의 요소가 많다
ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

int32 outId = 0;
SQLLEN outIdLen = 0;
ASSERT_CRASH(dbConn->BindCol(1, SQL_C_LONG, sizeof(outId), &outId, &outIdLen));

int32 outGold = 0;
SQLLEN outGoldLen = 0;
ASSERT_CRASH(dbConn->BindCol(2, SQL_C_LONG, sizeof(outGold), &outGold, &outGoldLen));

// ...
```

```cpp
class DBConnection
{
// ...

public:
	bool			BindParam(int32 paramIndex, bool* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, float* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, double* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int8* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int16* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int32* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int64* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, const WCHAR* str, SQLLEN* index);
	bool			BindParam(int32 paramIndex, const BYTE* bin, int32 size, SQLLEN* index);

	bool			BindCol(int32 columnIndex, bool* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, float* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, double* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int8* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int16* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int32* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int64* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index);
	bool			BindCol(int32 columnIndex, BYTE* bin, int32 size, SQLLEN* index);

private:
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

// ...
```

```cpp
// DB Bind도 개선을 해보자

// Add Data
for (int32 i = 0; i < 3; i++)
{
    DBConnection* dbConn = GDBConnectionPool->Pop();
    
    DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");

    int32 gold = 100;
    dbBind.BindParam(0, gold);
    WCHAR name[100] = L"루키스";
    dbBind.BindParam(1, name);
    TIMESTAMP_STRUCT ts = {2021, 6, 5};
    dbBind.BindParam(2, ts);

    ASSERT_CRASH(dbBind.Execute());
```

```cpp
// Read
{
    DBConnection* dbConn = GDBConnectionPool->Pop();

    DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)");
    
    int32 gold = 100;
    dbBind.BindParam(0, gold);

    int32 outId = 0;		
    int32 outGold = 0;
    WCHAR outName[100];
    TIMESTAMP_STRUCT outDate = {};
    dbBind.BindCol(0, OUT outId);
    dbBind.BindCol(1, OUT outGold);
    dbBind.BindCol(2, OUT outName);
    dbBind.BindCol(3, OUT outDate);

    ASSERT_CRASH(dbBind.Execute());

    wcout.imbue(locale("kor"));

    while (dbConn->Fetch())
    {
        wcout << "Id: " << outId << " Gold : " << outGold << " Name: " << outName << endl;
        wcout << "Date : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
    }

    GDBConnectionPool->Push(dbConn);
```