---
layout: default
title: "3. TodoApp 제작기 2"
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

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/FixBug-Refresh)

🧸 현재 웹페이지에 **개선사항**이 몇 가지 있다.<br>
🧸 Todo를 추가/삭제하면 **즉각 반영**이 안됨.<br>
🧸 대략 설명하면 **props**를 활용할 예정<br>
🧸 일단 **App**아래의 **todoItems**에서 데이터를 관리한다에 주목해야한다.

```html
<!-- App.vue -->

<template>
  <div id="app">
    <TodoHeader></TodoHeader>
    <!-- addItem라는 이벤트가 온다면 addOneItem로 연결 -->
    <TodoInput v-on:addItem="addOneItem"></TodoInput>
    <!-- todoItems를  propsdata로 내린다. -->
    <TodoList v-bind:propsdata="todoItems" v-on:removeItem="removeOneItem" v-on:toggleItem="toggleOneItem"></TodoList>
    <!-- 만약 pros를 하나더 보내고자 한다면 -->
    <!-- <TodoList v-bind:{받는쪽 변수}="{보내는 쪽 변수}" -->
    <!-- <TodoList v-bind:newProps="newVar" -->
    <TodoFooter v-on:clearAll="clearAllItems"></TodoFooter>
  </div>
</template>

<script>
import TodoHeader from './components/TodoHeader.vue'
import TodoInput from './components/TodoInput.vue'
import TodoList from './components/TodoList.vue'
import TodoFooter from './components/TodoFooter.vue'

export default {
  data: function() {
    return {
      todoItems: []

      // 만약 변수를 하나 더 추가하고자 한다면
      // newVar: []
    }
  },
  methods: {
    addOneItem: function(todoItem) {
      var obj = {completed: false, item: todoItem};
      localStorage.setItem(todoItem, JSON.stringify(obj));
      this.todoItems.push(obj);
    },
    removeOneItem: function(todoItem, index) {
      this.todoItems.splice(index, 1);
      localStorage.removeItem(todoItem.item);
    },
    toggleOneItem: function(todoItem, index) {
      todoItem.completed = !todoItem.completed;
      localStorage.removeItem(todoItem.item);
      localStorage.setItem(todoItem.item, JSON.stringify(todoItem));
    },
    clearAllItems: function() {
      this.todoItems = [];
      localStorage.clear();
    }
  },
  created: function() {
    if (localStorage.length > 0) {
      for (var i = 0; i < localStorage.length; i++) {
        if (localStorage.key(i) !== 'loglevel:webpack-dev-server') {
          this.todoItems.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
        }
      }
    }
  },
  components: {
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

```html
<template>
  <section>
    <ul>
      <li v-for="(todoItem, index) in propsdata1" class="shadow" v-bind:key="todoItem.item">
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
  //props: ['propsdata'],
  props: {
    propsdata: {
      type: Array,
      default: function() {
        return [];
      }
    },
    // props를 하나 더 쓰고자 한다면
    newProps: {
      type: Array,
      default: function() {
        return [];
      }
    }
  },
  methods: {
    removeTodo: function(todoItem, index) {
      this.$emit('removeItem', todoItem, index);
    },
    toggleComplete: function(todoItem, index) {
      this.$emit('toggleItem', todoItem, index); 
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

{% endraw %}