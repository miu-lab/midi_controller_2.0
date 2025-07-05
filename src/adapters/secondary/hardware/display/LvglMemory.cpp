#include "LvglMemory.hpp"
#include <Arduino.h>
#include "config/PerformanceDefines.h"

// Pool mémoire pour LVGL - DMA ou interne selon configuration
#if LVGL_USE_DMA_MEMORY
    DMAMEM static uint8_t lvgl_memory_pool[LVGL_MEMORY_POOL_SIZE];
#else
    static uint8_t lvgl_memory_pool[LVGL_MEMORY_POOL_SIZE];
#endif

extern "C" {
    uint8_t* getLvglMemoryPool(size_t size) {
        (void)size; // Ignore le paramètre size
        return lvgl_memory_pool;
    }

    size_t getLvglMemoryPoolSize(void) {
        return sizeof(lvgl_memory_pool);
    }
}