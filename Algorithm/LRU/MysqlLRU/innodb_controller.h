/*
 * cache_controller.h
 *
 * 封装了与用户交互并操作 LRU 缓存的辅助函数。
 */

#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "lru_innodb.h" // 包含 InnoDB 版本的头文件

/**
 * @brief 打印主菜单。
 */
void print_menu(void);

/**
 * @brief 处理“Put”操作。
 */
void handle_put(LRUCache* cache);

/**
 * @brief 处理“Get”操作。
 */
void handle_get(LRUCache* cache);

/**
 * @brief 处理“Print”操作。
 */
void handle_print(LRUCache* cache);

/**
 * @brief 处理“Wait”操作：让程序暂停 N 秒，以测试延迟晋升。
 */
void handle_wait(void);

#endif // CACHE_CONTROLLER_H