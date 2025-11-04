/*
 * main.c
 *
 * 交互式 LFU 缓存演示的主程序。
 */

#include <stdio.h>
#include "lfu_controller.h"

// 辅助函数：清除输入缓冲区
void clear_main_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int capacity = 0;
    while (capacity <= 0) {
        printf("请输入 LFU 缓存的容量 (必须 > 0): ");
        if (scanf("%d", &capacity) != 1) {
            clear_main_input_buffer();
        }
    }
    clear_main_input_buffer(); // 清除 scanf 遗留的换行符
    
    // --- 调用 LFU API ---
    LFUCache* cache = lfu_cache_create(capacity);
    if (!cache) {
        fprintf(stderr, "创建缓存失败！\n");
        return 1;
    }
    printf("容量为 %d 的 LFU 缓存已创建。\n\n", capacity);
    
    int done = 0;
    char choice = '\0';

    while (!done) {
        print_menu();
        
        if (scanf(" %c", &choice) != 1) {
            clear_main_input_buffer();
            continue;
        }
        clear_main_input_buffer(); 
        printf("\n");

        switch (choice) {
            case 'p': case 'P':
                handle_put(cache);
                break;
            
            case 'g': case 'G':
                handle_get(cache);
                break;
            
            case 'd': case 'D':
                handle_print(cache);
                break;
            
            case 'q': case 'Q':
                done = 1;
                printf("正在退出...\n");
                break;
            
            default:
                printf("  无效的选择 '%c'，请重试。\n\n", choice);
                break;
        }
    }

    // --- 调用 LFU API ---
    lfu_cache_destroy(cache);
    printf("缓存已释放。再见！\n");

    return 0;
}