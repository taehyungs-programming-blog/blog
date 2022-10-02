---
layout: default
title: "03. MySQL with Python"
parent: "(MySQL 💾)"
grand_parent: "Python"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---


```py
# DB Connect
import pymysql

# 에러없이 넘어간다면 정상동작.
db = pymysql.connect(host='localhost', port=3306, user='root', passwd='root', db='dave', charset='utf8')

db
```

---

## TABLE 생성

* [Get Code 🌎](https://github.com/kthCodeLog/pymysql-tutorial/tree/2)

```py
import pymysql

db = pymysql.connect(host='localhost', port=3306, user='root', passwd='root', db='dave', charset='utf8')

ecommerce = db.cursor()
sql = """
    CREATE TABLE product (
        PRODUCT_CODE VARCHAR(20) NOT NULL,
        TITLE VARCHAR(200) NOT NULL,
        ORI_PRICE INT,
        DISCOUNT_PRICE INT,
        DISCOUNT_PERCENT INT,
        DELIVERY VARCHAR(2),
        PRIMARY KEY(PRODUCT_CODE)
    );
"""

ecommerce.execute(sql)
db.commit()
db.close()
```

```sql
USE dave;
SELECT * FROM product;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-3-1.png"/>
</p>

---

## 데이터 넣기

* [Get Code 🌎](https://github.com/kthCodeLog/pymysql-tutorial/tree/3)

```py
import pymysql

db = pymysql.connect(host='localhost', port=3306, user='root', passwd='root', db='dave', charset='utf8')

cursor = db.cursor()

for index in range(10):
    product_code = 215673140 + index + 1
    sql = """INSERT INTO product VALUES(
    '""" + str(product_code) + """', '스위트바니 여름신상5900원~롱원피스티셔츠/긴팔/반팔', 23000, 6900, 70, 'F'); """
    print (sql)
    cursor.execute(sql)

db.commit()
db.close()
```

```sql
USE dave;
SELECT * FROM product;
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/python/mysql/mysql-3-2.png"/>
</p>

---

## 데이터 조회

* [Get Code 🌎](https://github.com/kthCodeLog/pymysql-tutorial/tree/4)

```py
import pymysql
db = pymysql.connect(host='localhost', port=3306, user='root', passwd='root', db='dave', charset='utf8')

cursor = db.cursor()
sql = "SELECT * FROM product"
cursor.execute(sql)
result = cursor.fetchone()
print(result)
db.close()
```

---

## UPDATE

```py
# 3. 커서 가져오기
cursor = db.cursor()

# 4. SQL 구문 만들기
SQL = """
UPDATE product SET 
    TITLE='달리샵린넨원피스 뷔스티에 썸머 가디건 코디전', 
    ORI_PRICE=33000, 
    DISCOUNT_PRICE=9900, 
    DISCOUNT_PERCENT=70 
    WHERE PRODUCT_CODE='215673141'
"""

# 5. SQL 구문 실행하기
cursor.execute(SQL)

# 6. commit 하기
db.commit()

# 7. close 하기
db.close()
```

---

## DELETE

```py
cursor = db.cursor()

SQL = """DELETE FROM product WHERE PRODUCT_CODE='215673142'"""

cursor.execute(SQL)

db.commit()

db.close()
```