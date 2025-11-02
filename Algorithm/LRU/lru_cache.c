/*
 * lru_cache.c
 *
 * LRU 缓存的内部实现。
 */

#include "lru_cache.h"
#include <stdlib.h>
#include <stdio.h> // NULL 和 printf

// --- 内部数据结构定义 ---

/**
 * @brief 内部节点结构
 * 它同时作为双向链表节点和哈希表冲突链的节点。
 */
typedef struct Node {
    char* key;
    int value;
    struct Node* prev;  // 双向链表 (LRU 顺序)
    struct Node* next;  // 双向链表 (LRU 顺序)
    struct Node* hnext; // 哈希表冲突链
} Node;

/**
 * @brief LRU 缓存的完整内部定义
 * (这是 lru_cache.h 中不透明指针的实际结构)
 */
struct LRUCache {
    int capacity;
    int size;
    Node* head;       // LRU 链表的哨兵头节点
    Node* tail;       // LRU 链表的哨兵尾节点
    Node** hash_map;   // 哈希表 (一个指针数组)
    int hash_map_size;
};


// --- 内部辅助函数 (声明为 static) ---

/**
 * @brief SDBM 字符串哈希函数
 */
static unsigned long _hash(const char *str, int map_size) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % map_size;
}

/**
 * @brief (哈希表) 在哈希表中查找节点
 */
static Node* _hash_find(LRUCache* cache, const char* key) {
    int index = _hash(key, cache->hash_map_size);
    Node* current = cache->hash_map[index];

    // 遍历冲突链，使用 strcmp 比较
    while (current) {
        if (strcmp(current->key, key) == 0) { // <--- 更改为 strcmp
            return current;
        }
        current = current->hnext;
    }
    return NULL;
}

/**
 * @brief (哈希表) 从哈希表中移除一个节点
 * (注意：此函数不释放 node 或 node->key 的内存)
 */
static void _hash_remove(LRUCache* cache, Node* node) {
    int index = _hash(node->key, cache->hash_map_size);
    Node* current = cache->hash_map[index];
    Node* prev = NULL;

    while (current) {
        if (current == node) {
            if (prev) {
                prev->hnext = current->hnext;
            } else {
                cache->hash_map[index] = current->hnext;
            }
            return;
        }
        prev = current;
        current = current->hnext;
    }
}

/**
 * @brief (哈希表) 向哈希表中插入一个新节点
 */
static void _hash_insert(LRUCache* cache, Node* node) {
    int index = _hash(node->key, cache->hash_map_size);
    node->hnext = cache->hash_map[index];
    cache->hash_map[index] = node;
}

/**
 * @brief (LRU链表) 将一个节点从链表中移除
 */
static void _list_remove_node(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/**
 * @brief (LRU链表) 将一个节点添加到链表头部（设为最近使用）
 */
static void _list_add_to_head(LRUCache* cache, Node* node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

/**
 * @brief (LRU链表) 将一个节点移动到链表头部
 */
static void _list_move_to_head(LRUCache* cache, Node* node) {
    _list_remove_node(node);
    _list_add_to_head(cache, node);
}

/**
 * @brief (LRU链表) 移除链表尾部节点（最少使用）
 * @return 返回被移除的节点
 */
static Node* _list_remove_tail(LRUCache* cache) {
    Node* tailNode = cache->tail->prev;
    _list_remove_node(tailNode);
    return tailNode;
}


// --- 公共 API 函数实现 ---

LRUCache* lru_cache_create(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }

    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    if (!cache) return NULL;

    cache->capacity = capacity;
    cache->size = 0;
    // 为简单起见，哈希表大小设为容量的2倍
    cache->hash_map_size = capacity * 2;
    
    // 使用 calloc 将所有哈希桶指针初始化为 NULL
    cache->hash_map = (Node**)calloc(cache->hash_map_size, sizeof(Node*));
    if (!cache->hash_map) {
        free(cache);
        return NULL;
    }

    // 初始化哨兵头尾节点
    cache->head = (Node*)malloc(sizeof(Node));
    cache->tail = (Node*)malloc(sizeof(Node));
    
    if (!cache->head || !cache->tail) {
        free(cache->hash_map);
        if (cache->head) free(cache->head);
        if (cache->tail) free(cache->tail);
        free(cache);
        return NULL;
    }

    cache->head->prev = NULL;
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    cache->tail->next = NULL;

    return cache;
}

void lru_cache_destroy(LRUCache* cache) {
    if (!cache) return;

    // 释放所有数据节点（遍历LRU链表）
    Node* current = cache->head->next;
    while (current != cache->tail) {
        Node* temp = current;
        current = current->next;
        
        free(temp->key); // <--- 释放键的副本
        free(temp);      // <--- 释放节点
    }
    
    // 释放哨兵节点
    free(cache->head);
    free(cache->tail);
    
    // 释放哈希表数组
    free(cache->hash_map);
    
    // 释放缓存主结构体
    free(cache);
}

int lru_cache_get(LRUCache* cache, const char* key) {
    if (!cache || !key) return -1;
    
    Node* node = _hash_find(cache, key);
    
    if (node == NULL) {
        return -1; // 未找到
    }
    
    _list_move_to_head(cache, node);
    return node->value;
}

void lru_cache_put(LRUCache* cache, const char* key, int value) {
    if (!cache || !key) return;

    Node* node = _hash_find(cache, key);
    
    if (node != NULL) {
        // --- 1. 键已存在 (更新) ---
        node->value = value;
        _list_move_to_head(cache, node);
    } else {
        // --- 2. 键不存在 (插入) ---
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (!newNode) return; // 内存不足
        
        // strdup 是 POSIX 函数，等价于:
        // newNode->key = malloc(strlen(key) + 1);
        // strcpy(newNode->key, key);
        newNode->key = strdup(key); // <--- 关键：创建键的副本
        if (!newNode->key) { // 检查 strdup 是否失败
            free(newNode);
            return;
        }
        newNode->value = value;
        
        _hash_insert(cache, newNode);
        _list_add_to_head(cache, newNode);
        cache->size++;

        // --- 3. 检查是否超出容量 (淘汰) ---
        if (cache->size > cache->capacity) {
            Node* tailNode = _list_remove_tail(cache);
            _hash_remove(cache, tailNode);
            
            free(tailNode->key); // <--- 关键：释放被淘汰的键
            free(tailNode);      // <--- 关键：释放被淘汰的节点
            
            cache->size--;
        }
    }
}


void lru_cache_print(LRUCache* cache) {
    if (!cache) return;
    printf("  LRU Cache (Size: %d / Capacity: %d)\n", cache->size, cache->capacity);
    printf("  [Head] -> ");
    Node* current = cache->head->next;
    while (current != cache->tail) {
        // <--- 更改打印格式以包含字符串
        printf("[(\"%s\": %d)] -> ", current->key, current->value); 
        current = current->next;
    }
    printf("[Tail]\n\n");
}