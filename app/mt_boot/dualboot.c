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

typedef struct mmc_sdhci_bdev {
  	bdev_t dev; // base device
  
  	struct mmc_device *mmcdev;
  } mmc_sdhci_bdev_t;

static int boot_linux_from_ext2(char *kernel_path, char *ramdisk_path, char *cmdline) 
{
    video_printf("boooting from ext2 kernel: %c, ramdisk: %c, cmdline: %c \n", kernel_path, ramdisk_path, cmdline);
    uint32_t kernel_addr = 0;
	uint32_t ramdisk_addr = 0;
	uint32_t tags_addr = 0;
    //video_printf("getting tag addr \n");
    //tags_addr = get_tags_addr();
    video_printf("getting tag addr ok, getting kernel addr \n");
	kernel_addr = get_kernel_target_addr();
    video_printf("getting kernel addr ok, getting ramdisk addr \n");
	ramdisk_addr = get_ramdisk_target_addr();
    video_printf("getting ramdisk addr ok \n");
	unsigned char *kernel_raw = NULL;
    off_t kernel_raw_size = 0;
	off_t ramdisk_size = 0;
	//off_t dtb_size = 0;

	unsigned int dev_null;
	int ret;

	video_printf("booting from ext2 partition 'system'\n");

	if(fs_mount("/boot", "ext2", "cache")) {
		video_printf("fs_mount failed\n");
		return -1;
	}

	kernel_raw_size = fs_get_file_size(kernel_path);
    video_printf("fs_get_file_size (%s) gave %d\n", kernel_path, kernel_raw_size);
	if(!kernel_raw_size) {
		video_printf("fs_get_file_size (%s) failed\n", kernel_path);
		fs_unmount("/boot");
		return -1;
	}
	kernel_raw = ramdisk_addr; //right where the biggest possible decompressed kernel would end; sure to be out of the way*/

	if(fs_load_file(kernel_path, kernel_raw, kernel_raw_size) < 0) {
		video_printf("failed loading %s at %p\n", kernel_path, kernel_addr);
		fs_unmount("/boot");
		return -1;
	}

	/*if(is_gzip_package(kernel_raw, kernel_raw_size)) {
		ret = decompress(kernel_raw, kernel_raw_size, kernel_addr, ABOOT_FORCE_RAMDISK_ADDR - ABOOT_FORCE_KERNEL64_ADDR, &dev_null, &dev_null);
		if(ret) {
			printf("kernel decompression failed: %d\n", ret);
			fs_unmount("/boot");
			return -1;
		}
	} else {*/
		memmove(kernel_addr, kernel_raw, kernel_raw_size);
	//}

	kernel_raw = NULL; //get rid of dangerous reference to ramdisk_addr before it can do harm

	ramdisk_size = fs_get_file_size(ramdisk_path);
	if (!ramdisk_size) {
		printf("fs_get_file_size (%s) failed\n", ramdisk_path);
		fs_unmount("/boot");
		return -1;
	}

	if(fs_load_file(ramdisk_path, ramdisk_addr, ramdisk_size) < 0) {
		video_printf("failed loading %s at %p\n", ramdisk_path, ramdisk_addr);
		fs_unmount("/boot");
		return -1;
	}
    cmdline_append(get_cmdline());
	/*dtb_size = fs_get_file_size(dtb_path);
	if (!ramdisk_size) {
		printf("fs_get_file_size (%s) failed\n", dtb_path);
		fs_unmount("/boot");
		return -1;
	}

	if(fs_load_file("/boot/msm8937-motorola-cedric.dtb", tags_addr, dtb_size) < 0) {
		printf("failed loading /boot/msm8916-samsung-a3u-eur.dtb at %p\n", tags_addr);
		fs_unmount("/boot");
		return -1;
	}*/

	fs_unmount("/boot");
    video_printf("Booting linux kerneladdr: %d, tags_addr: %d, ramdisk add: %d, ramdisk size: %d \n", kernel_addr, tags_addr, ramdisk_addr, ramdisk_size);
    	boot_linux((void *)kernel_addr,
			(unsigned *)tags_addr,
		   	board_machtype(),
			(void *)ramdisk_addr,
			ramdisk_size);
    video_printf("failed \n");

	return -1; //something went wrong
}


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
        boot_test("/boot/db/ut", "/boot/db/utrd", "bootopt=64S3,32N2,64N2 androidboot.selinux=permissive");
        part_dev_t *dev;
        dev = mt_part_get_device();
        video_printf("type %d \n",dev->blkdev->type);
        int ret;
      //  ret = fs_mount("/boot", "ext2", "hd1p1"); //system
	  //  video_printf("fs_mount ret: %d\n", ret);
        video_printf("bio_dump_devices \n");
        bio_dump_devices();
        video_printf("bio_dump_devices end");
        }
        //}
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

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "main");
    lv_obj_set_event_cb(list_btn, event_handler);

    list_btn = lv_list_add_btn(list1,  LV_SYMBOL_FILE, "Extras");
    lv_obj_set_event_cb(list_btn, event_handler);
    int i;
    for (i = 0; i < num_of_boot_entries; i++) {
        list_btn = lv_list_add_btn(list1,  LV_SYMBOL_FILE, (entry_list + i)->title);
        lv_obj_set_event_cb(list_btn, event_handler);
    }

    //boot_test("/boot/db/ut", "/boot/db/utrd", "bootopt=64S3,32N2,64N2 androidboot.selinux=permissive");

}
