/*
 * lru_innodb.c
 *
 * InnoDB 优化的 LRU 缓存实现。
 */

#define _POSIX_C_SOURCE 199309L
#include "lru_innodb.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h> // 用于 clock_gettime

// --- 内部数据结构定义 ---

typedef struct Node {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
    struct Node* hnext;

    // --- InnoDB 优化所需字段 ---
    int in_old_list;                 // 标志位：1 表示在 Old 区, 0 表示在 New 区
    unsigned long first_access_in_old; // 首次在 Old 区被访问的时间戳 (毫秒)
} Node;

struct LRUCache {
    int capacity;
    int size;
    Node* head;       // 哨兵：New 区的头部 (MRU)
    Node* tail;       // 哨兵：Old 区的尾部 (LRU)
    Node* midpoint;   // **关键指针**：指向 Old 区的头部
    
    int new_list_size;
    int old_list_size;
    
    Node** hash_map;
    int hash_map_size;
    
    unsigned long access_delay_ms; // 晋升延迟
};

// --- 内部辅助函数 ---

/**
 * @brief 获取单调递增的毫秒时间戳
 */
static unsigned long _get_current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)ts.tv_sec * 1000 + (unsigned long)ts.tv_nsec / 1000000;
}

static int _hash(int key, int map_size) {
    return (key & 0x7FFFFFFF) % map_size;
}

static Node* _hash_find(LRUCache* cache, int key) {
    int index = _hash(key, cache->hash_map_size);
    Node* current = cache->hash_map[index];
    while (current) {
        if (current->key == key) return current;
        current = current->hnext;
    }
    return NULL;
}

static void _hash_remove(LRUCache* cache, Node* node) {
    int index = _hash(node->key, cache->hash_map_size);
    Node* current = cache->hash_map[index];
    Node* prev = NULL;
    while (current) {
        if (current == node) {
            if (prev) prev->hnext = current->hnext;
            else cache->hash_map[index] = current->hnext;
            return;
        }
        prev = current;
        current = current->hnext;
    }
}

static void _hash_insert(LRUCache* cache, Node* node) {
    int index = _hash(node->key, cache->hash_map_size);
    node->hnext = cache->hash_map[index];
    cache->hash_map[index] = node;
}

/**
 * @brief (内部) 从链表中移除一个节点，并正确维护 midpoint
 */
static void _list_remove_node(LRUCache* cache, Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    
    // **关键**：如果移除的节点是 midpoint，则 midpoint 必须后移
    if (cache->midpoint == node) {
        cache->midpoint = node->next;
    }
    
    // 更新分区大小
    if (node->in_old_list) cache->old_list_size--;
    else cache->new_list_size--;
}

/**
 * @brief (内部) 将节点添加到 New 区的头部 (设为 MRU)
 */
static void _list_add_to_new_head(LRUCache* cache, Node* node) {
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
    
    node->in_old_list = 0; // 它现在在 New 区
    node->first_access_in_old = 0; // 重置时间戳
    cache->new_list_size++;
}

/**
 * @brief (内部) 将节点添加到 Old 区的头部 (中点插入)
 */
static void _list_add_to_old_head(LRUCache* cache, Node* node) {
    // 插入到 midpoint 的前面
    node->next = cache->midpoint;
    node->prev = cache->midpoint->prev;
    cache->midpoint->prev->next = node;
    cache->midpoint->prev = node;
    
    // **关键**：新插入的节点现在是 Old 区的新头部
    cache->midpoint = node;
    
    node->in_old_list = 1; // 它现在在 Old 区
    node->first_access_in_old = 0; // 尚未被访问
    cache->old_list_size++;
}

// --- 公共 API 实现 ---

LRUCache* lru_cache_create(int capacity, double old_list_ratio, unsigned long access_delay_ms) {
    if (capacity <= 0 || old_list_ratio <= 0 || old_list_ratio >= 1.0) {
        return NULL;
    }
    
    LRUCache* cache = (LRUCache*)calloc(1, sizeof(LRUCache));
    if (!cache) return NULL;

    cache->capacity = capacity;
    cache->size = 0;
    cache->new_list_size = 0;
    cache->old_list_size = 0;
    cache->access_delay_ms = access_delay_ms;

    cache->hash_map_size = capacity * 2;
    cache->hash_map = (Node**)calloc(cache->hash_map_size, sizeof(Node*));
    if (!cache->hash_map) { free(cache); return NULL; }

    cache->head = (Node*)calloc(1, sizeof(Node));
    cache->tail = (Node*)calloc(1, sizeof(Node));
    if (!cache->head || !cache->tail) {
        free(cache->hash_map);
        if (cache->head) free(cache->head);
        if (cache->tail) free(cache->tail);
        free(cache);
        return NULL;
    }

    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;

    // **关键**：初始化时，Old 区的头部就是 tail
    cache->midpoint = cache->tail;
    
    // 预先计算并设置 New/Old 的目标大小
    // (在更复杂的实现中，我们会动态维护这个比例)
    // (为简单起见，我们主要在 get() 中使用 new_list_size)

    return cache;
}

void lru_cache_destroy(LRUCache* cache) {
    if (!cache) return;
    Node* current = cache->head->next;
    while (current != cache->tail) {
        Node* temp = current;
        current = current->next;
        free(temp); // (如果是 char* key, 还要 free(temp->key))
    }
    free(cache->head);
    free(cache->tail);
    free(cache->hash_map);
    free(cache);
}

int lru_cache_get(LRUCache* cache, int key) {
    if (!cache) return -1;
    Node* node = _hash_find(cache, key);
    
    if (node == NULL) {
        return -1; // 未找到
    }

    if (node->in_old_list) {
        // --- 核心优化：延迟晋升 ---
        unsigned long now = _get_current_time_ms();
        
        if (node->first_access_in_old == 0) {
            // 1. 这是在 Old 区的 *首次* 访问
            node->first_access_in_old = now;
            // *不* 移动它，等待下次访问
        } else if ((now - node->first_access_in_old) > cache->access_delay_ms) {
            // 2. 访问间隔已超过延迟，执行“晋升”
            _list_remove_node(cache, node);
            _list_add_to_new_head(cache, node);
        } else {
            // 3. 访问间隔未超过延迟，*不* 移动它
        }
        
    } else {
        // --- 在 New 区：标准 LRU 行为 ---
        // 移动到 New 区的头部
        _list_remove_node(cache, node);
        _list_add_to_new_head(cache, node);
    }
    
    return node->value;
}

void lru_cache_put(LRUCache* cache, int key, int value) {
    if (!cache) return;
    Node* node = _hash_find(cache, key);
    
    if (node != NULL) {
        // 键已存在 (更新)
        node->value = value;
        // 访问它以触发 get() 中的晋升/移动逻辑
        lru_cache_get(cache, key);
    } else {
        // 键不存在 (插入)
        if (cache->size == cache->capacity) {
            // --- 淘汰 ---
            // 总是淘汰 Old 区的尾部
            Node* lru_node = cache->tail->prev;
            _hash_remove(cache, lru_node);
            _list_remove_node(cache, lru_node);
            free(lru_node);
            cache->size--;
        }
        
        // 创建新节点
        Node* newNode = (Node*)calloc(1, sizeof(Node));
        if (!newNode) return; // 内存不足
        newNode->key = key;
        newNode->value = value;

        // --- 核心优化：中点插入 ---
        // **新节点插入到 Old 区的头部**
        _list_add_to_old_head(cache, newNode);
        
        _hash_insert(cache, newNode);
        cache->size++;
    }
}

void lru_cache_print(LRUCache* cache) {
    if (!cache) return;
    
    printf("\n--- Cache State (Size: %d / Capacity: %d) ---\n", cache->size, cache->capacity);
    printf("--- [NEW List (Size: %d)] ---\n", cache->new_list_size);
    printf("  [Head] -> ");
    
    Node* current = cache->head->next;
    
    // 打印 New 区
    while (current != cache->midpoint && current != cache->tail) {
        printf("[(%d: %d)] -> ", current->key, current->value);
        current = current->next;
    }
    
    printf("\n--- [OLD List (Size: %d)] --- (Midpoint)\n", cache->old_list_size);
    printf("  [Mid] -> ");
    
    // 打印 Old 区
    while (current != cache->tail) {
        printf("[(%d: %d)] -> ", current->key, current->value);
        current = current->next;
    }
    
    printf("[Tail]\n\n");
}