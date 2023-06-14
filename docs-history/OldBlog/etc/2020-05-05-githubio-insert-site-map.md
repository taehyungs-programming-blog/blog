---
title: "Github io : sitemap 넣기"
permalink: github-io/insert-sitemap/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-05 00:00:00 -0000
last_modified_at: 2020-05-05 00:00:00 -0000
sidebar:
  title: "etc"
  nav: etc
excerpt: ""
tag:
  - Github io
category:
  - sitemap
header:
  teaser: /file/image/etc-page-teaser.gif
---

* [참고사이트](http://jinyongjeong.github.io/2017/01/13/blog_make_searched/)

아마도 이것을 안넣으면 Google Search Console에서 색인지정이 안되는 것으로 보인다.;;; 매우 심각;;

---

/root 밑에 sitemap.xml을 만들어 아래를 넣는다.<br>
아래 Example은 changefreq = weekly, priority = 0.5로 되어 있는데 나는 daily, 1로 변경함.

```xml
---
layout: null
---
<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.sitemaps.org/schemas/sitemap/0.9 http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd" xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
  {% for post in site.posts %}
    <url>
      <loc>{{ site.url }}{{ post.url }}</loc>
      {% if post.lastmod == null %}
        <lastmod>{{ post.date | date_to_xmlschema }}</lastmod>
      {% else %}
        <lastmod>{{ post.lastmod | date_to_xmlschema }}</lastmod>
      {% endif %}

      {% if post.sitemap.changefreq == null %}
        <changefreq>weekly</changefreq>
      {% else %}
        <changefreq>{{ post.sitemap.changefreq }}</changefreq>
      {% endif %}

      {% if post.sitemap.priority == null %}
          <priority>0.5</priority>
      {% else %}
        <priority>{{ post.sitemap.priority }}</priority>
      {% endif %}

    </url>
  {% endfor %}
</urlset>
```

---

## 만약 나처럼 제외된 색인이 너무 많아 블로그 전체를 새로 크롤리하고 싶다면??

실수로 sitemap.xml을 넣지 않아 이런 문제가 발생했다.. 난감...

![](/file/image/insert-site-map.png)

사이트 맵을 새로 신청하면 된다.

/root밑에 sitemap_new.xml을 만들고 위 코드를 그대로 복사이후 다시 사이트맵을 추가한다.

![](/file/image/insert-site-map1.png)