/*
 * lfu_standard.c
 *
 * LFU O(1) 算法的核心实现 (Key: char*)
 */

#include "lfu_standard.h"
#include <stdlib.h> // calloc, malloc, free
#include <stdio.h>  // NULL, printf
#include <string.h> // strcmp, strlen, strcpy

// --- 内部数据结构 ---

typedef struct Node {
    char* key; // 
    int value;
    int frequency;
    struct Node* prev;
    struct Node* next;
    struct Node* hnext_key;
} Node;


typedef struct FreqList {
    int frequency;
    int size;
    Node* head;
    Node* tail;
    struct FreqList* hnext_freq;
} FreqList;

struct LFUCache {
    int capacity;
    int size;
    int minFrequency;
    Node** keyToNodeMap;
    int keyMapSize;
    FreqList** freqToListMap;
    int freqMapSize;
};

// --- 内部哈希函数 ---

/**
 * @brief SDBM 字符串哈希函数
 */
static unsigned long _key_hash(const char *str, int map_size) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash % map_size;
}


static int _freq_hash(int freq, int map_size) {
    return (freq & 0x7FFFFFFF) % map_size;
}

// --- 内部 FreqList 链表操作 ---
// ( _list_remove_node, _list_add_to_head, _list_remove_tail )
static void _list_remove_node(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}
static void _list_add_to_head(FreqList* list, Node* node) {
    node->next = list->head->next;
    node->prev = list->head;
    list->head->next->prev = node;
    list->head->next = node;
    list->size++;
}
static Node* _list_remove_tail(FreqList* list) {
    if (list->size == 0) return NULL;
    Node* tailNode = list->tail->prev;
    _list_remove_node(tailNode);
    list->size--;
    return tailNode;
}


// --- 内部哈希表 1 (Key->Node) 操作 ---
static Node* _hash_find_node(LFUCache* cache, const char* key) {
    int index = _key_hash(key, cache->keyMapSize);
    Node* current = cache->keyToNodeMap[index];
    while (current) {
        if (strcmp(current->key, key) == 0) return current; // <---  strcmp
        current = current->hnext_key;
    }
    return NULL;
}
static void _hash_insert_node(LFUCache* cache, Node* node) {
    int index = _key_hash(node->key, cache->keyMapSize); // <--- node->key 已经是 char*
    node->hnext_key = cache->keyToNodeMap[index];
    cache->keyToNodeMap[index] = node;
}

static void _hash_remove_node(LFUCache* cache, Node* node) {
    int index = _key_hash(node->key, cache->keyMapSize);
    Node* current = cache->keyToNodeMap[index];
    Node* prev = NULL;
    while (current) {
        if (current == node) {
            if (prev) prev->hnext_key = current->hnext_key;
            else cache->keyToNodeMap[index] = current->hnext_key;
            return;
        }
        prev = current;
        current = current->hnext_key;
    }
}


// --- 内部哈希表 2 (Freq->FreqList) 操作 ---
// ( _hash_find_freq_list, _hash_remove_freq_list, _get_or_create_freq_list )

static FreqList* _hash_find_freq_list(LFUCache* cache, int freq) {
    int index = _freq_hash(freq, cache->freqMapSize);
    FreqList* current = cache->freqToListMap[index];
    while (current) {
        if (current->frequency == freq) return current;
        current = current->hnext_freq;
    }
    return NULL;
}
static void _hash_remove_freq_list(LFUCache* cache, FreqList* list) {
    int index = _freq_hash(list->frequency, cache->freqMapSize);
    FreqList* current = cache->freqToListMap[index];
    FreqList* prev = NULL;
    while (current) {
        if (current == list) {
            if (prev) prev->hnext_freq = current->hnext_freq;
            else cache->freqToListMap[index] = current->hnext_freq;
            free(list->head);
            free(list->tail);
            free(list);
            return;
        }
        prev = current;
        current = current->hnext_freq;
    }
}
static FreqList* _get_or_create_freq_list(LFUCache* cache, int freq) {
    FreqList* list = _hash_find_freq_list(cache, freq);
    if (list == NULL) {
        list = (FreqList*)calloc(1, sizeof(FreqList));
        list->frequency = freq;
        list->head = (Node*)calloc(1, sizeof(Node));
        list->tail = (Node*)calloc(1, sizeof(Node));
        list->head->next = list->tail;
        list->tail->prev = list->head;
        int index = _freq_hash(freq, cache->freqMapSize);
        list->hnext_freq = cache->freqToListMap[index];
        cache->freqToListMap[index] = list;
    }
    return list;
}


// --- LFU 核心逻辑 ---
static void _update_node_frequency(LFUCache* cache, Node* node) {
    int oldFreq = node->frequency;
    FreqList* oldList = _hash_find_freq_list(cache, oldFreq);
    _list_remove_node(node);
    oldList->size--;
    int newFreq = oldFreq + 1;
    node->frequency = newFreq;
    FreqList* newList = _get_or_create_freq_list(cache, newFreq);
    _list_add_to_head(newList, node);
    if (oldList->size == 0 && cache->minFrequency == oldFreq) {
        cache->minFrequency = newFreq;
        _hash_remove_freq_list(cache, oldList);
    }
}

// --- 公共 API 实现 ---

LFUCache* lfu_cache_create(int capacity) {
    if (capacity <= 0) return NULL;
    LFUCache* cache = (LFUCache*)calloc(1, sizeof(LFUCache));
    cache->capacity = capacity;
    cache->keyMapSize = capacity * 2;
    cache->freqMapSize = capacity * 2;
    cache->keyToNodeMap = (Node**)calloc(cache->keyMapSize, sizeof(Node*));
    cache->freqToListMap = (FreqList**)calloc(cache->freqMapSize, sizeof(FreqList*));
    return cache;
}

void lfu_cache_destroy(LFUCache* cache) {
    if (!cache) return;
    for (int i = 0; i < cache->freqMapSize; i++) {
        FreqList* list = cache->freqToListMap[i];
        while (list) {
            FreqList* nextList = list->hnext_freq;
            Node* current = list->head->next;
            while (current != list->tail) {
                Node* temp = current;
                current = current->next;
                free(temp->key); // <--- 关键：释放键的副本
                free(temp);      // <--- 释放节点
            }
            free(list->head);
            free(list->tail);
            free(list);
            list = nextList;
        }
    }
    free(cache->keyToNodeMap);
    free(cache->freqToListMap);
    free(cache);
}

int lfu_cache_get(LFUCache* cache, const char* key) {
    if (!cache || !key) return -1;
    Node* node = _hash_find_node(cache, key);
    if (node == NULL) {
        return -1;
    }
    _update_node_frequency(cache, node);
    return node->value;
}

void lfu_cache_put(LFUCache* cache, const char* key, int value) {
    if (!cache || !key || cache->capacity <= 0) return;

    Node* node = _hash_find_node(cache, key);
    
    if (node != NULL) {
        // --- 1. 键已存在 (更新) ---
        node->value = value;
        _update_node_frequency(cache, node);
    } else {
        // --- 2. 键不存在 (插入) ---
        if (cache->size == cache->capacity) {
            // --- 2a. 淘汰 ---
            FreqList* minList = _hash_find_freq_list(cache, cache->minFrequency);
            Node* nodeToEvict = _list_remove_tail(minList);
            _hash_remove_node(cache, nodeToEvict);
            
            free(nodeToEvict->key); // <--- 关键：释放被淘汰的键
            free(nodeToEvict);      // <--- 关键：释放被淘汰的节点
            
            if (minList->size == 0) {
                _hash_remove_freq_list(cache, minList);
            }
            cache->size--;
        }
        
        // --- 2b. 创建新节点 ---
        Node* newNode = (Node*)calloc(1, sizeof(Node));
        if (!newNode) return; // 内存不足

        // <--- 关键：创建键的副本 (C11 兼容) ---
        size_t key_len = strlen(key);
        newNode->key = (char*)malloc(key_len + 1); // +1 for '\0'
        if (newNode->key == NULL) {
            free(newNode);
            return; // 内存不足
        }
        strcpy(newNode->key, key);
        // --- 副本创建完毕 ---
        
        newNode->value = value;
        newNode->frequency = 1;

        // --- 2c. 插入新节点 ---
        _hash_insert_node(cache, newNode);
        FreqList* listOne = _get_or_create_freq_list(cache, 1);
        _list_add_to_head(listOne, newNode);
        
        cache->size++;
        cache->minFrequency = 1;
    }
}

void lfu_cache_print(LFUCache* cache) {
    if (!cache) return;
    printf("\n--- LFU Cache (Size: %d / Capacity: %d) ---\n", cache->size, cache->capacity);
    printf("--- (Global MinFrequency: %d) ---\n", cache->minFrequency);

    int max_freq_to_print = cache->minFrequency + 10;
    
    for (int f = cache->minFrequency; f <= max_freq_to_print; f++) {
        if (f == 0) continue;
        FreqList* list = _hash_find_freq_list(cache, f);
        if (list != NULL) {
            printf("  [Freq = %d] (Size: %d)\n", f, list->size);
            printf("    (MRU) Head -> ");
            Node* current = list->head->next;
            while (current != list->tail) {
                // <--- 更改打印格式
                printf("[(\"%s\": %d)] -> ", current->key, current->value); 
                current = current->next;
            }
            printf("Tail (LRU)\n");
        }
    }
    printf("--- End of LFU Cache ---\n\n");
}