---
layout: default
title: "05. [JAVA8] date/time"
parent: "(JAVA)"
grand_parent: "Spring 🐍"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```java
// Instant
Instant instant = Instant.now();
System.out.println(instant);    // UTC 기준
ZonedDateTime z = instant.atZone(ZoneId.systemDefault());
System.out.println(z);          // 시스템 기준

// LocalDateTime
LocalDateTime now = LocalDateTime.now();
System.out.println(now);
ZonedDateTime nowInKorea = ZonedDateTime.now(ZoneId.of("Asia/Seoul"));
System.out.println(nowInKorea);

// Period
LocalDate today = LocalDate.now();
LocalDate thisYearBirthday = LocalDate.of(2020, Month.APRIL, 15);
Period period = Period.between(today, thisYearBirthday);
System.out.println(period.getDays());
Period period2 = today.until(thisYearBirthday);
System.out.println(period2.getDays());

// Duration
Instant n = Instant.now();
Instant plus = n.plus(10, ChronoUnit.SECONDS);
Duration between = Duration.between(n, plus);
System.out.println(between.getSeconds());

// format
LocalDateTime nn = LocalDateTime.now();
System.out.println(nn.format(DateTimeFormatter.ofPattern("MM/dd/YYYY")));

LocalDate parse = LocalDate.parse("07/15/1982", DateTimeFormatter.ofPattern("MM/dd/YYYY"));
System.out.println(parse);
```