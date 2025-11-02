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

// 辅助函数：清除输入缓冲区
static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void handle_put(LRUCache* cache) {
    char key[KEY_BUFFER_SIZE]; // <--- 用于读取字符串的缓冲区
    int value;
    
    printf("  请输入 Key (string): ");
    // 读取字符串, %255s 防止缓冲区溢出
    if (scanf("%255s", key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer(); 
        return;
    }
    clear_input_buffer(); // 清除 scanf 遗留的换行符
    
    printf("  请输入 Value (int): ");
    if (scanf("%d", &value) != 1) {
        printf("  无效的 Value。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer(); // 清除 scanf 遗留的换行符

    lru_cache_put(cache, key, value);
    printf("  操作成功：Put(\"%s\", %d)\n", key, value);
    handle_print(cache); // 操作后立即显示状态
}

void handle_get(LRUCache* cache) {
    char key[KEY_BUFFER_SIZE];
    printf("  请输入 Key (string): ");
    
    if (scanf("%255s", key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int value = lru_cache_get(cache, key);
    
    if (value == -1) {
        printf("  操作结果：Get(\"%s\") -> 未找到 (Not Found)\n\n", key);
    } else {
        printf("  操作结果：Get(\"%s\") -> 找到了 %d\n", key, value);
        handle_print(cache); // Get 操作会改变顺序，所以打印
    }
}

void handle_print(LRUCache* cache) {
    lru_cache_print(cache);
}