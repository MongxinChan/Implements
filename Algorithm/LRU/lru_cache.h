/*
 * lru_cache.h
 *
 * 一个标准 LRU 缓存的公共接口。
 * 使用“哈希表 + 双向链表”实现 O(1) 的 get 和 put。
 */

#ifndef LRU_CACHE_H
#define LRU_CACHE_H

// --- 公共类型定义 ---

#define KEY_BUFFER_SIZE 256

/**
 * @brief LRU 缓存的不透明结构体。
 * 用户只能持有此类型的指针，不能直接访问其内部成员。
 */
typedef struct LRUCache LRUCache;

// --- 公共 API 函数 ---

/**
 * @brief 创建一个指定容量的 LRU 缓存。
 *
 * @param capacity 缓存的最大容量（必须 > 0）。
 * @return 成功则返回指向新 LRU 缓存的指针，失败（如内存不足）则返回 NULL。
 */
LRUCache* lru_cache_create(int capacity);

/**
 * @brief 销毁一个 LRU 缓存，释放所有相关内存。
 *
 * @param cache 指向 lru_cache_create() 返回的缓存的指针。
 */
void lru_cache_destroy(LRUCache* cache);

/**
 * @brief 从缓存中获取一个值。
 * 访问成功会将该键值对标记为“最近使用”。
 *
 * @param cache 缓存实例。
 * @param key 要查找的键。
 * @return 如果键存在，返回对应的值；如果键不存在，返回 -1。
 */
int lru_cache_get(LRUCache* cache, const char* key);

/**
 * @brief 向缓存中插入或更新一个键值对。
 * 该操作会将键值对标记为“最近使用”。
 * 如果插入导致缓存超出容量，将淘汰“最近最少使用”的条目。
 *
 * @param cache 缓存实例。
 * @param key 要插入/更新的键。
 * @param value 对应的值。
 */
void lru_cache_put(LRUCache* cache, const char* key, int value);

/**
 * @brief 打印缓存的当前状态（从最近使用到最少使用）。
 * (这是一个新添加的辅助函数)
 *
 * @param cache 缓存实例。
 */
void lru_cache_print(LRUCache* cache);

#endif // LRU_CACHE_H