/*
 * lfu_controller.h
 *
 * 封装了与用户交互并操作 LFU 缓存的辅助函数。
 */

#ifndef LFU_CONTROLLER_H
#define LFU_CONTROLLER_H

#include "lfu_standard.h" // 包含 LFU 版本的头文件

/**
 * @brief 打印主菜单。
 */
void print_menu(void);

/**
 * @brief 处理“Put”操作。
 */
void handle_put(LFUCache* cache);

/**
 * @brief 处理“Get”操作。
 */
void handle_get(LFUCache* cache);

/**
 * @brief 处理“Print”操作。
 */
void handle_print(LFUCache* cache);

#endif // LFU_CONTROLLER_H