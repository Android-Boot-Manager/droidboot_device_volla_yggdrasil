#include <lib/lvgl.h>
void db_init();
static int sleep_thread(void * arg);
void my_disp_flush(lv_disp_t * disp,
  const lv_area_t * area, lv_color_t * color_p);
bool key_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
