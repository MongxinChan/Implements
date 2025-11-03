/*
 * cache_controller.c
 *
 * 缓存控制器的实现。
 */

#include "innodb_controller.h"
#include <stdio.h>
#include <unistd.h> // <unistd.h> (Linux/macOS) 或 <windows.h> (Windows)

// 辅助函数：清除输入缓冲区
static void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_menu(void) {
    printf("--- InnoDB LRU 演示 (Key: int) ---\n");
    printf("  (p) Put (插入/更新)\n");
    printf("  (g) Get (获取)\n");
    printf("  (d) Display (显示当前状态)\n");
    printf("  (w) Wait (等待 N 秒)\n"); // <-- 新增的等待选项
    printf("  (q) Quit (退出)\n");
    printf("请输入您的选择: ");
}

void handle_put(LRUCache* cache) {
    int key, value;
    
    printf("  请输入 Key (int): ");
    if (scanf("%d", &key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer(); 
        return;
    }
    
    printf("  请输入 Value (int): ");
    if (scanf("%d", &value) != 1) {
        printf("  无效的 Value。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer(); // 清除 scanf 遗留的换行符

    lru_cache_put(cache, key, value);
    printf("  操作成功：Put(%d, %d)\n", key, value);
    handle_print(cache); // 操作后立即显示状态
}

void handle_get(LRUCache* cache) {
    int key;
    printf("  请输入 Key (int): ");
    
    if (scanf("%d", &key) != 1) {
        printf("  无效的 Key。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

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

void handle_wait(void) {
    int seconds = 0;
    printf("  请输入要等待的秒数 (用于测试延迟晋升): ");
    if (scanf("%d", &seconds) != 1 || seconds < 0) {
        printf("  无效的秒数。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    printf("  正在等待 %d 秒...\n", seconds);
    
    // sleep() 在 unistd.h (Linux/macOS) 中
    // Sleep() 在 windows.h (Windows) 中
    sleep(seconds);
    
    printf("  ...等待完毕。\n\n");
}