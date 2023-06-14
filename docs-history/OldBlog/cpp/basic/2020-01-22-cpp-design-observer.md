---
title: "(C++) Design Pattern - Observer"
date: 2020-01-22 00:00:00 -0000
---

> 간단예제와 고급예제로 구분

---

## 목차

* [간단예제](#간단예제)
* [고급예제](#고급예제)

---

## 간단예제

```cpp
// 옵저버 인터페이스
class Observer
{
public:
    virtual void update() = 0;
};
```

```cpp
// 옵저버 상속 클래스
class ConcreteObserver : public Observer
{
public:
    void update() override { cout << this << " : update" << endl; }
};
```

```cpp
// 옵저버 관리 및 통치 인터페이스(Subject)
class Subject
{
public:
    void attach(Observer* o) { mList.AddTail(o); }
    void detach(Observer* o) { mList.RemoveAt(mList.Find(o)); }
 
protected:
    void notify()
    {
        POSITION pos = mList.GetHeadPosition();
        while (pos != NULL)
            mList.GetNext(pos)->update();
    }
 
public:
    virtual void change() = 0;
 
private:
    CAtlList<Observer*> mList;
};
```

```cpp
// Subject 상속 클래스
class ConcreteSubject : public Subject
{
public:
    void change() override { notify(); }
};
```

```cpp
// 실제 사용
int _tmain(int argc, _TCHAR* argv[])
{
    Subject* pSubject = new ConcreteSubject;
 
    Observer* pObserver1 = new ConcreteObserver;
    Observer* pObserver2 = new ConcreteObserver;
    Observer* pObserver3 = new ConcreteObserver;
 
    pSubject->attach(pObserver1);
    pSubject->attach(pObserver2);
    pSubject->attach(pObserver3);
 
    pSubject->change();
 
    pSubject->detach(pObserver1);
    pSubject->detach(pObserver2);
    pSubject->detach(pObserver3);
 
    pSubject->change();
 
    delete pSubject;
    delete pObserver1;
    delete pObserver2;
    delete pObserver3;
 
    return 0;
}
```

---

## 고급예제

* Getting notification when things happen.
* An observer is ans object that wishes to be informed about events happening in the system.
* The entity generating the events is an observable.

* [Observer and Observable](#Observer-and-Observable)
* [Thread Safety and Reentrancy(재진입성)](#Thread-Safety-and-Reentrancy)
> * [Observer 전체코드](#Observer-전체코드)

---

### Observer and Observable

```cpp
class Person : public Observable<Person>
{
    int age;
public:
    int get_age() const
    {
        return age;
    }

    void set_age(int age)
    {
        if (this->age == age) return;
        this->age = age;
        notify(*this, "age");
    }
};

struct ConsolePersonObserver : public Observer<Person>
{
private:
    void field_changed(Person& source, const std::string& field_name) override
    {
        cout << "Person's " << field_name << " has changed to ";
        if(field_name == "age") cout << source.get_age();
        cout << "\n.";
    }
};

int main()
{
    Person person{10};
    ConsolePersonOberver cpo;
    person.subscribe(cpo);

    person.set_age(11);
    person.set_age(12);

    person.unsubscribe(cpo);
    person.set_age(13);

    return 0;
}
```

```cpp
// Observer
#pragma once
#include <string>

template <typename T>
class Observer
{
    virtual void field_changed(T& source, const std::string& field_name) = 0;
};
```

```cpp
// Observable
#pragma once
#include <string>
#include <vector>

template <typename> struct Observer;

template <typename T>
class Observable
{
    std::vector<Observer<T>*> observers;
public:
    void nofity(T& source, const std::string& field_name)
    {
        for(auto observer : observers)
        {
            observer->field_changed(source, field_name);
        }
    }

    void subscribe(Observer<T>& observer)
    {
        observers.push_back(&observer);
    }

    void unsubscribe(Observer<T>& observer)
    {
        observer.erase(
            remove(osbervers.begin(), observers.end(), oberver),
            observers.end()
        );
    }
};
```

---

### Thread Safety and Reentrancy

```cpp
#pragma once

#include <string>
#include <vector>
#include <mutex>

template <typename> struct Observer;

template <typename T>
struct SaferObservable
{
  std::vector<Observer<T>*> observers;
  typedef std::recursive_mutex mutex_t;
  mutex_t mtx;
public:
  void notify(T& source, const std::string& field_name)
  {
    std::scoped_lock<mutex_t> lock{mtx};
    for (auto observer : observers)
      if (observer)
        observer->field_changed(source, field_name);
  }

  void subscribe(Observer<T>& observer)
  {
    std::scoped_lock<mutex_t> lock{mtx};
    observers.push_back(&observer);
  }

  void unsubscribe(Observer<T>& observer)
  {
    auto it = std::find(begin(observers), end(observers), &observer);
    if (it != end(observers))
      *it = nullptr;
//    std::scoped_lock<mutex_t> lock{mtx};
//    observers.erase(
//      remove(observers.begin(), observers.end(), &observer),
//      observers.end()
//    );
  }
};
```

---

### Observer 전체코드

```cpp
// main.cpp
#include "Headers.hpp"
#include "Observer.hpp"
#include "SaferObservable.hpp"

class Person : public SaferObservable<Person>
{
  int age{0};
public:
  Person(){}
  Person(int age) : age(age) {}

  int get_age() const
  {
    return age;
  }

  void set_age(int age)
  {
    if (this->age == age) return;

    auto old_can_vote = get_can_vote();
    this->age = age;
    notify(*this, "age");

    if (old_can_vote != get_can_vote())
      notify(*this, "can_vote");
  }

  bool get_can_vote() const {
    return age >= 16;
  }
};

// observer & observable

struct ConsolePersonObserver
  : public Observer<Person>
{
private:
  void field_changed(Person &source, const std::string &field_name) override
  {
    cout << "Person's " << field_name << " has changed to ";
    if (field_name == "age") cout << source.get_age();
    if (field_name == "can_vote")
      cout << boolalpha << source.get_can_vote();
    cout << ".\n";
  }
};

struct TrafficAdministration : Observer<Person>
{
  void field_changed(Person &source, const std::string &field_name) override
  {
    if (field_name == "age")
    {
      if (source.get_age() < 17)
        cout << "Whoa there, you're not old enough to drive!\n";
      else
      {
        cout << "Oh, ok, we no longer care!\n";
        source.unsubscribe(*this);
      }
    }
  }
};

int main(int ac, char* av[])
{
  Person p;
  TrafficAdministration ta;
  p.subscribe(ta);

  p.set_age(15);
  p.set_age(16);
  try
  {
    p.set_age(17);
  }
  catch (const std::exception& e)
  {
    cout << "Oops, " << e.what() << "\n";
  }

  return 0;
}
```

```cpp
// Observable.hpp
#pragma once

#include <string>
#include <vector>

template <typename> struct Observer;

template <typename T>
struct Observable
{
  std::vector<Observer<T>*> observers;
public:
  void notify(T& source, const std::string& field_name)
  {
    for (auto observer : observers)
      observer->field_changed(source, field_name);
  }

  void subscribe(Observer<T>& observer)
  {
    observers.push_back(&observer);
  }

  void unsubscribe(Observer<T>& observer)
  {
    observers.erase(
      remove(observers.begin(), observers.end(), &observer),
      observers.end()
    );
  }
};
```

```cpp
// Observer.hpp
#pragma once
#include <string>

template <typename T>
struct Observer
{
  virtual void field_changed(
    T& source,
    const std::string& field_name
  ) = 0;
};
```

```cpp
// SaferObservable.hpp
#pragma once

#include <string>
#include <vector>
#include <mutex>

template <typename> struct Observer;

template <typename T>
struct SaferObservable
{
  std::vector<Observer<T>*> observers;
  typedef std::recursive_mutex mutex_t;
  mutex_t mtx;
public:
  void notify(T& source, const std::string& field_name)
  {
    std::scoped_lock<mutex_t> lock{mtx};
    for (auto observer : observers)
      if (observer)
        observer->field_changed(source, field_name);
  }

  void subscribe(Observer<T>& observer)
  {
    std::scoped_lock<mutex_t> lock{mtx};
    observers.push_back(&observer);
  }

  void unsubscribe(Observer<T>& observer)
  {
    auto it = std::find(begin(observers), end(observers), &observer);
    if (it != end(observers))
      *it = nullptr;
//    std::scoped_lock<mutex_t> lock{mtx};
//    observers.erase(
//      remove(observers.begin(), observers.end(), &observer),
//      observers.end()
//    );
  }
};
```