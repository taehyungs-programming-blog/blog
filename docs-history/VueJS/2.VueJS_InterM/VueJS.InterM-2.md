---
layout: default
title: "2. TodoApp 제작기 1"
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

## 컴포넌트 생성

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/Add_Component)

👑 Footer, Header, Input, List 컴포넌트를 사용할 예정인데 각 .vue파일을 생성하자.<br>
👑 App.vue에서 각 컴포넌트를 임포트 한다.

```vue
<template>
  <div id="app">
    <TodoHeader></TodoHeader>
    <TodoInput></TodoInput>
    <TodoList></TodoList>
    <TodoFooter></TodoFooter>
  </div>
</template>

<script>
import TodoHeader from './components/TodoHeader.vue'
import TodoInput from './components/TodoInput.vue'
import TodoList from './components/TodoList.vue'
import TodoFooter from './components/TodoFooter.vue'

export default {
  components: {
      // TodoHeader에 TodoHeader를 담아달라
    TodoHeader: TodoHeader,
    TodoInput: TodoInput,
    TodoList: TodoList,
    TodoFooter: TodoFooter
  }  
}
</script>

<style>
body {
  text-align: center;
  background-color: #F6F6F8;
}
input {
  border-style: groove;
  width: 200px;
}
button {
  border-style: groove;
}
.shadow {
  box-shadow: 5px 10px 10px rgba(0, 0, 0, 0.03)
}
</style>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-2-1.png"/>
</p>

---

## 폰트 및 아이콘 사용법

```html
<!-- public/index.html -->
<!DOCTYPE html>
<html lang="">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">

    <!-- viewport : 반응형 웹 기기에 따라 크기 자동 변경 선언 -->
    <meta name="viewport" content="width=device-width,initial-scale=1.0">

    <!-- 파비콘 선언 -->
    <link rel="icon" href="<%= BASE_URL %>favicon.ico">

    <!-- 구글폰트 사용 -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Ubuntu:ital,wght@0,300;0,400;0,500;0,700;1,300;1,400;1,500;1,700&display=swap" rel="stylesheet">
    
    <!-- awesome icon 사용 -->
    <script src="https://kit.fontawesome.com/e6c10e11b1.js" crossorigin="anonymous"></script>

    <title><%= htmlWebpackPlugin.options.title %></title>
  </head>

  <!-- ... -->
```

👑 [구글 폰트](https://fonts.google.com/specimen/Ubuntu?query=ubuntu&preview.text=TODO%20it!&preview.text_type=custom)사용하기<br>
👑 사용할 폰트 클릭 후 링크 사용

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-2-2.png"/>
</p>

👑 [fontawesome](https://fontawesome.com/) 사용하기<br>
👑 로그 인 후 Settings에서 스크립트 복사하여 사용

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-2-3.png"/>
</p>

---

## 컴포넌트 내부 구현

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/EditComponent)

👑 몰랐던 내용 위주로 정리.

```html
<!-- TodoHeader.vue -->

<template>
  <header>
    <h1>TODO it!</h1>
  </header>
</template>

<!-- scoped : 해당 코드내에서만 반영해 달라 -->
<style scoped>
h1 {
  color: #2F3B52;
  font-family: 'Ubuntu', sans-serif;
  font-weight: 500;
  margin: 2.5rem 0 1.5rem;
}
</style>
```

```html
<!-- TodoInput.vue -->

<template>
  <div class="inputBox shadow">
      <!-- input의 text를 newTodoItem으로 받는다 -->
    <input type="text" v-model="newTodoItem" v-on:keyup.enter="addTodo">
    <span class="addContainer" v-on:click="addTodo">
      <i class="addBtn fas fa-plus" aria-hidden="true"></i>
    </span>
  </div>
</template>

<script>
export default {
  data: function() {
    return {
        // newTodoItem를 변수로 쓰겠다고 알린다.
      newTodoItem: ''
      // 만약 변수를 더 쓰고 싶다면 밑에 추가하면 됨.
    }
  },
  methods: {
    addTodo: function() {
      if (this.newTodoItem !== '') {
        var obj = {completed: false, item: this.newTodoItem};
        // localStorage.setItem는 key, value 형태로 저장됨
        localStorage.setItem(this.newTodoItem, JSON.stringify(obj));
        this.clearInput();
      }
    },
    clearInput: function() {
      this.newTodoItem = '';
    }
  }
}
</script>

<style scoped>
input:focus {
  outline: none;
}
.inputBox {
  background: white;
  height: 50px;
  line-height: 50px;
  border-radius: 5px;
}
.inputBox input {
  border-style: none;
  font-size: 0.9rem;
}
.addContainer {
  float: right;
  background: linear-gradient(to right, #6478FB, #8763FB);
  display: block;
  width: 3rem;
  border-radius: 0 5px 5px 0;
}
.addBtn {
  color: white;
  vertical-align: middle;
}
</style>
```

👑 local storage 확인법

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-2-5.png"/>
</p>

```html
<!-- TodoList.vue -->

<template>
  <section>
    <ul>
      <li v-for="(todoItem, index) in todoItems" class="shadow" v-bind:key="todoItem.item">
        <span class="checkBtn" v-on:click="toggleComplete(todoItem, index)">
            <i class="checkBtn fas fa-check" v-bind:class="{checkBtnCompleted: todoItem.completed}"></i>
        </span>
        <span v-bind:class="{textCompleted: todoItem.completed}">{{ todoItem.item }}</span>
        <span class="removeBtn" v-on:click="removeTodo(todoItem, index)">
          <i class="removeBtn fas fa-trash-alt"></i>
        </span>
      </li>
    </ul>
  </section>
</template>

<script>
export default {
  data: function() {
    return {
      todoItems: []
    }
  },
  methods: {
    removeTodo: function(todoItem, index) {
      this.todoItems.splice(index, 1);
      localStorage.removeItem(todoItem);
    },
    toggleComplete: function(todoItem, index) {
      todoItem.completed = !todoItem.completed;
      localStorage.removeItem(todoItem.item);
      localStorage.setItem(todoItem.item, JSON.stringify(todoItem));
    }
  },
  created: function() {
      // 생성시 로컬스토리지에서 데이터를 읽어온다
    if (localStorage.length > 0) {
      for (var i = 0; i < localStorage.length; i++) {
          // 안쓰는 데이터의 경우 아래와 같이 예외처리하자
        if (localStorage.key(i) !== 'loglevel:webpack-dev-server') {
          this.todoItems.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
        }
      }
    }
  }
}
</script>

<style scoped>
ul {
  list-style-type: none;
  padding-left: 0px;
  margin-top: 0;
  text-align: left;
}
li {
  display: flex;
  min-height: 50px;
  height: 50px;
  line-height: 50px;
  margin: 0.5rem 0;
  padding: 0 0.9rem;
  background: white;
  border-radius: 5px;
}
.checkBtn {
  line-height: 45px;
  /* color: black; */
  color: #62acde;
  margin-right: 5px;
}
.checkBtnCompleted {
  /* color: #62acde; */
  color: black;
}
.textCompleted {
  text-decoration: line-through;
}
.removeBtn {
  margin-left: auto;
  color: #de4343;
}
</style>

```

```html
<!-- TodoFooter.vue -->

<template>
  <div class="clearAllContainer">
    <span class="clearAllBtn" v-on:click="clearTodo">Clear All</span>
  </div>
</template>

<script>
export default {
  methods: {
    clearTodo: function() {
      localStorage.clear();
    }
  }
}
</script>

<style scoped>
.clearAllContainer {
  width: 8.5rem;
  height: 50px;
  line-height: 50px;
  background-color: white;
  border-radius: 5px;
  margin: 0 auto;
}
.clearAllBtn {
  color: #e20303;
  /* 추가 */
  display: block;
}
</style>

```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-2-4.png"/>
</p>

{% endraw %}
