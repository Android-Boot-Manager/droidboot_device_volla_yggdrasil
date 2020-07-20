#include "lvgl/lvgl.h"
#include <kernel/thread.h>
#include <platform/mtk_key.h>
#include <target/cust_key.h>
#include <sys/types.h>
#include <reg.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <lib/fs.h>
#include <video.h>
#include <platform/mt_typedefs.h>
#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_gpt.h>
#include <platform/mtk_wdt.h>
#include <lib/bio.h>
#include <part_interface.h>
#include <block_generic_interface.h>
#include <lib/partition.h>
#include "config.h"

#define MMC_HOST_ID                 0
typedef unsigned int        u32;
int num_of_boot_entries;

struct boot_entry *entry_list;
bool booting=false;
typedef struct mmc_sdhci_bdev {
  	bdev_t dev; // base device
  
  	struct mmc_device *mmcdev;
  } mmc_sdhci_bdev_t;


static ssize_t bdev_read_block_data(struct bdev *_bdev, void *buf, bnum_t block, size_t len, size_t len1)
{
        unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = 0; 
    unsigned int part_id;
   index = partition_get_index("cache");
    ptn = partition_get_offset(index);
    part_id = partition_get_region(index);
    emmc_read(8, ptn+block , (void *)buf, len1);
}
static ssize_t bdev_write_block_data(struct bdev *_bdev, void *buf, bnum_t block, u32 count)
{
    unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = 0; 
    unsigned int part_id;
   index = partition_get_index("userdata");
    ptn = partition_get_offset(index);
    part_id = partition_get_region(index);
    emmc_write(8, ptn+block , (void *)buf,  count*512);
}
static int sleep_thread(void * arg) {
  /*Handle LitlevGL tasks (tickless mode)*/
  while (1) {
    lv_tick_inc(10);
    lv_task_handler();
    thread_sleep(10);
    if(booting){
        break;
    }
  }

  return 0;
}




void cache_init(){
    bio_init();
    struct mmc_card *card;
    card = mmc_get_card(MMC_HOST_ID);
    part_dev_t *dev;
    dev = mt_part_get_device();
	char name[20]= "cache";
	mmc_sdhci_bdev_t *bdev = malloc(sizeof(mmc_sdhci_bdev_t));
	/* set up the base device */
    bio_initialize_bdev(&bdev->dev, name, card->blklen,partition_get_size(33)/card->blklen);
	/* our bits */
	bdev->mmcdev = dev;
	bdev->dev.read_block = bdev_read_block_data;
	/* register it */
	bio_register_device(&bdev->dev);
}
void my_disp_flush(lv_disp_t * disp,
  const lv_area_t * area, lv_color_t * color_p) {
  uint x, y;
  for (y = area -> y1; y <= area -> y2; y++) {
    for (x = area -> x1; x <= area -> x2; x++) {
      video_draw_pixel(x, y, 0xff << 24 | color_p->ch.red << 16 | color_p->ch.green << 8 | color_p->ch.blue ); /* Put a pixel to the display.*/
      
      color_p++;
    }
  }
  mt_disp_update(0, 0, 1080, 2340);
  lv_disp_flush_ready(disp); /* Indicate you are ready with the flushing*/
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        int index = lv_list_get_btn_index(NULL, obj);
        if(index==0){
            boot_linux_from_storage();
        }
        else
        {
            char *linux = malloc(strlen("/boot/") + strlen((entry_list + index)->linux) + 1);
		    char *initrd = malloc(strlen("/boot/") + strlen((entry_list + index)->initrd) + 1);
            strcpy(linux, "/boot/");
		    strcat(linux, (entry_list + index)->linux);
		    strcpy(initrd, "/boot/");
		    strcat(initrd, (entry_list + index)->initrd);
            booting=true;
            boot_linux_ext2(linux, initrd, (entry_list + index)->options);
        }
    }
}

bool key_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
    data->key = LV_KEY_UP;
    if (mtk_detect_key(MT65XX_MENU_SELECT_KEY)){
        data->state = LV_INDEV_STATE_PR;
        return false; /*No buffering now so no more data read*/
    } 
    data->key = LV_KEY_DOWN;
    if (mtk_detect_key(MT65XX_MENU_OK_KEY)){
        data->state = LV_INDEV_STATE_PR;
         return false; /*No buffering now so no more data read*/
    } 
    data->key = LV_KEY_ENTER;
    if (mtk_detect_key(MTK_PMIC_PWR_KEY)){
        video_printf("power");
        data->state = LV_INDEV_STATE_PR;
         return false; /*No buffering now so no more data read*/
    } 
   
}

void db_init()
{
    cache_init();
    parse_boot_entries(&entry_list);
    num_of_boot_entries = get_entry_count();
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

    lv_obj_t * list1 = lv_list_create(win, NULL);
    lv_group_t * g1 = lv_group_create();
    lv_group_add_obj(g1, list1);
    lv_group_focus_obj(list1);
    lv_obj_set_size(list1, 1000, 2100);
    lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_list_set_anim_time(list1, 0);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);      /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = key_read;
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
    lv_indev_set_group(my_indev, g1);
    
    lv_obj_t * list_btn;

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, entry_list->title);
    lv_obj_set_event_cb(list_btn, event_handler);
    int i;
    for (i = 1; i < num_of_boot_entries; i++) {
        list_btn = lv_list_add_btn(list1,  LV_SYMBOL_FILE, (entry_list + i)->title);
        lv_obj_set_event_cb(list_btn, event_handler);
    }
    list_btn = lv_list_add_btn(list1,  LV_SYMBOL_FILE, "Extras");
    lv_obj_set_event_cb(list_btn, event_handler);
    

}
