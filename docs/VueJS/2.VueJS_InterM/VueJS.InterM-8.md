---
layout: default
title: "8. Vuex Helper"
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

* [Get Code 🌎](https://github.com/EasyCoding-7/vue-3.0-intermidiate/tree/map)

🍅 **helper** - store를 좀 더 쉽게 쓰기 위해서 도입됨

* **mapState**
* **mapGetters**
* **mapMutations** 
* **mapActions** 

```js
import { mapState } from 'vuex'
import { mapGetters } from 'vuex'
import { mapMutations } from 'vuex'
import { mapActions } from 'vuex'

export default {
    computed() {
        ...mapState(['num']),
        ...mapGetters(['countedNum'])
    },
    methods: {
        ...mapMutations(['clickBtn']),
        ...mapActions(['asyncClickBtn'])
    }
}
```

🍅 참고로 `...`은 왜 쓸까?

```js
let josh = {
  field: 'web',
  language: 'js'
};

let developer = {
  nation: 'kor',
  person: josh
};

console.log(developer);
/*
[object Object] {
  nation: "kor",

  // person이 object로 들어가길 원했던게 아님
  person: [object Object] {
    field: "web",
    language: "js"
  }
}
*/
```

```js
let josh = {
  field: 'web',
  language: 'js'
};

let developer = {
  nation: 'kor',
  ...josh
};

console.log(developer);
/*
[object Object] {
  field: "web",
  language: "js",
  nation: "kor"
}
*/
```

* [Run This Code 🌎](https://jsbin.com/?html,js,output)

---

## mapState

```js
import { mapState } from 'vuex'

computed() {
    ...mapState(['num'])
}

// store.js
state: {
    num: 10
}
```

```html
<!-- <p>{{ this.$store.state.num }}</p> -->
<p>{{ this.num }}</p>
```

```vue
<template>
  <div class="clearAllContainer">
    <span class="clearAllBtn" v-on:click="clearTodo">Clear All</span>
  </div>
</template>

<script>
import { mapMutations } from 'vuex'

export default {
  methods: {
    ...mapMutations({
      clearTodo: 'clearAllItems'
    })
  }
}
</script>
```

```vue
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
import { mapGetters, mapMutations } from 'vuex'

export default {
  props: ['propsdata'],
  methods: {
    ...mapMutations({
      removeTodo: 'removeOneItem',
      toggleComplete: 'toggleOneItem'
    })
  },
  computed: {
    ...mapGetters({
      storedTodoItems: 'getTodoItems'
    })
  }
}
</script>
```

{% endraw %}