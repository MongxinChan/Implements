/*
 * cache_controller.c
 *
 * 缓存控制器的实现。
 */

#include "cache_controller.h"
#include <stdio.h> // 用于 printf 和 scanf

void print_menu(void) {
    printf("--- LRU 缓存演示 ---\n");
    printf("  (p) Put (插入/更新)\n");
    printf("  (g) Get (获取)\n");
    printf("  (d) Display (显示当前状态)\n");
    printf("  (q) Quit (退出)\n");
    printf("请输入您的选择: ");
}

void handle_put(LRUCache* cache) {
    int key, value;
    
    printf("  请输入 Key: ");
    // 检查 scanf 的返回值是一种好的做法
    if (scanf("%d", &key) != 1) {
        printf("  无效的 Key。\n");
        // 清空输入缓冲区
        while (getchar() != '\n'); 
        return;
    }
    
    printf("  请输入 Value: ");
    if (scanf("%d", &value) != 1) {
        printf("  无效的 Value。\n");
        while (getchar() != '\n');
        return;
    }

    lru_cache_put(cache, key, value);
    printf("  操作成功：Put(%d, %d)\n", key, value);
    handle_print(cache); // 操作后立即显示状态
}

void handle_get(LRUCache* cache) {
    int key;
    printf("  请输入 Key: ");
    
    if (scanf("%d", &key) != 1) {
        printf("  无效的 Key。\n");
        while (getchar() != '\n');
        return;
    }

    int value = lru_cache_get(cache, key);
    
    if (value == -1) {
        printf("  操作结果：Get(%d) -> 未找到 (Not Found)\n\n", key);
    } else {
        printf("  操作结果：Get(%d) -> 找到了 %d\n", key, value);
        handle_print(cache); // Get 操作会改变顺序，所以打印
    }
}

void handle_print(LRUCache* cache) {
    lru_cache_print(cache);
}