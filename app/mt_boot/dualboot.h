#include "lvgl/lvgl.h"
void db_init();
static int sleep_thread(void * arg);
void my_disp_flush(lv_disp_t * disp,
  const lv_area_t * area, lv_color_t * color_p);
