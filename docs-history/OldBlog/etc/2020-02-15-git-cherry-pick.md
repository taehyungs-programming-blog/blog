---
title: "(git) Sourcetree로 cherry pick하기"
date: 2020-02-15 00:00:00 -0000
---

> 체리픽은 특정 버전의 commit과 현재 코드를 비교할때 사용된다.<br>
> 역시 설명으론 이해가 되지 않으니 실사용 예를 보자.<br>

![](/file/image/git-cherry-pick_image_01.png)

> 내가 비교를 원하는 commit에서 우클릭 후 체리 픽을 선택한다.

![](/file/image/git-cherry-pick_image_02.png)

> 이런 경고창이 뜨는데 일단은 무시하고 확인~

![](/file/image/git-cherry-pick_image_03.png)

> 위 그림과 같이 체리픽이 되는데 ... 이렇게 비교하면 힘드니 Beyond Compare사용<br>
> 우 클릭 -> 충돌 해결 -> 외부 병합 툴 사용 클릭

![](/file/image/git-cherry-pick_image_04.png)

> Beyond Compare의 사용법까지는 설명하지 않겠다.<br>
> 이런식으로 과거 Commit과 비교가 가능하다는 점을 알아 두자.