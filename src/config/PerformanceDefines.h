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