/*
 * main.c
 *
 * 交互式 LRU 缓存演示的主程序。
 * 使用 switch 语句将操作分发给 cache_controller。
 */

#include <stdio.h>
#include "lru_cache.h"
#include "cache_controller.h"

// 辅助函数：清除输入缓冲区，防止 scanf 遗留换行符
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int capacity = 0;
    while (capacity <= 0) {
        printf("请输入 LRU 缓存的容量 (必须 > 0): ");
        if (scanf("%d", &capacity) != 1) {
            clear_input_buffer();
        }
    }
    
    LRUCache* cache = lru_cache_create(capacity);
    if (!cache) {
        fprintf(stderr, "创建缓存失败！\n");
        return 1;
    }
    printf("容量为 %d 的缓存已创建。\n\n", capacity);
    
    int done = 0;
    char choice = '\0';

    while (!done) {
        print_menu();
        
        // 读取用户的选择 (注意 ' %c' 中的空格，它会跳过之前的换行符)
        if (scanf(" %c", &choice) != 1) {
            clear_input_buffer(); // 处理无效输入
            continue;
        }
        
        // 清除选择后可能遗留的换行符
        clear_input_buffer(); 
        printf("\n");

        switch (choice) {
            case 'p': // Put
            case 'P':
                handle_put(cache);
                break;
            
            case 'g': // Get
            case 'G':
                handle_get(cache);
                break;
            
            case 'd': // Display
            case 'D':
                handle_print(cache);
                break;
            
            case 'q': // Quit
            case 'Q':
                done = 1;
                printf("正在退出...\n");
                break;
            
            default:
                printf("  无效的选择 '%c'，请重试。\n\n", choice);
                break;
        }
    }

    // 释放内存
    lru_cache_destroy(cache);
    printf("缓存已释放。再见！\n");

    return 0;
}