---
title: "(gitpage) google analytics 붙이기"
date: 2020-03-09 00:00:00 -0000
---

> 우선 [google analytics](https://analytics.google.com/) 에서 가입을 해야함.<br>
> 가입하면서 자신의 블로그 등록하는거 잊지말것!

![](/file/image/google_analytics_image_1.png)

> 속성 -> 속성 설정

![](/file/image/google_analytics_image_2.png)

> 추적 ID확인<br>
> _config.yml에 추적 ID를 추가한다.<br>
> (지킬에 따라 _config.yml을 다를 수 있다.)

```yml
# Analytics
analytics:
  provider               : "google"
  google:
    tracking_id          : "추적 ID"
    anonymize_ip         : # true, false (default)
```

> * [참고사이트](https://inasie.github.io/it%EC%9D%BC%EB%B0%98/1/)
