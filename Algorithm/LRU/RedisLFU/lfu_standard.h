/*
 * lfu_standard.h
 *
 * 一个 "最近最不常用" (LFU) 缓存的公共接口。
 * 使用两个哈希表和多个双向链表实现 O(1) 的 get 和 put。
 *
 * (为简化算法演示，Key 和 Value 均使用 int 类型)
 */

#ifndef LFU_STANDARD_H
#define LFU_STANDARD_H

typedef struct LFUCache LFUCache;

/**
 * @brief 创建一个指定容量的 LFU 缓存。
 *
 * @param capacity 缓存的最大容量 (必须 > 0)。
 * @return 成功则返回指针，失败返回 NULL。
 */
LFUCache* lfu_cache_create(int capacity);

/**
 * @brief 销毁 LFU 缓存，释放所有相关内存。
 *
 * @param cache 缓存实例。
 */
void lfu_cache_destroy(LFUCache* cache);

/**
 * @brief 从缓存中获取一个值。
 * 访问成功会使该键的频率 +1。
 *
 * @param cache 缓存实例。
 * @param key 要查找的键。
 * @return 找到则返回对应的值；否则返回 -1。
 */
int lfu_cache_get(LFUCache* cache, const char* key);

/**
 * @brief 向缓存中插入或更新一个键值对。
 * 如果是新插入，频率为 1。
 * 如果是更新，频率 +1。
 *
 * @param cache 缓存实例。
 * @param key 要插入/更新的键。
 * @param value 对应的值。
 */
void lfu_cache_put(LFUCache* cache, const char* key, int value);

/**
 * @brief (辅助) 打印 LFU 缓存的详细分层状态。
 */
void lfu_cache_print(LFUCache* cache);

#endif // LFU_STANDARD_H