/*
 * cache_controller.h
 *
 * 封装了与用户交互并操作 LRU 缓存的辅助函数。
 */

#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "lru_cache.h" // 需要 LRUCache 类型

/**
 * @brief 打印主菜单。
 */
void print_menu(void);

/**
 * @brief 处理“Put”操作：提示用户输入键和值，并将其放入缓存。
 *
 * @param cache 要操作的缓存实例。
 */
void handle_put(LRUCache* cache);

/**
 * @brief 处理“Get”操作：提示用户输入键，并从缓存中获取值。
 *
 * @param cache 要操作的缓存实例。
 */
void handle_get(LRUCache* cache);

/**
 * @brief 处理“Print”操作：打印缓存的当前状态。
 *
 * @param cache 要操作的缓存实例。
 */
void handle_print(LRUCache* cache);

#endif // CACHE_CONTROLLER_H