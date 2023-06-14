---
title: "(git) 과거 commit 버전간 비교"
date: 2020-02-15 00:00:00 -0000
---

> 아래거 다 필요없음;;; sourcetree한 방에 처리가능!

![](/file/image/git-compare-past-commit_image_04.png)

> 비교를 원하는 commit 2개 선택시 바로 보여줌 ;;;

---
---

> 우선 git의 difftool을 beyond compare로 설정해야한다. [여기참고](https://8bitscoding.github.io/git-difftool/)

```s
$ git difftool <commit id> <commit id>
```

![](/file/image/git-compare-past-commit_image_01.png)

> 위 그림에서 E와 D의 commit을 비교하고 싶다면?

![](/file/image/git-compare-past-commit_image_02.png)

> 우선 각각의 commit id를 확인해야한다.

![](/file/image/git-compare-past-commit_image_03.png)

> 단, beyond compare를 사용할 경우 파일 하나하나 diff해서 나오기에 commit간 변경된 파일이 많을경우 보기가 힘들다.(하나하나 다 열리기때문에...)<br>
> 여기에 대한 해결책이 나오면 다시 작성하겠음.
