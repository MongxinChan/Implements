/*
 * mainExample.c
 *
 * 演示如何使用 lru_cache 模块
 */
#include <stdio.h>
#include "lru_cache.h" // 只包含公共头文件

int main() {
    int capacity = 3;
    LRUCache* cache = lru_cache_create(capacity);
    
    if (!cache) {
        fprintf(stderr, "创建缓存失败！\n");
        return 1;
    }

    printf("创建了一个容量为 %d 的 LRU 缓存。\n\n", capacity);

    lru_cache_put(cache, 1, 10);
    printf("Put(1, 10)\n");
    lru_cache_put(cache, 2, 20);
    printf("Put(2, 20)\n");
    lru_cache_put(cache, 3, 30);
    printf("Put(3, 30)\n");
    printf("==============缓存已满==============。\n\n");

    // 此时顺序: [3] -> [2] -> [1]

    lru_cache_put(cache, 4, 40);
    printf("Put(4, 40) -> 应该淘汰 (1: 10)\n");
    
    // 此时顺序: [4] -> [3] -> [2]
    
    int val1 = lru_cache_get(cache, 1);
    printf("Get(1) -> 返回: %d (预期 -1)\n", val1);

    int val2 = lru_cache_get(cache, 2);
    printf("Get(2) -> 返回: %d (预期 20)\n", val2);
    printf("... (2: 20) 应该被移到头部\n\n");

    // 此时顺序: [2] -> [4] -> [3]

    lru_cache_put(cache, 5, 50);
    printf("Put(5, 50) -> 应该淘汰 (3: 30)\n");

    // 此时顺序: [5] -> [2] -> [4]
    
    int val3 = lru_cache_get(cache, 3);
    printf("Get(3) -> (已被淘汰) 返回: %d (预期 -1)\n", val3);

    // 释放内存
    lru_cache_destroy(cache);
    printf("\n==============缓存已释放==============\n");

    return 0;
}