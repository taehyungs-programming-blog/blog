---
layout: default
title: "25. Command Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Command Pattern

* [참고 사이트 🌍](https://devjino.tistory.com/42)

* 커맨드 패턴은 어떤 객체를 활용할 때 직접 **그 객체의 API를 호출하여 조작하는 대신, 작업을 어떻게 하라고 명령을 보내는 방식**입니다.
* 코드를 보면알겠지만 Action의 Timing을 뒤로 미루는 것이 핵심이다.

---

## Example (Bank Account)

```cpp
struct BankAccount
{
    int balance = 0;
    int overdraft_limit = -500;

    void deposit(int amount)
    {
        balance  = amount;
        cout << "deposited " << amount << ", balance now " <<
        balance << "\n";
    }

    void withdraw(int amount)
    {
        if (balance - amount >= overdraft_limit)
        {
            balance -= amount;
            cout << "withdrew " << amount << ", balance now " <<
            balance << "\n";
        }
    }
}
```

* 갑자기 몇 가지 기능추가가 필요하다고 한다.
    * Undo
    * 거래내역 기록

```cpp
struct Command
{
    virtual void call() const = 0;
    virtual void undo() const = 0;
};
struct BankAccountCommand : Command
{
    BankAccount& account;
    enum Action { deposit, withdraw } action;
    int amount;

    BankAccountCommand(BankAccount& account, const Action action, const int amount)
    : account(account), action(action), amount(amount) {}

    void call() const override
    {
        // LOG ~~ 여기서 거래내역 기록
        switch (action)
        {
            case deposit:
                account.deposit(amount);
                break;
            case withdraw:
                account.withdraw(amount);
                break;
            default: break;
        }
    }

    void undo() const override
    {
        switch (action)
        {
            case withdraw:
                account.deposit(amount);
                break;
            case deposit:
                account.withdraw(amount);
                break;
            default: break;
        }
    }
};
```

```cpp
BankAccount ba;
BankAccountCommand cmd{ ba, BankAccountCommand::deposit, 100 };
cmd.call();
```

---

## Composite Pattern으로도 해결이 가능

```cpp
struct CompositeBankAccountCommand
  : vector<BankAccountCommand>, Command
{
    CompositeBankAccountCommand(const initializer_list<value_type>& items)
    : vector<BankAccountCommand>(items) {}

    void call() const override
    {
        for (auto& cmd : *this)
            cmd.call();
    }

    void undo() const override
    {
        //for (auto& cmd : *this)
            //cmd.undo();
        for(auto it = rbegin(); it != rend();   it) // 역순으로 undo
            it->undo();
    }
};
```

```cpp
BankAccount ba;
CompositeBankAccountCommand commands{
    BankAccountCommand{ba, BankAccountCommand::deposit, 100},
    BankAccountCommand{ba, BankAccountCommand::withdraw, 200}
};
commands.call();
commands.undo();
```