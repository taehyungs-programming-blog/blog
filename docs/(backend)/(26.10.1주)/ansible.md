---
layout: default
title: "Ansible"
parent: "Backend"
nav_order: 10
permalink: "/(backend)/ansible/"
---

{% raw %}

# Ansible

## 개요

사내 네트워크 장비 약 5,000대를 Ansible로 매일 밤
백업·생성·배포해 설정을 강제한다.
수동 변경은 다음 날 밤 golden config로 덮어써진다.

## 배경

6개 대륙 오피스의 멀티 벤더 장비 수천 대를 수동으로 관리하기 어려워졌다.

주요 문제:

- 지역별 하드웨어 차이
  (예: 인도와 멕시코의 스위치가 다름, 대형 오피스는 별도 장비)
- 장비당 2,000행 이상의 설정 파일, 문자 하나 오류로 파서가 깨짐
- 잘못된 설정 하나가 전 세계 장비로 퍼질 위험
  ("unpredictable blast radius")
- 특정 벤더 문법만을 위한 중복 변수
- 신규 장비 추가, 노후 장비 제거 등 생명주기 관리
- Ansible 미지원 장비는 별도 오케스트레이션 필요

| 항목    | 내용                                                                |
| ----- | ----------------------------------------------------------------- |
| 장비 수  | 약 5,000대                                                          |
| 리전    | EMEA, APAC, AMER West, AMER East, LATAM                           |
| 벤더    | Cisco, Juniper, Arista                                            |
| 장비 종류 | 스위치, 라우터, 방화벽, VPN concentrator, 게이트웨이, AP, 콘솔 서버                 |
| 관련 도구 | Ansible(온프레미스 장비), Terraform(클라우드), Puppet, GitHub, Jinja, Python |

## Ansible 기초

뒤 내용을 읽는 데 필요한 Ansible 개념을 정리한다.
Ansible을 처음 보는 사람 기준으로 적었다.

### Ansible이란

여러 장비에 같은 작업을 자동으로 실행하는 도구다.

- 관리 서버 한 대에 Ansible을 설치한다.
- 관리 서버가 각 장비에 SSH로 접속해 명령을 보낸다.
- 장비에는 별도 프로그램을 설치하지 않는다.
- 할 일은 YAML 파일(playbook)에 적고, 실행하면 대상 장비 전부에 적용된다.

```
          ┌──────────── SSH ───────────▶ sel-sw01 (Cisco)
[관리 서버] ├──────────── SSH ───────────▶ tyo-rt01 (Juniper)
 Ansible   └──────────── SSH ───────────▶ sin-fw01 (Arista)
```

### 구성 요소

| 이름       | 역할                        | 이 문서에서의 예                        |
| -------- | ------------------------- | -------------------------------- |
| 인벤토리     | 관리할 장비 목록과 그룹             | `inventory/hosts.ini`            |
| 그룹       | 장비를 묶는 단위                 | `[apac]`, `[emea]`               |
| 변수       | 장비나 그룹별 설정 값              | `group_vars/apac.yml`            |
| playbook | 어느 장비에 무엇을 할지 적은 파일       | `playbooks/backup.yml`           |
| task     | playbook 안의 작업 하나         | "running config 백업"              |
| 모듈       | task가 실제로 호출하는 기능         | `cisco.ios.ios_config`           |
| 컬렉션      | 모듈·플러그인을 묶어 배포하는 패키지      | `cisco.ios`, `ansible.netcommon` |
| 템플릿      | 변수를 넣어 파일을 만드는 틀 (Jinja)  | `templates/ios/aaa.j2`           |
| role     | task·템플릿·변수를 재사용 단위로 묶은 것 | Config Role                      |
| facts    | 장비에서 자동으로 읽어 온 정보         | 모델, OS 버전, 포트                    |
| Vault    | 비밀번호 등을 암호화해 보관하는 기능      | 장비 접속 비밀번호                       |

### 파일 구조

```
project/
├── inventory/
│   └── hosts.ini            ← 장비 목록 + 그룹
├── group_vars/
│   ├── all.yml              ← 전체 공통 변수
│   └── apac.yml             ← apac 그룹 변수
├── host_vars/
│   └── sel-sw01.yml         ← 장비 1대 전용 변수
├── templates/
│   ├── ios/                 ← Cisco용 Jinja 템플릿
│   └── junos/               ← Juniper용 Jinja 템플릿
└── playbooks/
    ├── backup.yml           ← 무엇을 할지
    └── nightly.yml
```

- 인벤토리: 어느 장비가 있는지
- 변수: 장비·그룹마다 다른 값
- 템플릿: 설정 파일의 틀
- playbook: 무엇을 어느 장비에 할지

### 인벤토리

```ini
# inventory/hosts.ini
[apac]
sel-sw01  ansible_network_os=cisco.ios.ios
tyo-rt01  ansible_network_os=junipernetworks.junos.junos
sin-fw01  ansible_network_os=arista.eos.eos

[apac:vars]
ansible_connection=ansible.netcommon.network_cli
```

위 파일은 **인벤토리**다. Ansible이 관리할 장비와 접속 방법을 적은 목록이다.

**왜 필요한가**

Ansible은 관리 서버 한 대에서 실행되고, 각 장비에 SSH로 접속해 명령을 보낸다.
장비에는 별도 프로그램을 설치하지 않는다. 그래서 Ansible은 관리할 장비를 스스로
찾지 못하고, 실행 전에 아래 네 가지를 알아야 한다.

1. **어떤 장비가 있는가**
   - 5,000대의 이름과 IP 목록이 필요하다.
     - 인벤토리가 없으면 실행할 때마다 IP를 직접 나열해야 한다.
2. **어떤 장비끼리 묶는가**
   - 리전 단위로 백업·생성·배포한다.
     - `[apac]` 그룹이 있어야 `hosts: apac` 한 줄로 APAC 전체를 대상으로 삼는다.
     - 그룹별 변수도 줄 수 있다. 예: APAC 장비는 `apac.yml`의 DHCP 서버를 쓴다.
     - YAML 하나로 전역 또는 특정 리전·오피스에 반영하는 것이
       이 그룹 구조로 가능하다.
3. **장비 OS가 무엇인가**
   - Cisco·Juniper·Arista는 명령어가 다르다.
     - `ansible_network_os`로 사용할 모듈과 문법을 정한다.
       이 값이 없으면 장비와 통신하지 못한다.
4. **어떻게 접속하는가**
   - 접속 방식(`network_cli`), IP, 계정 정보.

playbook은 "무엇을 할지"를, 인벤토리는 "어느 장비에 할지"를 정한다.

**각 행의 의미**

- `[apac]`
  - `apac` 그룹을 만든다. 아래에 적은 장비가 이 그룹에 속한다.
  - playbook에 `hosts: apac`라고 쓰면 그룹의 모든 장비가 대상이 된다.
  - "리전 전체 백업"의 리전이 이 그룹이다.
- `sel-sw01  ansible_network_os=cisco.ios.ios`
  - `sel-sw01`: 장비 이름 (서울 스위치를 가정한 이름)
  - `ansible_network_os`: 장비 OS. Ansible이 이 값으로 모듈과 명령 문법을 고른다.
  - `cisco.ios.ios`: Cisco IOS 장비
- `tyo-rt01`: 도쿄 라우터를 가정한 이름, Juniper Junos 장비
- `sin-fw01`: 싱가포르 장비를 가정한 이름, Arista EOS 장비
- `[apac:vars]`: `apac` 그룹의 모든 장비에 공통으로 적용할 변수
- `ansible_connection=ansible.netcommon.network_cli`
  - SSH로 접속해 CLI 명령을 보내는 방식이다.
  - 서버는 보통 SSH로 접속해 Python을 실행한다.
    네트워크 장비에서는 Python을 실행할 수 없어서 이 방식을 쓴다.

실제 환경에서는 보통 접속 IP와 계정도 넣는다.

```ini
[apac]
sel-sw01  ansible_host=10.20.1.2  ansible_network_os=cisco.ios.ios

[apac:vars]
ansible_connection=ansible.netcommon.network_cli
ansible_user=netauto
ansible_password="{{ vault_net_password }}"
```

- `ansible_host`: 접속할 IP. 없으면 장비 이름으로 DNS를 조회한다.
- `ansible_user`, `ansible_password`: 접속 계정.
  비밀번호는 Ansible Vault로 암호화해 넣는다.

### 그룹은 어디서 쓰나

그룹 이름은 두 군데에서 쓴다.

1. **playbook의 `hosts`**: 이 그룹에 무엇을 할지
   ```yaml
   # playbooks/backup.yml
   - hosts: apac
     tasks:
       - cisco.ios.ios_config:
           backup: yes
   ```
2. **`group_vars/<그룹이름>.yml`**: 이 그룹이 쓸 변수
   ```yaml
   # group_vars/apac.yml
   dhcp_servers:
     - 10.20.0.5
   ansible_connection: ansible.netcommon.network_cli
   ```
   - `[apac:vars]`와 역할이 같다.
   - 변수가 많아지면 인벤토리에 적지 않고 이 파일로 분리한다.
   - Ansible은 `group_vars/` 아래에서 그룹 이름과 같은 파일을 자동으로 읽는다.
   - `group_vars/all.yml`은 모든 장비에 적용된다.
   - `host_vars/<장비이름>.yml`은 장비 1대에만 적용된다.

### `hosts`는 왜 쓰나

playbook을 **어느 장비에 실행할지** 정하는 줄이다.
인벤토리에는 5,000대가 모두 있으므로, 매번 그중 대상을 골라야 한다.

1. **playbook에는 대상이 반드시 있어야 한다**
   - `tasks`에는 할 일만 있다. 예: "running config 백업"
     - `hosts`가 없으면 어느 장비에 할지 몰라 Ansible이 실행하지 않는다.
2. **전체가 아니라 리전 단위로 나눠 실행한다**
   - `hosts: all`이면 5,000대에 한 번에 실행된다.
     - 잘못된 설정이 한 번에 전 세계 장비로 퍼질 수 있다.
     - `hosts: apac`이면 문제가 생겨도 영향이 APAC 안에 머문다.
     - 리전마다 시간대가 달라 각 리전의 밤에 맞춰 따로 실행할 수 있다.
3. **장비 이름 대신 그룹 이름을 쓴다**
   - `hosts: sel-sw01,tyo-rt01,...`처럼 나열하면
     장비가 바뀔 때마다 playbook을 고쳐야 한다.
     - `hosts: apac`이면 인벤토리 `[apac]`에 장비를 넣고 빼기만 하면 된다.
4. **같은 playbook을 다른 리전에도 쓴다**
   ```yaml
   - hosts: "{{ target }}"
   ```

인벤토리는 "관리하는 장비 전체 목록", `hosts`는 "이번 실행의 대상"이다.

### `network_cli`는 어디에 있나

직접 만드는 파일이 아니다. Ansible 컬렉션에 들어 있는 **접속 플러그인**이다.

```bash
ansible-galaxy collection install ansible.netcommon
```

설치하면 실제 코드는 이런 경로에 생긴다.

```
~/.ansible/collections/ansible_collections/ansible/netcommon/plugins/connection/network_cli.py
```

- 인벤토리에는 이름만 적는다.
  Ansible이 설치된 플러그인을 찾아 SSH 접속과 CLI 명령 전송을 처리한다.
- 벤더 모듈도 같다. `cisco.ios`, `junipernetworks.junos`, `arista.eos`
  컬렉션을 설치해 쓴다.

### 실행하면 일어나는 일

```bash
ansible-playbook -i inventory/hosts.ini playbooks/backup.yml
```

1. 인벤토리를 읽어 장비 목록과 그룹을 만든다.
2. playbook의 `hosts: apac`로 대상 장비를 고른다.
3. `group_vars`, `host_vars`에서 각 장비의 변수를 모은다.
4. `ansible_connection`에 적힌 방식(`network_cli`)으로 장비마다 SSH 접속한다.
5. `ansible_network_os`에 맞는 모듈로 task를 차례로 실행한다.
6. 장비별 성공·실패 결과를 출력한다.

## 구조와 동작 방식 (실제 ansible의 사용)

핵심은 벤더 문법과 설정 로직의 분리다.
정책은 YAML 변수로 정의하고, OS별 Jinja 템플릿이
Cisco·Juniper·Arista 문법으로 바꾼다.

### 야간 강제(Nightly Enforcement) 흐름

```
[1. 백업] ──────▶ [2. golden config 생성] ──────▶ [3. 야간 배포]
    │                     │                         ▲
    ▼                     ▼                         │ 실패 시 중단
[GitHub 비공개 저장소: 백업 이력 + golden config]    └─▶ [배포 중단 + 외부 알림]
```

1. 백업
   - 리전 전체 running config를 playbook으로 백업
     - GitHub 비공개 저장소에 푸시해 변경분(delta)과 이력 추적
2. golden config 생성
   - 각 장비의 facts 수집
     - 인벤토리 그룹·변수를 OS별 Jinja 템플릿에 주입해 렌더링
     - 24시간마다 생성
     - Config Ansible Role이 민감정보 마스킹·포맷팅 후 GitHub에 저장
3. 야간 배포
   - golden config를 장비에 적용
     - Config Role이 저장 시 처리의 역처리를 해서 배포용으로 변환
     - 백업 또는 생성이 실패하면 배포를 중단하고 외부 알림 발송

### 1단계 백업 상세

#### 용어

- **running config**: 장비가 지금 메모리에서 쓰고 있는 설정.
  CLI로 바꾼 내용이 즉시 반영된다.
  (재부팅 시 읽는 설정은 startup config로 따로 있다)
- **리전 전체**: 인벤토리의 리전 그룹(예: `[apac]`)에 속한 장비 전체.
  `hosts: apac`이면 APAC 그룹의 모든 장비가 대상이다.
  (인벤토리·그룹·`hosts`는 "Ansible 기초" 참고)

#### 진행 순서

1. **대상 목록 확정**
   - 인벤토리에서 해당 리전 그룹의 장비 목록을 읽는다.
2. **장비 접속**
   - 장비마다 SSH로 접속한다 (`network_cli`).
     - 계정 정보는 Ansible Vault 등으로 암호화해 보관한다.
3. **running config 수집**
   - 벤더별 모듈이 장비에서 설정을 받아 온다.
     - 내부적으로 Cisco·Arista는 `show running-config`,
       Juniper는 `show configuration`을 실행한다.
4. **파일 저장**
   - 장비 1대당 파일 1개로 저장한다. 예: `backup/apac/sel-sw01.cfg`
     - 파일명을 고정해야 매일 같은 파일을 덮어쓰고, Git이 차이를 계산할 수 있다.
5. **정리(정규화)**
   - 매번 바뀌는 행을 지운다.
     예: `! Last configuration change at 02:00:13 KST ...`
     - 이런 행을 남기면 실제 변경이 없어도 매일 diff가 생긴다.
     - 비밀번호 해시 같은 민감 정보는 마스킹한다.
6. **GitHub 커밋·푸시**
   - 리전 단위로 커밋한다. 예: `backup(apac): 2026-09-28 nightly`
7. **결과 판정**
   - 한 대라도 접속이나 수집에 실패하면 백업 단계를 실패로 처리한다.
     - 이후 생성·배포를 하지 않고 외부 알림을 보낸다.

```yaml
# playbooks/backup.yml
- hosts: apac
  gather_facts: no
  tasks:
    - name: Cisco IOS 백업
      cisco.ios.ios_config:
        backup: yes
        backup_options:
          dir_path: "backup/apac"
          filename: "{{ inventory_hostname }}.cfg"
      when: ansible_network_os == "cisco.ios.ios"

    - name: Juniper 백업
      junipernetworks.junos.junos_config:
        backup: yes
        backup_options:
          dir_path: "backup/apac"
          filename: "{{ inventory_hostname }}.cfg"
      when: ansible_network_os == "junipernetworks.junos.junos"

    - name: Arista 백업
      arista.eos.eos_config:
        backup: yes
        backup_options:
          dir_path: "backup/apac"
          filename: "{{ inventory_hostname }}.cfg"
      when: ansible_network_os == "arista.eos.eos"

- hosts: localhost
  tasks:
    - name: 타임스탬프 행 제거
      ansible.builtin.shell: >
        sed -i '/^! Last configuration change/d' backup/apac/*.cfg

    - name: GitHub 커밋·푸시
      ansible.builtin.shell: |
        git add backup/apac
        git commit -m "backup(apac): {{ lookup('pipe', 'date +%F') }} nightly" || true
        git push
```

#### 결과물 예시

낮에 누군가 sel-sw01에 route를 직접 추가했다면 커밋 diff에 이렇게 보인다.

```diff
--- a/backup/apac/sel-sw01.cfg
+++ b/backup/apac/sel-sw01.cfg
@@ -412,6 +412,7 @@
 ip route 0.0.0.0 0.0.0.0 10.20.0.1
+ip route 10.9.9.0 255.255.255.0 10.20.0.254
 !
```

### 2단계 facts 수집 상세

#### facts란

Ansible이 장비에 접속해 자동으로 읽어 오는 장비 자체 정보다.
사람이 YAML에 적는 값이 아니다.

```yaml
- hosts: apac
  tasks:
    - cisco.ios.ios_facts:
        gather_subset: all
```

내부적으로 `show version`, `show interfaces` 등을 실행해 변수로 만든다.

```yaml
ansible_net_hostname: sel-sw01
ansible_net_model: C9300-48P
ansible_net_version: 17.9.4
ansible_net_serialnum: FOC2233X0AB
ansible_net_interfaces:
  GigabitEthernet1/0/1: { ... }
  GigabitEthernet1/0/48: { ... }
```

- YAML: 무엇을 설정할지 (정책)
- facts: 이 장비가 어떻게 생겼는지 (모델, OS 버전, 포트)
- 둘을 합쳐야 장비별 golden config가 나온다.

#### 왜 OS 버전이 필요한가

설정 파일의 문법이 OS와 버전에 따라 다르다.
같은 설정이라도 장비마다 써야 하는 명령어가 다르다.

예: RADIUS 서버 10.0.0.10 등록

Cisco 구버전 IOS 문법

```
radius-server host 10.0.0.10 auth-port 1812 acct-port 1813 key XXXX
```

Cisco IOS-XE 신버전 문법

```
radius server RAD-1
 address ipv4 10.0.0.10 auth-port 1812 acct-port 1813
 key XXXX
```

- 신버전에서는 구버전 문법이 deprecated라 경고가 나오거나 동작이 다를 수 있다.
- 구버전 장비는 신버전 문법을 모르므로 명령을 거부한다.

그래서 템플릿이 버전을 보고 문법을 고른다.

```jinja
{% if ansible_net_version is version('16.0', '>=') %}
radius server RAD-1
 address ipv4 {{ s }} auth-port 1812 acct-port 1813
{% else %}
radius-server host {{ s }} auth-port 1812 acct-port 1813
{% endif %}
```

#### 왜 모델(포트 목록)이 필요한가

- 48포트 장비는 인터페이스 설정이 48개, 24포트 장비는 24개다.
- 인터페이스 이름도 다르다. Cisco `GigabitEthernet1/0/1`, Juniper `ge-0/0/1`
- 모델을 모르면 설정 파일에 넣을 인터페이스 목록을 만들 수 없다.
- 리전·오피스 규모마다 장비가 다르다.
  (인도와 멕시코의 스위치가 다름, 대형 오피스는 별도 장비)

```jinja
{% for name in ansible_net_interfaces if name.startswith('GigabitEthernet') %}
interface {{ name }}
 switchport access vlan {{ user_vlan }}
{% endfor %}
```

`user_vlan`은 YAML의 정책이고, 인터페이스 목록은 facts에서 온다.

#### 잘못 알고 만들면

3단계 배포는 장비 설정을 golden config로 덮어쓴다.

- 모르는 문법이나 없는 인터페이스를 넣으면 명령이 거부되어 배포가 실패한다.
- 일부 명령만 적용되어 설정이 중간 상태로 남는다.
- 있는 포트가 golden config에서 빠지면 그 포트 설정이 지워진다.
- 인증·라우팅 설정이 빠지면 장비 접속이나 통신이 끊길 수 있다.

#### 왜 YAML에 적지 않고 매번 수집하나

- 5,000대의 모델·버전·포트를 사람이 YAML로 관리해야 한다.
- 장비 교체, 모듈 추가, OS 업그레이드마다 고쳐야 하고, 빠뜨리면 실제와 달라진다.
- facts는 매일 밤 장비에서 새로 읽으므로 항상 현재 상태다.

{% endraw %}
