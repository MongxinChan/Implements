/*
 * lfu_controller.c
 *
 * LFU 缓存控制器的实现。
 */

#include "lfu_controller.h"
#include <stdio.h> // 用于 printf 和 scanf

#define KEY_BUFFER_SIZE 256

// 辅助函数：清除输入缓冲区
static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_menu(void) {
    printf("--- LFU 缓存演示 (Key: String) ---\n");
    printf("  (p) Put (插入/更新)\n");
    printf("  (g) Get (获取)\n");
    printf("  (d) Display (显示当前状态)\n");
    printf("  (q) Quit (退出)\n");
    printf("请输入您的选择: ");
}

void handle_put(LFUCache* cache) {
    char key[KEY_BUFFER_SIZE];
    int value;
    
    printf("  请输入 Key (string): ");
    if (scanf("%255s", key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer(); 
        return;
    }
    clear_input_buffer();
    
    printf("  请输入 Value (int): ");
    if (scanf("%d", &value) != 1) {
        printf("  无效的 Value。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // --- 调用 LFU API ---
    lfu_cache_put(cache, key, value);
    printf("  操作成功：Put(\"%s\", %d)\n", key, value);
    handle_print(cache); // 操作后立即显示状态
}

void handle_get(LFUCache* cache) {
    char key[KEY_BUFFER_SIZE];
    printf("  请输入 Key (string): ");
    
    if (scanf("%255s", key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    // --- 调用 LFU API ---
    int value = lfu_cache_get(cache, key);
    
    if (value == -1) {
        printf("  操作结果：Get(\"%s\") -> 未找到 (Not Found)\n\n", key);
    } else {
        printf("  操作结果：Get(\"%s\") -> 找到了 %d\n", key, value);
        handle_print(cache); // Get 操作会改变频率和顺序，所以打印
    }
}

void handle_print(LFUCache* cache) {
    // --- 调用 LFU API ---
    lfu_cache_print(cache);
}