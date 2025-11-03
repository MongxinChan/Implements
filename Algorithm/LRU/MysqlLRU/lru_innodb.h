/*
 * lru_innodb.h
 *
 * 演示 InnoDB 优化的 LRU 缓存接口。
 * Key 类型为 int (简化演示)。
 */

#ifndef LRU_INNODB_H
#define LRU_INNODB_H

// (确保在 lru_innodb.c 中包含 <time.h> 和 #define _POSIX_C_SOURCE)
// 需要一个高精度时间源
typedef struct LRUCache LRUCache;

/**
 * @brief 创建一个 InnoDB 优化的 LRU 缓存。
 *
 * @param capacity 总容量。
 * @param old_list_ratio 老生代所占的比例 (例如 0.375 表示 3/8)。
 * @param access_delay_ms 'Old' 区数据晋升到 'New' 区所需的最小停留时间 (毫秒)。
 * @return 成功则返回缓存指针，失败返回 NULL。
 */
LRUCache* lru_cache_create(int capacity, double old_list_ratio, unsigned long access_delay_ms);

void lru_cache_destroy(LRUCache* cache);

int lru_cache_get(LRUCache* cache, int key);

void lru_cache_put(LRUCache* cache, int key, int value);

/**
 * @brief 打印缓存的详细状态，区分 'New' 和 'Old' 区。
 */
void lru_cache_print(LRUCache* cache);

#endif // LRU_INNODB_H