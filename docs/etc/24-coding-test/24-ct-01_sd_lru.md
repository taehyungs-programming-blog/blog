---
layout: default
title: "[System Design] LRU Cache"
parent: "24년 Coding Test 🦖"
grand_parent: "Etc 🛠"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Q

* LRU (Least Recently Used) 캐시를 설계하고 구현하시오. 이 캐시는 다음과 같은 기능을 지원해야 합니다.

* `int get(int key)`: 키가 존재하면 해당 값을 반환하고, 그렇지 않으면 -1을 반환합니다. 이때, 사용된 키는 가장 최근에 사용된 것으로 갱신됩니다.
* `void put(int key, int value)`: 키와 값을 캐시에 저장합니다. 캐시의 용량을 초과하면 가장 오랫동안 사용되지 않은 항목을 제거합니다.
* 캐시의 용량은 고정되어 있으며, 생성자에서 용량을 설정할 수 있어야 합니다.

---

## A

```cpp
#include <unordered_map>  

class LRUCache {  
private:  
    // 이중 연결 리스트의 노드 구조체 정의  
    struct Node {  
        int key;  
        int value;  
        Node* prev;  
        Node* next;  
        Node(int k, int v): key(k), value(v), prev(nullptr), next(nullptr) {}  
    };  

    int capacity; // 캐시 용량  
    std::unordered_map<int, Node*> cache; // 키와 노드의 매핑  
    Node* head; // 가장 최근에 사용된 노드  
    Node* tail; // 가장 오래된 노드  

    // 노드를 리스트의 앞쪽(head)으로 이동  
    void moveToHead(Node* node) {  
        removeNode(node);  
        addNode(node);  
    }  

    // 노드를 리스트에서 제거  
    void removeNode(Node* node) {  
        if (node->prev != nullptr)  
            node->prev->next = node->next;  
        else  
            head = node->next;  

        if (node->next != nullptr)  
            node->next->prev = node->prev;  
        else  
            tail = node->prev;  
    }  

    // 노드를 리스트의 앞쪽(head)에 추가  
    void addNode(Node* node) {  
        node->next = head;  
        node->prev = nullptr;  

        if (head != nullptr)  
            head->prev = node;  
        head = node;  

        if (tail == nullptr)  
            tail = head;  
    }  

    // 꼬리 노드(tail)를 제거  
    void removeTail() {  
        if (tail != nullptr) {  
            removeNode(tail);  
        }  
    }  

public:  
    // 생성자: 캐시 용량 설정  
    LRUCache(int capacity) : capacity(capacity), head(nullptr), tail(nullptr) {}  

    // 키에 해당하는 값을 반환  
    int get(int key) {  
        if (cache.find(key) != cache.end()) {  
            Node* node = cache[key];  
            moveToHead(node); // 노드를 가장 앞으로 이동  
            return node->value;  
        }  
        return -1; // 키가 존재하지 않음  
    }  

    // 키와 값을 캐시에 삽입  
    void put(int key, int value) {  
        if (cache.find(key) != cache.end()) {  
            // 키가 이미 존재하면 값 갱신 후 가장 앞으로 이동  
            Node* node = cache[key];  
            node->value = value;  
            moveToHead(node);  
        } else {  
            // 새로운 노드 생성  
            Node* newNode = new Node(key, value);  
            if (cache.size() >= capacity) {  
                // 용량 초과 시 꼬리 노드 제거  
                cache.erase(tail->key);  
                removeTail();  
            }  
            addNode(newNode); // 노드를 가장 앞으로 추가  
            cache[key] = newNode; // 맵에 추가  
        }  
    }  
};
```