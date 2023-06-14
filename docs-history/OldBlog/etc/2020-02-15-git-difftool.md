---
title: "(git) git의 difftool 지정하기(beyond compare4)"
date: 2020-02-15 00:00:00 -0000
---

```s
$ git config --global diff.tool bc
$ git config --global difftool.bc.path "C:\Program Files\Beyond Compare 4\bcomp.exe"

# tool 이 뜰때 마다 나오는 message를 사용 안함
$ git config --global --add difftool.prompt false
$ git config --global --add mergetool.prompt false
```

---

* [참고사이트1](https://www.lesstif.com/pages/viewpage.action?pageId=17105535)
* [참고사이트2](http://www.scootersoftware.com/support.php?zz=kb_vcs#gitwindows) : beyond compare 공식사이트