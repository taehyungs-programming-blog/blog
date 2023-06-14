---
title: "(C++) override"
permalink: cpp/override/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-06 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - override
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
class dog {
    virtual void A(int)
    virtual void B() const;
    void C();
};

class yellowdog : public dog{
    virtual void A(float) override;     // Error
    virtual void B() override;          // Error
    void C() override;                  // Error - no function to override
}
```