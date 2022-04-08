---
layout: default
title: "7. Vuex 써보기"
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

## 사용해볼 기술 요소

* **state** - 여러 컴포넌트에 공유되는 데이터
* **getters** - 연산된 state에 접근
* **mutations** - state를 변경하는 이벤트/매서드
* **actions** - 비동기 처리 로직 선언 매서드

🦍 이렇게 설명하고 이해하면 천재다 코드를 참고하자

---

## state, getters

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/initVuex)

* **state** - 여러 컴포넌트에 공유되는 데이터
* **getters** - 연산된 state에 접근

```html
<template>
    <!-- 호출하기 (Vue) -->
    <p>{{ mssage }}</p>

    <!-- 호출하기 (Vuex) -->
    <p>{{ this.$store.state.message }}</p>
</template>

<script>
    // Vue
    data: {
        message: "1234"
    }

    // Vuex
    state: {
        message: "1234"
    }
</script>
```

🦍 실사용은 대충 코드를 보면 이해가 된다.

```js
import { createStore } from 'vuex'

const storage = {
	fetch() {
		const arr = [];
		if (localStorage.length > 0) {
      for (let i = 0; i < localStorage.length; i++) {
        if (localStorage.key(i) !== 'loglevel:webpack-dev-server') {
          arr.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
        }
      }
    }
		return arr;
	}
}

export const store = createStore({
	state: {
		todoItems: storage.fetch()
	},
	getters: {
		getTodoItems(state) {
			return state.todoItems;
		}
	}
});

```

```html
<template>
  <section>
    <transition-group name="list" tag="ul">
      <li v-for="(todoItem, index) in this.storedTodoItems" class="shadow" v-bind:key="todoItem.item">
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
export default {
  props: ['propsdata'],
  methods: {
    removeTodo(todoItem, index) {
      this.$emit('removeItem', todoItem, index);
    },
    toggleComplete(todoItem, index) {
      this.$emit('toggleItem', todoItem, index);
    }
  },
  computed: {
    storedTodoItems() {
      // return this.$store.state.todoItems;
      return this.$store.getters.getTodoItems;
    }
  }
}
</script>
```

---

## mutations

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/mutations)

* **mutations** - state를 변경하는 이벤트/매서드

```js
state: { num: 10 },
mutations: {
    printNum(state) {
        return state.num;
    },
    sumNum(state, num2) {
        return state.num + num2;
    }
}

this.$store.commit('printNum');     // 10
this.$store.commit('sumNum', 20);   // 20
```

```js
state: { num: 10 },
mutations: {
    modify(state, obj) {
        console.log(obj.str);
        return state.num += obj.num;
    }
}

this.$store.commit('modify',{
    str: 'hi',
    num: 10
});
```

```js
// store.js

import { createStore } from 'vuex'

const storage = {
	fetch() {
		const arr = [];
		if (localStorage.length > 0) {
      for (let i = 0; i < localStorage.length; i++) {
        if (localStorage.key(i) !== 'loglevel:webpack-dev-server') {
          arr.push(JSON.parse(localStorage.getItem(localStorage.key(i))));
        }
      }
    }
		return arr;
	}
}

export const store = createStore({
	state: {
		todoItems: storage.fetch()
	},
	getters: {
		getTodoItems(state) {
			return state.todoItems;
		}
    },
    mutations: {
        addOneItem(state, todoItem) {
            const obj = {completed: false, item: todoItem};
      localStorage.setItem(todoItem, JSON.stringify(obj));
            state.todoItems.push(obj);
        },
        removeOneItem(state, payload) {
            state.todoItems.splice(payload.index, 1);
      localStorage.removeItem(payload.todoItem.item);
        },
        toggleOneItem(state, payload) {
            state.todoItems[payload.index].completed = !state.todoItems[payload.index].completed;
      localStorage.removeItem(payload.todoItem.item);
      localStorage.setItem(payload.todoItem.item, JSON.stringify(payload.todoItem));
        },
        clearAllItems(state) {
            state.todoItems = [];
            localStorage.clear();
        }
    }
});

```

```html
<script>
export default {
  props: ['propsdata'],
  methods: {
    removeTodo(todoItem, index) {
      this.$store.commit('removeOneItem', {todoItem, index});
    },
    toggleComplete(todoItem, index) {
      this.$store.commit('toggleOneItem', {todoItem, index});
    }
  },
  computed: {
    storedTodoItems() {
      // return this.$store.state.todoItems;
      return this.$store.getters.getTodoItems;
    }
  }
}
</script>
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/vuejs/2.VueJS_InterM/2.VueJS_InterM-7-1.jpg"/>
</p>

---

## action

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/action)

* **actions** - 비동기 처리 로직 선언 매서드(CF. mutation은 동기식임)

```js
state: { num: 10 },
mutations:{
    doubleNum(context) {
        state.num * 2;
    }
},
actions: {
    delayDoubleNum(context) {
        context.commit('doubleNum')
    }
}

this.$store.dispatch('delayDoubleNum')
```

```js
// Example
mutations: {
    addCounter(state) {
        state.counter++
    },
},
actions: {
    delayedAddCounter(context) {
        // 2초 후 카운터 증가1
        setTimeout(() => constext.commit('addCounter'), 2000);
    }
}

methods: {
    imcrementCounter() {
        this.$store.dispatch('delayedAddCounter');
    }
}
```

```js
// Example 2
mutatoins: {
    setData(state, fectchedData) {
        state.product = fectchedData;
    }
},
actions: {
    fectchProductData(context) {
        return axios.get('https://domain.com/products/1').then(response => context.commit('setData', response));
    }
}

methods: {
    getProduct() {
        this.$store.dispatch('fectchProductData');
    }
}
```

{% endraw %}