---
layout: default
title: "01. ssl"
parent: "(Etc.)"
grand_parent: "(C++)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## SSL이란?

> 참고: 현재는 TLS(Transport Layer Security)가 SSL의 후속 버전으로 사용되지만, 관습적으로 "SSL"이라는 용어가 여전히 널리 쓰인다. "SSL 인증서"라고 부르지만 실제로는 TLS를 사용

* SSL(Secure Sockets Layer)은 인터넷에서 데이터를 안전하게 전송하기 위한 암호화 프로토콜
* 클라이언트(브라우저)와 서버 사이의 통신을 암호화하여 제3자가 데이터를 도청하거나 변조하는 것을 방지

### SSL이 필요한 이유

* 암호화 없이 통신할 때의 위험

```
[사용자] ----"비밀번호: 1234"----> [서버]
              ↑
           해커가 중간에서 도청 가능!
```

* SSL로 암호화된 통신

```
[사용자] ----"@#$%^&*암호화된데이터"----> [서버]
              ↑
           해커가 봐도 내용을 알 수 없음
```

### SSL 작동 원리 (핸드셰이크)

```
[클라이언트]                              [서버]
     |                                      |
     |---(1) 연결 요청 (지원하는 암호화 방식)-->|
     |                                      |
     |<--(2) 인증서 + 공개키 전송-------------|
     |                                      |
     |---(3) 인증서 검증 후, 대칭키 생성------>|
     |       (공개키로 암호화해서 전송)        |
     |                                      |
     |<--(4) 연결 완료------------------------|
     |                                      |
     |<=====(5) 대칭키로 암호화된 통신=======>|
```

---

## ssl 인증서 시스템

* 인증서란?
    * 디지털 인증서는 특정 공개키가 특정 주체(도메인, 조직)에 속한다는 것을 신뢰할 수 있는 제3자(CA)가 보증하는 전자 문서

```
┌─────────────────────────────────────────────────────┐
│                    SSL 인증서                        │
├─────────────────────────────────────────────────────┤
│  버전 (Version): v3                                  │
│  일련번호 (Serial Number): 고유 식별 번호             │
│  서명 알고리즘: SHA256withRSA                        │
├─────────────────────────────────────────────────────┤
│  발급자 (Issuer):                                    │
│    CN=DigiCert Global Root CA                       │
│    O=DigiCert Inc                                   │
│    C=US                                             │
├─────────────────────────────────────────────────────┤
│  유효기간:                                           │
│    시작: 2024-01-01 00:00:00 UTC                    │
│    만료: 2025-01-01 23:59:59 UTC                    │
├─────────────────────────────────────────────────────┤
│  주체 (Subject):                                     │
│    CN=www.example.com                               │
│    O=Example Company Ltd                            │
│    L=Seoul, C=KR                                    │
├─────────────────────────────────────────────────────┤
│  주체 공개키 정보:                                    │
│    알고리즘: RSA (2048 bits)                         │
│    공개키: 30 82 01 0a 02 82 01 01 00 ...           │
├─────────────────────────────────────────────────────┤
│  확장 필드 (Extensions):                             │
│    - Subject Alternative Name (SAN)                 │
│    - Key Usage                                      │
│    - Basic Constraints                              │
├─────────────────────────────────────────────────────┤
│  CA의 디지털 서명: 4a 7b 9c ...                      │
└─────────────────────────────────────────────────────┘
```

---

## OpenSSL과 SChannel 비교 설명

### OpenSSL

* OpenSSL은 오픈소스 암호화 라이브러리로, SSL/TLS 프로토콜을 구현한 가장 널리 사용되는 도구

* 주요 특징
    * 크로스 플랫폼: Linux, macOS, Windows, BSD 등 거의 모든 운영체제에서 사용 가능
    * 오픈소스: 무료로 사용 가능하며 소스 코드를 직접 확인/수정 가능
    * 풍부한 기능: 인증서 생성, 암호화/복호화, 해시 함수, 디지털 서명 등 다양한 암호화 기능 제공
    * 널리 사용됨: Apache, Nginx, curl, Git 등 수많은 소프트웨어에서 사용

### SChannel (Secure Channel)

* SChannel은 Microsoft에서 개발한 Windows 전용 보안 패키지로, Windows의 네이티브 SSL/TLS 구현체

* 주요 특징
    * Windows 전용: Windows 운영체제에 내장되어 있음
    * 시스템 통합: Windows 인증서 저장소와 완벽하게 통합
    * 자동 업데이트: Windows Update를 통해 자동으로 보안 패치 적용
    * SSPI 기반: Windows Security Support Provider Interface의 일부

| 항목 | OpenSSL | SChannel |
|:---|:---|:---|
| 플랫폼 | 크로스 플랫폼 | Windows 전용 |
| 라이선스 | Apache License 2.0 | Windows 라이선스 |
| 인증서 저장소 | 파일 기반 (PEM, DER) | Windows 인증서 저장소 |
| 업데이트| 수동 업데이트 필요 | Windows Update 자동 적용 |
| 커스터마이징 | 매우 유연함 | 제한적 |
| 사용 예 | curl, Git, Apache | IE, Edge, .NET |

---

## Unreal의 HTTP/SSL 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                    Unreal HTTP Layer                        │
│                      (FHttpModule)                          │
├─────────────────────────────────────────────────────────────┤
│                   HTTP Implementation                        │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────────┐    │
│  │ WinHttp │  │  Curl   │  │  Apple  │  │   Android   │    │
│  │(Windows)│  │(Linux)  │  │ (iOS/   │  │   (Java)    │    │
│  │         │  │         │  │  macOS) │  │             │    │
│  └────┬────┘  └────┬────┘  └────┬────┘  └──────┬──────┘    │
├───────┼────────────┼────────────┼──────────────┼───────────┤
│       ▼            ▼            ▼              ▼            │
│   SChannel     OpenSSL    SecureTransport   OpenSSL        │
│  (Windows)                  (Apple)                         │
└─────────────────────────────────────────────────────────────┘
```

```
Engine/
└── Content/
    └── Certificates/
        └── cacert.pem    ← Mozilla의 CA 인증서 번들
```

```
// Engine/Source/Runtime/Online/SSL/

FSslModule
├── FSslManager
│   ├── InitializeSsl()           // OpenSSL 초기화
│   ├── ShutdownSsl()             // OpenSSL 정리
│   └── GetCertificateManager()   // 인증서 관리자 접근
│
└── FSslCertificateManager
    ├── AddCertificatesToSslContext()  // CA 인증서 로드
    ├── SetPinnedPublicKeys()          // 인증서 핀닝
    └── VerifySslCertificates()        // 인증서 검증
```