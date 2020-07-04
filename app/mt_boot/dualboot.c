#include "lvgl/lvgl.h"
#include <kernel/thread.h>
static int sleep_thread(void * arg) {
  /*Handle LitlevGL tasks (tickless mode)*/
  while (1) {
    lv_tick_inc(5);
    lv_task_handler();
    thread_sleep(5);
  }

  return 0;
}

void my_disp_flush(lv_disp_t * disp,
  const lv_area_t * area, lv_color_t * color_p) {
  uint x, y;
  for (y = area -> y1; y <= area -> y2; y++) {
    for (x = area -> x1; x <= area -> x2; x++) {
      video_put_pixel(x, y, 0xff << 24 | color_p->ch.red << 16 | color_p->ch.green << 8 | color_p->ch.blue ); /* Put a pixel to the display.*/
      
      color_p++;
    }
  }
  mt_disp_update(0, 0, 1080, 2340);
  lv_disp_flush_ready(disp); /* Indicate you are ready with the flushing*/
}


void db_init()
{
    thread_t *thr;
    video_clean_screen();
    lv_init();
    thr=thread_create("sleeper", & sleep_thread, NULL, HIGHEST_PRIORITY, 16*1024);
    thread_resume(thr);
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf[LV_HOR_RES_MAX * 10]; /*Declare a buffer for 10 lines*/
    lv_disp_buf_init( & disp_buf, buf, NULL, LV_HOR_RES_MAX * 10); /*Initialize the display buffer*/
    lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
    lv_disp_drv_init( & disp_drv); /*Basic initialization*/
    disp_drv.flush_cb = my_disp_flush; /*Set your driver function*/
    disp_drv.buffer = & disp_buf; /*Assign the buffer to the display*/
    lv_disp_drv_register( & disp_drv); /*Finally register the driver*/

    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Boot menu"); 
}
