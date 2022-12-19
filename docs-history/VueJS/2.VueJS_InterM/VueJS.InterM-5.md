---
layout: default
title: "5. ES6"
parent: "(Vue.JS 중급)"
grand_parent: "Vue.JS"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

💎 **ES6**(ECMAScript2015) - JavaScript 신 문법<br>
💎 **Babel** - 구 버전 브라우저 중 ES6를 지원하지 않는 브라우저를 위해 나온 기술 ([Babel 링크 🌎](https://babeljs.io/))

---

## const & let

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/constlet)

💎 **const** - 상수형으로 변수 선언
💎 **let** - 변수긴 한데 이후에 다시 선언 불가한 변수

```js
var a = 10;     // 기존의 선언법

// const
const a = 10;
a = 20;         // Error

// 단, 객체나 배열은 변경가능
const a = {};
a.num = 10;
console.log(a);     // { num:10 }

const a = [];
a.push(20);
console.log(a);     // [20]

// let
let a = 10;
a = 20;         // Okay
let a = 30;     // Error
```

---

## (ES5) 변수의 Scope

```js
// ES5 기준 - {}에 상관없이 Scope가 설정됨
var sum = 0;
for (var i = 1; i <= 5; i++) {
    sum = sum + i;
}

console.log(sum);   // 15
console.log(i);   // 6
```

### (ES6) 변수의 Scope

```js
let sum = 0;
for (let i = 1; i <= 5; i++) {
    // let 선언시 Scope 외부에서 해제됨.
    sum = sum + i;
}

console.log(sum);   // 10
console.log(i);     // Error
```

---

## (ES5) Hoisting

💎 함수와 변수를 코드의 **최상단으로 올려준다.**

```js
function funcA() {
    return 10;
}

console.log(funcA());   // 5

function funcA() {
    return 5;
}
```

```js
var sum = 5;
sum = sum + i;  // 15 - i를 선언하지 않았는데 안다고 ???

// ..

var i = 10;     // <- Hoisting이 된다.
```

---

## 화살표 함수

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/arrowFunction)

```js
// ES5 기준 함수 선언
var sum = function(a, b) {
    return a + b;
};

// ES6 기준 함수 선언
var sum (a, b) => {
    return a + b;
};
```

```js
// Example

// ES5
var arr = ["a", "b", "c"];
arr.forEach(function(value) {
    console.log(value);
});

// ES6
var arr = ["a", "b", "c"];
arr.forEach(value => console.log(value));
```

💎 Vue에서 활용해 보자면

```html
<!-- ... --->

<script>
// ...

export default {
  data: function() {
    return {
      todoItems: []
    }
  },
  methods: {
    // addOneItem: function(todoItem) {
    addOneItem(todoItem) {
      const obj = {completed: false, item: todoItem};
      localStorage.setItem(todoItem, JSON.stringify(obj));
      this.todoItems.push(obj);
    },
```

💎 쉽게 말하면 **function을 생략**해도 된다.

---

## Enhanced Object Literals

```js
var dictionary = {
    words: 100,
    // ES5
    lookup: function() {
        console.log("find words");
    },

    // ES6
    lookup() {
        console.log("find words 6");
    }
};
```

---

## Modules

```js
// libs/math.js 에 아래와 같은 함수가 정의되어 있다고 가정하자
export function sum(x, y) {
    return x + y;
}

// main.js
import {sum} from 'libs/math.js'
sum(1, 2);
```

{% endraw %}