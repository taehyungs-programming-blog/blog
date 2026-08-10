---
layout: default
title: "23. Memento Pattern"
parent: (Desing Pattern)
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고사이트 🌎](https://devjino.tistory.com/45)

🍅 우선 가장 기본적인 Memnto의 구조를 보자.

```cpp
class BankAccount
{
    int balance = 0;
public:
    explicit BankAccount(const int balance)
        : balance(balance)
    {
    }

    Memento deposit(int amount)   // 예금 처리후 메멘토 객체를 반환
    {
        balance += amount;
        return { balance };
    }

    void restore(const Memento& m)
    {
        balance = m.balance;
    }
};

class Memento
{
    int balance;
public:
    Memento(int balance) : balance(balance) {}
    friend class BankAccount;
};
```

```cpp
BankAccount ba{ 100 };
auto m1 = ba.deposit(50); // 150
auto m2 = ba.deposit(25); // 175
 
// m1로 복구, 잔고 150
ba.restore(m1);
 
// m2로 복구, 잔고 175
ba.restore(m2);

// Mememto : 특정 상태를 기록해 둔다고 생각하면 편할 듯 하다.
```

--- 

🍅 좀 더 진화 시켜본다면

```cpp
class BankAccount2
{
    int balance = 0;
    vector<shared_ptr<Memento>> changes;
    int current;

public:
    explicit BankAccount2(const int balance)
        : balance(balance)
    {
        changes.emplace_back(make_shared<Memento>(balance));
        current = 0;
    }

    shared_ptr<Memento> deposit(int amount)
    {
        balance += amount;
        // Mememto에 상태를 담아두고 vector로 관리
        auto m = make_shared<Memento>(balance);
        changes.push_back(m);
        ++current;
        return m;
    }

    void restore(const shared_ptr<Memento>& m)
    {
        if (m)
        {
            balance = m->balance;
            changes.push_back(m);
            current = changes.size() - 1;
        }
    }

    shared_ptr<Memento> undo()
    {
        if (current > 0)
        {
            --current;
            auto m = changes[current];
            balance = m->balance;
            return m;
        }
        return{};
    }

    shared_ptr<Memento> redo()
    {
        if (current + 1 < changes.size())
        {
            ++current;
            auto m = changes[current];
            balance = m->balance;
            return m;
        }
        return{};
    }
};
```

```cpp
BankAccount2 ba{ 100 };
ba.deposit(50); // 150
ba.deposit(25); // 175
 
ba.undo();    // 150
ba.undo();    // 100
ba.redo();    // 150
ba.undo();    // 100
```