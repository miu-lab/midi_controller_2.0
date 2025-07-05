#pragma once

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
extern "C" {
#else
#include <stdint.h>
#include <stddef.h>
#endif

/**
 * @brief Fournit l'accès au pool mémoire DMA pour LVGL
 * @param size Taille demandée (ignorée, retourne toujours le pool complet)
 */
uint8_t* getLvglMemoryPool(size_t size);

/**
 * @brief Retourne la taille du pool mémoire LVGL
 */
size_t getLvglMemoryPoolSize(void);

#ifdef __cplusplus
}
#endif