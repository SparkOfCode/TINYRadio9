#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_NEEDLE_RED
#define LV_ATTRIBUTE_IMAGE_NEEDLE_RED
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_NEEDLE_RED uint8_t NEEDLE_RED_map[] = {
  0xe3, 0xc8, 0xa2, 0xf0, 0xa2, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x62, 0xf8, 0x62, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0xa3, 0xf8, 0xa3, 0xf8, 0xa3, 0xf8, 0xa3, 0xf8, 0xe4, 0xf0, 0x26, 0xea, 0x02, 0xfc, 
  0xc3, 0x58, 0x69, 0xca, 0x4d, 0xfb, 0x6d, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8d, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0xae, 0xfb, 0x8d, 0xfb, 0xae, 0xfb, 0x8e, 0xfb, 0xae, 0xfb, 0x8e, 0xfb, 0xae, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0xae, 0xfb, 0xae, 0xfb, 0xae, 0xfb, 0x8e, 0xfb, 0xae, 0xfb, 0x8e, 0xfb, 0x8e, 0xfb, 0x8d, 0xfb, 0xae, 0xfb, 0xcf, 0xfb, 0xcf, 0xfb, 0x0b, 0xfb, 0x22, 0xf9, 
  0xe3, 0xd0, 0x82, 0xf0, 0xa2, 0xf8, 0xc3, 0xf8, 0xe3, 0xf8, 0xe3, 0xf8, 0xe3, 0xf8, 0xc3, 0xf8, 0xa3, 0xf8, 0xa2, 0xf8, 0xa2, 0xf8, 0xa2, 0xf8, 0x82, 0xf8, 0x82, 0xf8, 0x62, 0xf8, 0x61, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x61, 0xf8, 0x61, 0xf8, 0x61, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x41, 0xf8, 0x21, 0xf8, 0x62, 0xf8, 0xc3, 0xf8, 0x82, 0xf0, 0x23, 0xe1, 0x41, 0xfc, 
};

const lv_image_dsc_t NEEDLE_RED = {
  .header.cf = LV_COLOR_FORMAT_RGB565,
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.w = 40,
  .header.h = 3,
  .data_size = 120 * 2,
  .data = NEEDLE_RED_map,
};
