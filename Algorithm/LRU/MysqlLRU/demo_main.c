/*
 * demo_main.c
 *
 * 演示 InnoDB LRU 优化的效果。
 */
#include "lru_innodb.h"
#include <stdio.h>
#include <unistd.h> // 用于 sleep()

int main() {
    // 创建一个容量为 10, 老生代比例 40% (4个), 晋升延迟 1 秒的缓存
    int capacity = 10;
    double old_ratio = 0.4; // 4 个
    unsigned long delay = 1000; // 1000 毫秒 = 1 秒
    
    LRUCache* cache = lru_cache_create(capacity, old_ratio, delay);
    printf("创建缓存：Capacity=10, Old_Size_Target=4, Delay=1s\n");

    // --- 场景 1: 演示“缓存污染” (全表扫描) ---
    printf("--- 场景 1: 缓存污染测试 ---\n");
    printf("1. 填入 6 个“热”数据 (Key 1-6)\n");
    for (int i = 1; i <= 6; i++) {
        lru_cache_put(cache, i, i * 10);
    }
    lru_cache_print(cache);
    printf("注意：所有数据都在 'Old' 区 (因为它们是新插入的)\n");

    printf("2. 访问 Key 1-6，使其“晋升”(需要两次访问 + 延迟)\n");
    // (为了演示简单，我们直接访问两次并等待)
    for (int i = 1; i <= 6; i++) {
        lru_cache_get(cache, i); // 第一次访问 (设置时间戳)
    }
    printf("... (第一次访问 Key 1-6)\n");
    sleep(1); // 等待超过 1 秒延迟
    printf("... (等待 1 秒)\n");
    for (int i = 1; i <= 6; i++) {
        lru_cache_get(cache, i); // 第二次访问 (触发晋升)
    }
    printf("... (第二次访问 Key 1-6，触发晋升)\n");
    lru_cache_print(cache);
    printf("注意：Key 1-6 现在都在 'New' 区了。\n");
    
    printf("3. 模拟全表扫描 (插入 4 个 Key 101-104)，填满缓存\n");
    for (int i = 101; i <= 104; i++) {
        lru_cache_put(cache, i, i);
    }
    lru_cache_print(cache);
    printf("!!! 结果：缓存已满 (10/10)。\n");
    printf("!!! “热”数据 (1-6) 仍在 'New' 区。\n");
    printf("!!! “扫描”数据 (101-104) 停留在 'Old' 区。\n");

    printf("4. 模拟扫描更多数据 (Key 105)，触发淘汰\n");
    lru_cache_put(cache, 105, 105);
    lru_cache_print(cache);
    printf("!!! 结果：Key 101 (Old 区的尾部) 被淘汰。\n");
    printf("!!! “热”数据 (1-6) 毫发无损。\n");
    printf("--- 场景 1 演示完毕 ---\n\n");


    // --- 场景 2: 演示“延迟晋升” ---
    printf("--- 场景 2: 延迟晋升测试 ---\n");
    lru_cache_destroy(cache); // 重置缓存
    cache = lru_cache_create(capacity, old_ratio, delay);
    printf("1. 插入一个新 Key 99\n");
    lru_cache_put(cache, 99, 990);
    lru_cache_print(cache);
    
    printf("2. 立即 Get(99) (第一次访问)\n");
    lru_cache_get(cache, 99);
    lru_cache_print(cache);
    printf("!!! 结果：Key 99 仍在 'Old' 区 (仅设置了时间戳)\n");
    
    printf("3. 再次立即 Get(99) (未到延迟时间)\n");
    lru_cache_get(cache, 99);
    lru_cache_print(cache);
    printf("!!! 结果：Key 99 仍在 'Old' 区 (未到延迟时间)\n");

    printf("4. 等待 2 秒 (超过 1 秒延迟)...\n");
    sleep(2);
    
    printf("5. 再次 Get(99) (已过延迟时间)\n");
    lru_cache_get(cache, 99);
    lru_cache_print(cache);
    printf("!!! 结果：Key 99 已成功晋升到 'New' 区！\n");
    printf("--- 场景 2 演示完毕 ---\n");

    lru_cache_destroy(cache);
    return 0;
}