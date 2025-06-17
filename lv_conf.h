/**
 * @file lv_conf.h
 * LVGL Configuration for Teensy 4.1 (i.MX RT1062) MIDI Controller
 * Minimal, optimized configuration focusing only on what's actually used
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   BASIC SETTINGS
 *====================*/

#define LV_COLOR_DEPTH 16

/*=========================
   MEMORY & PERFORMANCE
 *=========================*/

/* Use Teensy's optimized C library */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING    LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_CLIB

/* Standard includes */
#define LV_STDINT_INCLUDE       <stdint.h>
#define LV_STDDEF_INCLUDE       <stddef.h>
#define LV_STDBOOL_INCLUDE      <stdbool.h>
#define LV_INTTYPES_INCLUDE     <inttypes.h>
#define LV_LIMITS_INCLUDE       <limits.h>
#define LV_STDARG_INCLUDE       <stdarg.h>

/*====================
   DISPLAY & REFRESH
 *====================*/

#define LV_DEF_REFR_PERIOD  16      /* 60fps optimal for MIDI controller */
#define LV_DPI_DEF 130               /* Correct DPI for ILI9341 240x320 */

/* Teensy 4.1 tick configuration */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE <Arduino.h>
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*====================
   RENDERING
 *====================*/

#define LV_USE_OS   LV_OS_NONE

/* Optimize buffers for Teensy 4.1 performance */
#define LV_DRAW_BUF_STRIDE_ALIGN                1
#define LV_DRAW_BUF_ALIGN                       4
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (12 * 1024)   /* 12KB for performance */
#define LV_DRAW_THREAD_STACK_SIZE        (6 * 1024)    /* 6KB stack */

/* Software rendering with Cortex-M7 optimizations */
#define LV_USE_DRAW_SW 1
#if LV_USE_DRAW_SW == 1
    /* Color formats - only what we need */
    #define LV_DRAW_SW_SUPPORT_RGB565               1  /* ILI9341 native */
    #define LV_DRAW_SW_SUPPORT_RGB888               1  /* Gradients */
    
    /* Performance optimizations */
    #define LV_USE_DRAW_ARM2D_SYNC                  1  /* ARM-2D acceleration */
    #define LV_DRAW_SW_COMPLEX                      1  /* Arcs, shadows, etc */
    #define LV_DRAW_SW_SHADOW_CACHE_SIZE            4  /* Small shadow cache */
    #define LV_DRAW_SW_CIRCLE_CACHE_SIZE            8  /* Circle cache */
    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS        1  /* Better UI gradients */
#endif

/*====================
   FEATURES
 *====================*/

/* Minimal logging and debugging */
#define LV_USE_LOG 0

/* Basic asserts only */
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_ASSERT_HANDLER while(1);

/* Performance optimizations */
#define LV_GRADIENT_MAX_STOPS       4     /* Better gradients */
#define LV_COLOR_MIX_ROUND_OFS      128   /* Better color mixing on M7 */
#define LV_OBJ_STYLE_CACHE          1     /* Style cache for performance */

/*====================
   FONTS
 *====================*/

/* Only the font we actually use */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*====================
   TEXT
 *====================*/

#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_)]}"

/*====================
   WIDGETS - MIDI Controller essentials only
 *====================*/

/* Core widgets for MIDI interface */
#define LV_USE_ARC        1    /* Encoders, knobs */
#define LV_USE_BAR        1    /* Level meters, progress */
#define LV_USE_BUTTON     1    /* UI buttons */
#define LV_USE_LABEL      1    /* Text display */
#define LV_USE_LINE       1    /* Basic drawing */

/* Everything else disabled */
#define LV_USE_ANIMIMG    0
#define LV_USE_BUTTONMATRIX  0
#define LV_USE_CALENDAR   0
#define LV_USE_CANVAS     0
#define LV_USE_CHART      0
#define LV_USE_CHECKBOX   0
#define LV_USE_DROPDOWN   0
#define LV_USE_IMAGE      0
#define LV_USE_IMAGEBUTTON     0
#define LV_USE_KEYBOARD   0
#define LV_USE_LED        0
#define LV_USE_LIST       0
#define LV_USE_MENU       0
#define LV_USE_MSGBOX     0
#define LV_USE_ROLLER     0
#define LV_USE_SCALE      0
#define LV_USE_SLIDER     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    0
#define LV_USE_SWITCH     0
#define LV_USE_TABLE      0
#define LV_USE_TABVIEW    0
#define LV_USE_TEXTAREA   0
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0

/* Label configuration */
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 0
    #define LV_LABEL_LONG_TXT_HINT 0
#endif

/*====================
   THEMES
 *====================*/

/* Dark theme for professional MIDI look */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 1
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_SIMPLE 1      /* Fallback */

/*====================
   LAYOUTS
 *====================*/

#define LV_USE_FLEX 1     /* Useful for MIDI layouts */
#define LV_USE_GRID 0     /* Not needed */

/*====================
   TEENSY 4.1 SPECIFIC
 *====================*/

/* Memory alignment for Cortex-M7 */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4
#define LV_ATTRIBUTE_MEM_ALIGN __attribute__((aligned(4)))

/* Use Teensy's memory sections */
#define LV_ATTRIBUTE_LARGE_CONST __attribute__((section(".rodata")))
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY DMAMEM
#define LV_ATTRIBUTE_FAST_MEM __attribute__((section(".fastrun")))

/* Disable float/matrix (not needed for MIDI) */
#define LV_USE_FLOAT            0
#define LV_USE_MATRIX           0

/* Keep private API access disabled */
#define LV_USE_PRIVATE_API      0

#endif /*LV_CONF_H*/