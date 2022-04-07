---
layout: default
title: "4. Modal Alert, slot 사용하기, 트렌지션 넣기"
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

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/InsertModalAlert)

🏆 입력이 들어오지 않는다면 Alert 출력

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-4-1.png"/>
</p>

```html
<!-- TodoInput.vue -->

<template>
  <div class="inputBox shadow">
    <input type="text" v-model="newTodoItem">
    <span class="addContainer" v-on:click="addTodo">
      <i class="addBtn fas fa-plus" aria-hidden="true"></i>
    </span>

    <Modal v-if="showModal" @close="showModal = false">
      <template v-slot:header>
        <h3>경고</h3>
        <span class="closeModalBtn" @click="showModal = false">
            <i class="fa fa-times" aria-hidden="true"></i>
        </span>
      </template>
      <template v-slot:body>
          <p>할 일을 입력하세요.</p>
      </template>
    </Modal>
  </div>
</template>

<!-- ... -->
```

🏆 트렌지션 넣기

```html
<template>
  <section>
    <transition-group name="list" tag="ul">
      <li v-for="(todoItem, index) in propsdata" class="shadow" v-bind:key="todoItem.item">
        <span class="checkBtn" v-on:click="toggleComplete(todoItem, index)">
            <i class="checkBtn fas fa-check" v-bind:class="{checkBtnCompleted: todoItem.completed}"></i>
        </span>
        <span v-bind:class="{textCompleted: todoItem.completed}">{{ todoItem.item }}</span>
        <span class="removeBtn" v-on:click="removeTodo(todoItem, index)">
          <i class="removeBtn fas fa-trash-alt"></i>
        </span>
      </li>
    </transition-group>
  </section>
</template>

<script>
 // ...
</script>

<style scoped>
/* ... */

/* transition css */
.list-enter-active, .list-leave-active {
  transition: all 1s;
}
.list-enter-from, .list-leave-to {
  opacity: 0;
  transform: translateY(30px);
}
</style>
```

{% endraw %}