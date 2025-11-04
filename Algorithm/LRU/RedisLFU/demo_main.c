/*
 * main.c
 *
 * 演示 LFU 缓存的行为
 */
#include "lfu_standard.h"
#include <stdio.h>

int main() {
    int capacity = 3;
    LFUCache* cache = lfu_cache_create(capacity);
    printf("创建了一个容量为 %d 的 LFU 缓存。\n\n", capacity);

    lfu_cache_put(cache, "apple", 10);
    printf("Put(\"apple\", 10)\n");
    lfu_cache_print(cache);

    lfu_cache_put(cache, "banana", 20);
    printf("Put(\"banana\", 20)\n");
    lfu_cache_print(cache);
    
    lfu_cache_put(cache, "cherry", 30);
    printf("Put(\"cherry\", 30)\n");
    lfu_cache_print(cache);
    printf("缓存已满。\n\n");

    printf("--- Get(\"apple\") --- (apple 的频率将变为 2)\n");
    lfu_cache_get(cache, "apple");
    lfu_cache_print(cache);

    printf("--- Put(\"date\", 40) --- (缓存已满，需要淘汰)\n");
    printf("此时：\n  Key \"apple\" (Freq=2)\n  Key \"banana\" (Freq=1, LRU)\n  Key \"cherry\" (Freq=1, MRU)\n");
    printf("LFU 策略：淘汰 Freq 最低的。Freq=1 是最低的。\n");
    printf("平局策略 (LRU)：在 \"banana\" 和 \"cherry\" 中，\"banana\" 是 '最近最少使用' 的。\n");
    printf("--> 应该淘汰 \"banana\"。\n");
    lfu_cache_put(cache, "date", 40);
    lfu_cache_print(cache);

    printf("--- Get(\"cherry\") ---\n");
    lfu_cache_get(cache, "cherry");
    lfu_cache_print(cache);

    printf("--- Put(\"elderberry\", 50) --- (缓存已满，需要淘汰)\n");
    printf("此时：\n  Key \"apple\" (Freq=2)\n  Key \"cherry\" (Freq=2)\n  Key \"date\" (Freq=1)\n");
    printf("LFU 策略：淘汰 Freq 最低的，即 \"date\" (Freq=1)。\n");
    lfu_cache_put(cache, "elderberry", 50);
    lfu_cache_print(cache);


    lfu_cache_destroy(cache);
    printf("缓存已释放。\n");

    return 0;
}