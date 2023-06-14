---
title: "(C++) Define문에서 ##"
date: 2020-01-22 00:00:00 -0000
---

> `##` is called token concatenation(연속), used to concatenate two tokens in a macro invocation(주문).

```cpp
#define ArgArg(x, y)          x##y
#define ArgText(x)            x##TEXT
#define TextArg(x)            TEXT##x
#define TextText              TEXT##text
#define Jitter                1
#define bug                   2
#define Jitterbug             3
```

> output

```
ArgArg(lady, bug)   "ladybug"
ArgText(con)    "conTEXT"
TextArg(book)   "TEXTbook"
TextText    "TEXTtext"
ArgArg(Jitter, bug)     3
```

---

* [참고사이트1](https://stackoverflow.com/questions/6503586/what-does-in-a-define-mean)
* [참고사이트2](https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/numnum.htm)