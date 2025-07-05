/**
 * @file PerformanceDefines.h
 * @brief Définitions C pour lv_conf.h
 * 
 * Fichier C pur (pas de namespace) utilisable par lv_conf.h
 */
#pragma once

/* Refresh rate - compatible C */
#define LVGL_REFRESH_RATE_HZ 120
#define LVGL_REFRESH_PERIOD_MS (1000 / LVGL_REFRESH_RATE_HZ)

/* LVGL Memory Configuration */
#define LVGL_USE_DMA_MEMORY 1                    /**< 1: Use DMA memory, 0: Use internal memory */
#define LVGL_MEMORY_POOL_SIZE_KB 64              /**< Pool size in KB (32, 64, 128, etc.) */
#define LVGL_MEMORY_POOL_SIZE (LVGL_MEMORY_POOL_SIZE_KB * 1024)  /**< Pool size in bytes */