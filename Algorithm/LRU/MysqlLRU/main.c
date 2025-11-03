/*
 * main.c
 *
 * 交互式 InnoDB LRU 缓存演示的主程序。
 */

#include <stdio.h>
#include "lru_innodb.h"
#include "innodb_controller.h"

// 辅助函数：清除输入缓冲区
void clear_main_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int capacity = 0;
    double old_ratio = 0.0;
    unsigned long delay_ms = 0;

    // --- 1. 获取 InnoDB 参数 ---
    while (capacity <= 0) {
        printf("请输入 LRU 缓存的容量 (必须 > 0): ");
        if (scanf("%d", &capacity) != 1) {
            clear_main_input_buffer();
        }
    }
    clear_main_input_buffer(); // 清除换行符

    while (old_ratio <= 0.0 || old_ratio >= 1.0) {
        printf("请输入 Old 区比例 (0.0 到 1.0, 例如 0.375): ");
        if (scanf("%lf", &old_ratio) != 1) { // %lf 用于 double
            clear_main_input_buffer();
        }
    }
    clear_main_input_buffer();

    printf("请输入晋升延迟时间 (毫秒, 例如 2000): ");
    while (scanf("%lu", &delay_ms) != 1) { // %lu 用于 unsigned long
        printf("无效输入，请重新输入: ");
        clear_main_input_buffer();
    }
    clear_main_input_buffer();

    LRUCache* cache = lru_cache_create(capacity, old_ratio, delay_ms);
    if (!cache) {
        fprintf(stderr, "创建缓存失败！\n");
        return 1;
    }
    printf("缓存已创建。\n");
    printf("(Capacity: %d, Old Ratio: %.3f, Delay: %lu ms)\n\n", capacity, old_ratio, delay_ms);
    
    int done = 0;
    char choice = '\0';

    // --- 2. 主循环 ---
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
            
            case 'w': case 'W': // <-- 新增的 Wait 选项
                handle_wait();
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

    lru_cache_destroy(cache);
    printf("缓存已释放。再见！\n");

    return 0;
}