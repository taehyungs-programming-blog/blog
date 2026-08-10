---
layout: default
title: "[구현] 패킷 자동화(Protobuf 활용, 코드자동화)"
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

* [Get This Code 🌎(Protobuf)](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-27)
* [Get This Code 🌎(코드 자동화)](https://github.com/EasyCoding-7/Windows_Game_Server_Tutorial/tree/RA-Tag-28)

---

{% raw %}

🐔 우선 지난강과 달라진 점이 패킷을 protobuf로 처리하는데 [여기 🌍](https://taehyungs-programming-blog.github.io/blog/docs/cpp/protobuf/2022-02-21-proto-1/)를 참조

---

🐔 protobuf를 적용하긴 했으나 아직까진 반복적인 작업이 요구된다.

* 불편한 작업 정리
    * 배치파일 실행
    * `.proto` 파일 생성
    * 생성된 `.h`, `.cc` 파일 옮기기
    * ...

🐔 우선 쉬운거 부터 처리해보자, 

## 생성된 .h, .cc 파일 옮기기

```bash
# GenPackets.bat

# pushd %~dp0 - GenPackets.bat가 있는 경로가 시작경로가 되게 변경
pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto
IF ERRORLEVEL 1 PAUSE

# /Y - 덮어쓰기 yes
XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../DummyClient"
XCOPY /Y Enum.pb.cc "../../../DummyClient"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
```

🐔 빌드시에도 자동으로 `GenPackets.bat`을 실행하도록 해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/iocp/iocp-50-1.png"/>
</p>

🐔 단, `*.proto` 파일이 수정되었을때 `GenPackets.bat`가 호출되어야 하니 조금더 수정이 필요하다.<br>
🐔 그냥은 불가능하고 `.vcxproj`파일의 수정이 필요하다

```xml
<!-- ... -->

  <ItemGroup>
    <UpToDateCheckInput Include="..\Common\Protobuf\bin\Enum.proto" />
    <UpToDateCheckInput Include="..\Common\Protobuf\bin\Protocol.proto" />
    <UpToDateCheckInput Include="..\Common\Protobuf\bin\Struct.proto" />
  </ItemGroup>

<!-- ... -->
```

---

## 코드 자동화 필요 부분

🐔 우선 대략 어떻게 쓰일지 설명하면

```cpp
void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = PacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    // 패킷이 수신되면 HandlePacket를 호출
	ServerPacketHandler::HandlePacket(session, buffer, len);
}
```

```cpp
static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    // 정의된 패킷내에서 처리된다.
    return GPacketHandler[header->id](session, buffer, len);
}
```

🐔 이 정의된 패킷내의 패킷이 자동화 되어야 함.

```cpp
#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
    // TODO : 전체 패킷 자동화 필요
	PKT_S_TEST = 1,
	PKT_S_LOGIN = 2,
};

// TODO : 자동화
// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt);

class ServerPacketHandler
{
public:
	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
            /*
                bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
                {
                    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
                    // TODO : Log
                    return false;
                }
            */

		GPacketHandler[PKT_S_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

	// TODO : 자동화
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};
```

---

## 코드를 자동화 해보자.

### python jinja2

```bash
$ pip install jinja2
$ pip install pyinstaller
```

```py
import argparse     # 실행파일 전달 아규먼트 처리 라이브러리
import jinja2       # 코드 자동화 라이브러리
import ProtoParser  # 우리가 만든 passer

def main():

	arg_parser = argparse.ArgumentParser(description = 'PacketGenerator')

    # .proto 파일의 경로 지정
	arg_parser.add_argument('--path', type=str, default='C:/Rookiss/CPP_Server/Server/Common/Protobuf/bin/Protocol.proto', help='proto path')

    # 출력파일의 경로 지정
	arg_parser.add_argument('--output', type=str, default='TestPacketHandler', help='output file')

    # 
	arg_parser.add_argument('--recv', type=str, default='C_', help='recv convention')

    # 
	arg_parser.add_argument('--send', type=str, default='S_', help='send convention')

    # 사용은 args.path = --path 이런식으로 사용됨.(아래참조)
	args = arg_parser.parse_args()

	parser = ProtoParser.ProtoParser(1000, args.recv, args.send)
	parser.parse_proto(args.path)
	file_loader = jinja2.FileSystemLoader('Templates')
	env = jinja2.Environment(loader=file_loader)

	template = env.get_template('PacketHandler.h')
	output = template.render(parser=parser, output=args.output)

	f = open(args.output+'.h', 'w+')
	f.write(output)
	f.close()

	print(output)
	return

if __name__ == '__main__':
	main()
```

```py
# ProtoParser.py

class ProtoParser():
	def __init__(self, start_id, recv_prefix, send_prefix):
		self.recv_pkt = []	    # 수신 패킷 목록
		self.send_pkt = []      # 송신 패킷 목록
        self.total_pkt = []     # 모든 패킷 목록
		self.start_id = start_id
		self.id = start_id
		self.recv_prefix = recv_prefix
		self.send_prefix = send_prefix

	def parse_proto(self, path):
        # .proto 파일을 라인단위로 읽어준다.
		f = open(path, 'r')
		lines = f.readlines()

		for line in lines:
            # message로 시작하는지 확인
			if line.startswith('message') == False:
				continue

            # S_, C_로 시작하는지 확인
			pkt_name = line.split()[1].upper()
			if pkt_name.startswith(self.recv_prefix):
				self.recv_pkt.append(Packet(pkt_name, self.id))
			elif pkt_name.startswith(self.send_prefix):
				self.send_pkt.append(Packet(pkt_name, self.id))
			else:
				continue

			self.total_pkt.append(Packet(pkt_name, self.id))
			self.id += 1

		f.close()

class Packet:
	def __init__(self, name, id):
		self.name = name
		self.id = id
```

```cpp
// template 파일

#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
// total_pkt 내에서 반복문을 돌아달라는 말.
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

// 역시 recv_pkt내에서 반복문을 돌아달라는 말.
{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(PacketSessionRef& session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}

class {{output}}
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;

{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}); }
{%- endfor %}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};

```

---

## 참고) pyinstaller 사용

```bash
pushd %~dp0
pyinstaller --onefile PacketGenerator.py
MOVE .\dist\PacketGenerator.exe .\GenPackets.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\PacketGenerator.spec
PAUSE
```

{% endraw %}