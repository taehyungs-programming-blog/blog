---
layout: default
title: "네트워크 분석 📨"
parent: "Etc 🛠"
grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://blog.naver.com/PostView.nhn?blogId=ak0402&logNo=221545095163)

## 참고

👩‍💻 0x00이 하나의 바이트이다. (2^8 = 256 / 16진수 2개 = 16 * 16 = 256) `0x12 34 56 78` = 4바이트(32bit)이다.<br>
👩‍💻 빅엔디언의 표기 0x12 34 👉 0x 12 34 / 리틀엔디언의 표기 0x 34 12<br>

---

## Data-link 계층

```
| Destination Mac Address (6Bytes) | Source Mac Address (6Bytes) | Ether Type (2Bytes) | PayLoad (N Bytes...) 
```

```
0000 | ff ff ff ff ff ff 00 03  ff 2a 45 d2 08 06 00 01 ....
```

👩‍💻 **Destination Mac Address (6Bytes)** : `ff ff ff ff ff ff`<br>
👩‍💻 **Source Mac Address (6Bytes)** : `00 03 ff 2a 45 d2`<br>
👩‍💻 **Ether Type (2Bytes)** : `08 06`<br>

---

## Network 계층 - 1

```
| Hardware Address (2Bytes) | Protocol Address Type (2Bytes) | Hardware Address Length(1Bytes) | Protocol Address Length(1Bytes) | Operation (2bytes) | ...
```

```
0000 | ff ff ff ff ff ff 00 03  ff 2a 45 d2 08 06 (여기부터) 00 01
0010 | 08 00 06 04 00 01 00 03  ff 2a 45 d2 c0 a8 00 7a
0020 | 00 00 00 00 00 00 c0 a8  00 0b (여기까지)
```

👩‍💻 **Hardware Address (2Bytes)** : `00 01`<br>
👩‍💻 **Protocol Address Type (2Bytes)** : `08 00`<br>
👩‍💻 ...

---

## Network 계층 - 2

```
0000 | 00 18 f8 5c d1 c1 0c 60  76 13 05 37 08 00 (여기부터 네트워크) 45 00
0010 | 00 d7 59 b4 40 00 80 06  b7 5a c0 a8 01 64 da e8
0020 | 4c 1d (여기까지)
```

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

👩‍💻 `0x00` 하나가 1바이트 이기에 `0x0`은 4비트이다.<br>
👩‍💻 따라서 위 표의 한 줄은 4bytes<br>
👩‍💻 **Version** : 4<br>
👩‍💻 **IHL** : 5<br>
👩‍💻 ...<br>
👩‍💻 **TTL(Time to Live)** : 4bytes * 2 = 8bytes를 건너 뛰고 `80`(128)