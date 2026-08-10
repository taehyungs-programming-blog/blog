---
layout: default
title: "numpy, pandas 기본"
parent: (numpy)
grand_parent: Python
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/pythonExamples/tree/master/pythonExamples)

🐶 너무 기본적인것은 다루지 않음. 내가 헷깔리는 것 위주로 정리 됨.

## numpy 

### array

```py
import numpy as np

"""
numpy array example
"""

arr = np.array([10, 20, 30, 40, 50, 60, 70, 80, 90, 100])

arr1 = np.arange(8)
print(arr1)
# [0 1 2 3 4 5 6 7]

arr2 = np.arange(8).reshape(4, 2)
print(arr2)
"""
[[0 1]
 [2 3]
 [4 5]
 [6 7]]
"""
```

---

### Condition

```py
import numpy as np

'''
numpy condition example
'''

arr = np.array([10, 20, 30, 40, 50, 60, 70, 80, 90, 100])

# one condition
print(arr[ arr > 10 ])
# [ 20  30  40  50  60  70  80  90 100]

# two conditions
print(arr[( arr > 10 ) & ( arr < 30 ) ])
# [20]

# where사용해 코드 간결화
arr2 = np.where(arr > 40, 99, -99)  # np.where(조건문, true, false)
print(arr2)
# [-99 -99 -99 -99  99  99  99  99  99  99]

arr3 = np.where(arr > 40, arr, -1)
print(arr3)
# [ -1  -1  -1  -1  50  60  70  80  90 100]
```

---

## pandas

### Series

```py
'''
Series만들기
'''
data = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
s = Series(data)
print(s)
"""
0     10
1     20
2     30
3     40
4     50
5     60
6     70
7     80
8     90
9    100
dtype: int64
"""
```

```py
'''
인덱스 선언
'''
data = [10, 20, 30]
index = ["대한", "민국", "만세"]
s = Series(data, index)
# s = Series(data=data, index=index) # 명시적 선언도 가능.
print(s)
"""
대한    10
민국    20
만세    30
dtype: int64
"""
```

```py
'''
인덱스로 접근
'''
print(s.iloc[0])        # 인덱스 번호로 호출가능
print(s.loc["대한"])    # 인덱스로 호출가능
# 10
```

```py
'''
연산
'''
철수 = Series([10, 20, 30], index=['NAVER', 'SKT', 'KT'])
영희 = Series([10, 20, 30], index=['KT', 'SKT', 'NAVER'])
가족 = 철수 + 영희
print(가족)
"""
KT       40
NAVER    40
SKT      40
dtype: int64
"""
```

```py
"""
최대/최소 출력
"""
date = ["6/1", "6/2", "6/3", "6/4", "6/5"]
value = [42800, 42700, 42050, 42950, 43000]
s = Series(value, date)
print(s.max())      # 43000
print(s.idxmax())   # 6/5
```

```py
"""
map 활용
"""
def remove_comma(x):
    return int(x.replace(",", ""))

value = ["42,800", "42,700", "42,050", "42,950", "43,000"]
s = Series(value)
s = s.map(remove_comma)
print(s)
"""
0    42800
1    42700
2    42050
3    42950
4    43000
dtype: int64
"""
```

```py
"""
condition
"""
close = [42500, 42550, 41800, 42550, 42650]
open = [42600, 42200, 41850, 42550, 42500]
index = ['2019-05-31', '2019-05-30', '2019-05-29', '2019-05-28', '2019-05-27']
close = Series(data=close, index=index)
open = Series(data=open, index=index)
print(close[close>open])
"""
2019-05-30    42550
2019-05-27    42650
dtype: int64
"""
```

---

### DataFrame 1 - (기초, condition)

```py
"""
DataFrame 만들기
"""
data = {
    '종목코드' : ['037730', '036360', '005960'],
    '종목명' : ['3R', '3SOFT', 'ACTS'],
    '현재가' : [1510, 1790, 1185]
}
df = DataFrame(data)

"""
위와 같은 표현이다.
data = {
    ['037730', '3R', 1510],
    ['036360', '3SOFT', 1790],
    ['005960', 'ACTS', 1185]
}
columns = ['종목코드', '종목명', '현재가']
df = DataFrame(data=data, columns=columns)
"""

print(df)
"""
     종목코드    종목명   현재가
0  037730     3R  1510
1  036360  3SOFT  1790
2  005960   ACTS  1185
"""
```

```py
"""
index지정
"""
data = {
    '종목코드' : ['037730', '036360', '005960'],
    '종목명' : ['3R', '3SOFT', 'ACTS'],
    '현재가' : [1510, 1790, 1185]
}
df = DataFrame(data)
df = df.set_index('종목코드')
print(df)
"""
          종목명   현재가
종목코드
037730     3R  1510
036360  3SOFT  1790
005960   ACTS  1185
"""
```

```py
"""
Condition
"""
data = {
    '종목코드' : ['037730', '036360', '005960'],
    '종목명' : ['3R', '3SOFT', 'ACTS'],
    '현재가' : [1510, 1790, 1185]
}
df = DataFrame(data)
df = df.set_index('종목코드')
print(df.loc[df['현재가']>1600])
"""
          종목명   현재가
종목코드
036360  3SOFT  1790
"""
```

---

### DataFrame 2 - (query, groupby)

```py
"""
query
"""
data = [
    {"cd":"A060310", "nm":"3S", "open":2920, "close":2800},
    {"cd":"A095570", "nm":"AJ네트웍스", "open":1920, "close":1900},
    {"cd":"A006840", "nm":"AK홀딩스", "open":2020, "close":2010},
    {"cd":"A054620", "nm":"APS홀딩스", "open":3120, "close":3200}
]
df = DataFrame(data=data)
df = df.set_index('cd')
print(df.query("nm=='3S'"))
"""
         nm  open  close
cd
A060310  3S  2920   2800
"""

print(df.query("nm in ['3S', 'AK홀딩스']"))
"""
            nm  open  close
cd
A060310     3S  2920   2800
A006840  AK홀딩스  2020   2010
"""

name = "AK홀딩스"
print(df.query("nm == @name"))
"""
            nm  open  close
cd
A006840  AK홀딩스  2020   2010
"""
```

```py
"""
GroupBy
"""
data = [
    ["2차전지(생산)","SK이노베이션", 10.19, 1.29],
    ["해운","팬오션", 21.23, 0.95],
    ["시스템반도체","티엘아이", 35.97, 1.12],
    ["해운","HMM", 21.52, 3.2],
    ["시스템반도체","아이에이", 37.32, 3.55],
    ["2차전지(생산)","LG화학", 83.06, 3.75]
]

columns = ["테마", "종목명", "PER", "PBR"]
df = DataFrame(data=data, columns=columns)
print(df)
"""
         테마      종목명    PER   PBR
0  2차전지(생산)  SK이노베이션  10.19  1.29
1        해운      팬오션  21.23  0.95
2    시스템반도체     티엘아이  35.97  1.12
3        해운      HMM  21.52  3.20
4    시스템반도체     아이에이  37.32  3.55
5  2차전지(생산)     LG화학  83.06  3.75
"""

print(df.groupby("테마")["PER"].mean())
"""
테마
2차전지(생산)    46.625
시스템반도체      36.645
해운          21.375
Name: PER, dtype: float64
"""

temp = df[["테마", "PER", "PBR"]].groupby("테마").get_group("2차전지(생산)")
print(temp)
"""
         테마    PER   PBR
0  2차전지(생산)  10.19  1.29
5  2차전지(생산)  83.06  3.75
"""

temp = df.groupby("테마").agg({"PER":max, "PBR":min})
print(temp)
"""
            PER   PBR
테마
2차전지(생산)  83.06  1.29
시스템반도체    37.32  1.12
해운        21.52  0.95
"""

temp = df.groupby("테마").agg({"PER":[max, min], "PBR":[np.std, np.var]})
print(temp)
"""
            PER              PBR
            max    min       std      var
테마
2차전지(생산)  83.06  10.19  1.739483  3.02580
시스템반도체    37.32  35.97  1.718269  2.95245
해운        21.52  21.23  1.590990  2.53125
"""
```

---

### DataFrame 3 - (concat, merge)

```py
"""
concat
"""
data = {
    '종가': [113000, 111500],
    '거래량': [555850, 282163]
}
index = ['2019-06-21', '2019-06-20']
df1 = DataFrame(data=data, index=index)

data = {
    '시가': [112500, 110000],
    '고가': [111500, 112000],
    '저가': [111500, 109000]
}
index = ['2019-06-21', '2019-06-20']
df2 = DataFrame(data=data, index=index)

# index에 맞게 concat된다.
df = pd.concat([df1, df2], axis=1)  # 좌/우로 붙여달라
print(df)
"""
                종가     거래량      시가      고가      저가
2019-06-21  113000  555850  112500  111500  111500
2019-06-20  111500  282163  110000  112000  109000
"""

df = pd.concat([df1, df2], axis=0)  # 상/하로 붙여달라
print(df)
"""
                  종가       거래량        시가        고가        저가
2019-06-21  113000.0  555850.0       NaN       NaN       NaN
2019-06-20  111500.0  282163.0       NaN       NaN       NaN
2019-06-21       NaN       NaN  112500.0  111500.0  111500.0
2019-06-20       NaN       NaN  110000.0  112000.0  109000.0
"""
```

```py
data = [
    ["전기전자", "005930", "삼성전자", 74400],
    ["화학", "051910", "LG화학", 896000],
    ["전기전자", "000660", "SK하이닉스", 101500]
]

columns = ["업종", "종목코드", "종목명", "현재가"]
df1 = DataFrame(data=data, columns=columns)

data = [
    ["은행", 2.92],
    ["보험", 0.37],
    ["화학", 0.06],
    ["전기전자", -2.43]
]
columns = ["업종", "등락률"]
df2 = DataFrame(data=data, columns=columns)

df = pd.merge(left=df1, right=df2, on="업종")
print(df)
"""
     업종    종목코드     종목명     현재가   등락률
0  전기전자  005930    삼성전자   74400 -2.43
1  전기전자  000660  SK하이닉스  101500 -2.43
2    화학  051910    LG화학  896000  0.06
"""
```


