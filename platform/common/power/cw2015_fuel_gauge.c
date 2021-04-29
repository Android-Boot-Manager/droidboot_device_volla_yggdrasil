#include <platform/mt_typedefs.h>
#include <platform/mt_i2c.h>   
#include <platform/mt_pmic.h>
#include "cw2015_fuel_gauge.h"
#include <platform/timer.h>
#include <platform/mt_pmic.h>
#if defined(CONFIG_MTK_CW2015_SUPPORT_OF)
#include <libfdt.h>
#include <lk_builtin_dtb.h>
#endif

#define CWFG_ENABLE_LOG 1 //CHANGE   Customer need to change this for enable/disable log
#define CWFG_I2C_BUSNUM 0 //CHANGE   Customer need to change this number according to the principle of hardware
/*
#define USB_CHARGING_FILE "/sys/class/power_supply/usb/online" // Chaman
#define DC_CHARGING_FILE "/sys/class/power_supply/ac/online"
*/

#define REG_VERSION             0x0
#define REG_VCELL               0x2
#define REG_SOC                 0x4
#define REG_SOC1                0x5
#define REG_RRT_ALERT           0x6
#define REG_CONFIG              0x8
#define REG_MODE                0xA
#define REG_VTEMPL              0xC
#define REG_VTEMPH              0xD
#define REG_BATINFO             0x10

#define MODE_SLEEP_MASK         (0x3<<6)
#define MODE_SLEEP              (0x3<<6)
#define MODE_NORMAL             (0x0<<6)
#define MODE_QUICK_START        (0x3<<4)
#define MODE_RESTART            (0xf<<0)

#define CONFIG_UPDATE_FLG       (0x1<<1)
#define ATHD                    (0x0<<3)        // ATHD = 0%

#define BATTERY_UP_MAX_CHANGE   300*1000            // The time for add capacity when charging //420s
#define BATTERY_DOWN_MAX_CHANGE 120*1000
#define BATTERY_JUMP_TO_ZERO    30*1000
#define BATTERY_CAPACITY_ERROR  40*1000
#define BATTERY_CHARGING_ZERO   1800*1000

#define CHARGING_ON 1
#define NO_CHARGING 0


#define cw_printk(flg, fmt, arg...)        \
	({                                    \
		if(flg >= CWFG_ENABLE_LOG){\
			printk("FG_CW2015 : %s : " fmt, __FUNCTION__ ,##arg);\
		}else{}                           \
	})     //need check by Chaman

//#define cw_printk(fmt, arg...)  printk("FG_CW2015 : %s : " fmt, __FUNCTION__ ,##arg);

#define CWFG_NAME "cw2015"
#define SIZE_BATINFO    64
#define CW2015_SLAVE_ADDR_WRITE   0xC4 //0x62
#define CW2015_SLAVE_ADDR_READ    0xC5 //0x63
#define CW2015_I2C_ID	I2C0
#define  LOW_TEMPERATURE_POWER_OFF 0xf0
static struct mt_i2c_t cw2015_i2c;


#if defined(CONFIG_MTK_CW2015_SUPPORT_OF)
static kal_uint16 cw2015_i2c_idinfo = I2C6;
static unsigned char config_info[SIZE_BATINFO] = {0};
#else
static unsigned char config_info[SIZE_BATINFO] = {
    #include "profile_BRZH301_K606Q_3300mAhLT_180117-1.i"
};
#endif



struct cw_battery {
    struct i2c_client *client;
    int charger_mode;
    int capacity;
    int voltage;
    int status;
    int time_to_empty;
	int change;
    //int alt;
};

int cw2015_write_byte(kal_uint8 addr, kal_uint8 value)
{
    int ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

#if defined(CONFIG_MTK_CW2015_SUPPORT_OF)
    cw2015_i2c.id = cw2015_i2c_idinfo;
#else
    cw2015_i2c.id = CW2015_I2C_ID;
#endif
	
    /* Since i2c will left shift 1 bit, we need to set CW2015 I2C address to >>1 */
    cw2015_i2c.addr = (CW2015_SLAVE_ADDR_WRITE >> 1);
    cw2015_i2c.mode = ST_MODE;
    cw2015_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&cw2015_i2c, write_data, len);
    if(I2C_OK != ret_code)
        dprintf(CRITICAL, "%s: i2c_write: ret_code: %d\n", __func__, ret_code);

	//dprintf(CRITICAL,"cw2015 %2x = %2x\n", addr, value);
    return ret_code;
}

int cw2015_read_byte(kal_uint8 addr, kal_uint8 *dataBuffer) 
{
    int ret_code = I2C_OK;
    kal_uint16 len;
    *dataBuffer = addr;

#if defined(CONFIG_MTK_CW2015_SUPPORT_OF)
    cw2015_i2c.id = cw2015_i2c_idinfo;
#else
    cw2015_i2c.id = CW2015_I2C_ID;
#endif

    /* Since i2c will left shift 1 bit, we need to set CW2015 I2C address to >>1 */
    cw2015_i2c.addr = (CW2015_SLAVE_ADDR_READ >> 1);
    cw2015_i2c.mode = ST_MODE;
    cw2015_i2c.speed = 100;
    len = 1;

    ret_code = i2c_write_read(&cw2015_i2c, dataBuffer, len, len);
	//dprintf(CRITICAL,"cw2015 read %2x = %2x ret_code=%d\n", addr,  ret_code);
    if(I2C_OK != ret_code)
        dprintf(CRITICAL, "%s: i2c_read: ret_code: %d\n", __func__, ret_code);
	
	//dprintf(CRITICAL,"cw2015 read %2x = %2x\n", addr,  dataBuffer[0]);

    return ret_code;
}

/*CW2015 update profile function, Often called during initialization*/
int cw_update_config_info(void)
{
    int ret;
    unsigned char reg_val;
	unsigned char reg_val1;
    int i;
    unsigned char reset_val;

    dprintf(CRITICAL,"[FGADC] test config_info = 0x%x\n",config_info[0]);

    // make sure no in sleep mode
    ret = cw2015_read_byte(REG_MODE, &reg_val);
    if(ret < 0) {
		dprintf(CRITICAL,"cw2015 err [FGADC] test config_info = 0x%x\n",ret);
        return ret;
    }

    reset_val = reg_val;
    if((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP) {
		dprintf(CRITICAL,"cw2015 err [FGADC] test MODE_SLEEP = 0x%x\n",ret);
        return -1;
    }

    // update new battery info
    for (i = 0; i < SIZE_BATINFO; i++) {
        ret = cw2015_write_byte(REG_BATINFO + i, config_info[i]);
        if(ret < 0) 
			return ret;
    }
	ret = cw2015_read_byte(REG_CONFIG, &reg_val);
	if(ret < 0) 
		return ret;
    reg_val |= CONFIG_UPDATE_FLG;   // set UPDATE_FLAG
    //reg_val &= 0x07;                // clear ATHD
    //reg_val |= ATHD;                // set ATHD
    ret = cw2015_write_byte(REG_CONFIG, reg_val);
    if(ret < 0) 
		return ret;
    // read back and check
    ret = cw2015_read_byte(REG_CONFIG, &reg_val);
    if(ret < 0) {
        return ret;
    }

    if (!(reg_val & CONFIG_UPDATE_FLG)) {
		dprintf(CRITICAL,"Error: The new config set fail\n");
		//return -1;
    }
#if 0
    if ((reg_val & 0xf8) != ATHD) {
		dprintf(CRITICAL,"Error: The new ATHD set fail\n");
		//return -1;
    }
#endif
	ret = cw2015_read_byte(REG_MODE, &reset_val);
    if(ret < 0) {
		dprintf(CRITICAL,"cw2015 err [FGADC] test config_info = 0x%x\n",ret);
        return ret;
    }
	dprintf(CRITICAL,"cw2015 reset_val=%x\n", reset_val);
    // reset
    reset_val &= ~(MODE_RESTART);
    reg_val = reset_val | MODE_RESTART;
    ret = cw2015_write_byte(REG_MODE, reg_val);
    if(ret < 0) return ret;

    mdelay(10);
    
    ret = cw2015_write_byte(REG_MODE, reset_val);
    if(ret < 0) 
		return ret;

	mdelay(120);
    return 0;
}

//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-start
#if defined(CONFIG_MTK_CW2015_BATTERY_ID_AUXADC)
static unsigned int chr_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                 const char *name)
{
	const void *data = NULL;
	int len = 0;

	data = fdt_getprop(fdt, nodeoffset, name, &len);
	if (len > 0 && data)
		return fdt32_to_cpu(*(unsigned int *)data);
	else
		return 0;
}
#endif
//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-end

/*CW2015 init function, Often called during initialization*/
int cw2015_init(void)
{
    int ret;
    int i;
    unsigned char reg_val = MODE_SLEEP;
	unsigned char reg_val00;

#if defined(CONFIG_MTK_CW2015_SUPPORT_OF)
	const void *data = NULL;
	int len = 0;
	int offset = 0;

// prize-add-sunshuai-2015 Multi-Battery Solution-20200222-start
#if defined(CONFIG_MTK_CW2015_BATTERY_ID_AUXADC)
	int val =0;
	int Voltiage_cali =0;
#endif
// prize-add-sunshuai-2015 Multi-Battery Solution-20200222-end
	
	//void *kernel_fdt = get_kernel_fdt();
	
//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-start
#if defined(CONFIG_MTK_CW2015_BATTERY_ID_AUXADC)
    	//get i2c_id from dts
	offset = fdt_node_offset_by_compatible(get_lk_overlayed_dtb(),-1,"cellwise,cw2015");
	if (offset < 0) {
		dprintf(CRITICAL, "[cw2015] Failed to find cellwise,cw2015 in dtb\n");
	}else{		
        val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_first_startrange");
		if (val)
			cw2015fuelguage.bat_first_startrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_first_endrange");
		if (val)
			cw2015fuelguage.bat_first_endrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_second_startrange");
		if (val)
			cw2015fuelguage.bat_second_startrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_second_endrange");
		if (val)
			cw2015fuelguage.bat_second_endrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_third_startrange");
		if (val)
			cw2015fuelguage.bat_third_startrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_third_endrange");
		if (val)
			cw2015fuelguage.bat_third_endrange = val;
		
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "bat_channel_num");
		if (val)
			cw2015fuelguage.bat_channel_num = val;

	    dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_first_startrange =%d \n",cw2015fuelguage.bat_first_startrange);
		dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_first_endrange =%d \n",cw2015fuelguage.bat_first_endrange);
		dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_second_startrange =%d \n",cw2015fuelguage.bat_second_startrange);
	    dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_second_endrange =%d \n",cw2015fuelguage.bat_second_endrange);
		dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_third_startrange =%d \n",cw2015fuelguage.bat_third_startrange);
	    dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_third_endrange =%d \n",cw2015fuelguage.bat_third_endrange);
		dprintf(CRITICAL, "[cw2015] cw2015fuelguage.bat_channel_num =%d \n",cw2015fuelguage.bat_channel_num);

        ret= IMM_GetOneChannelValue_Cali(cw2015fuelguage.bat_channel_num, &Voltiage_cali);
		if (ret < 0) {
			dprintf(1,"cw2015_init [adc_driver]: get channel[%d] cali voltage error\n",cw2015fuelguage.bat_channel_num);
		} else {
			dprintf(1,"cw2015_init [adc_driver]: get channel[%d] cali_voltage =%d\n",cw2015fuelguage.bat_channel_num,Voltiage_cali);
		}

		if(Voltiage_cali > cw2015fuelguage.bat_first_startrange && Voltiage_cali < cw2015fuelguage.bat_first_endrange)
			cw2015fuelguage.bat_id = 0;
		else if(Voltiage_cali > cw2015fuelguage.bat_second_startrange && Voltiage_cali < cw2015fuelguage.bat_second_endrange)
			cw2015fuelguage.bat_id = 1;
		else if(Voltiage_cali > cw2015fuelguage.bat_third_startrange && Voltiage_cali < cw2015fuelguage.bat_third_endrange)
			cw2015fuelguage.bat_id = 2;
		else{
			cw2015fuelguage.bat_id = 3;
			dprintf(CRITICAL, "[cw2015] cw2015_init did not find Curve corresponding to the battery ,use default Curve");
		}
		
		dprintf(CRITICAL, "[cw2015]  Curve name %s",fuelguage_name[cw2015fuelguage.bat_id]);
		
		//batinfo
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, fuelguage_name[cw2015fuelguage.bat_id], &len);
		if (len > 0 && data){
			dprintf(CRITICAL, "[cw2015] batinfo len(%d)\n",len);
			if (len != SIZE_BATINFO){
				dprintf(CRITICAL, "[cw2015] get bat batinfo fail len(%d) != SIZE_BATINFO(%d)\n",len,SIZE_BATINFO);
			}else{
				memcpy(config_info,data,len);
				dprintf(CRITICAL,"[cw2015]batinfo \n");
				for (i=0;i<SIZE_BATINFO;i++){
					dprintf(CRITICAL,"cw2015_init lk config_info[%d] =%x\n",i,config_info[i]);
				}
				dprintf(CRITICAL,"\n");
			}
		}
		//i2c id
		offset = fdt_parent_offset(get_lk_overlayed_dtb(),offset);
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, "id", &len);
		if (len > 0 && data){
			cw2015_i2c_idinfo = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[cw2015] i2c_id(%d)\n",cw2015_i2c_idinfo);
		}
	}
#else
	//get i2c_id from dts
	offset = fdt_node_offset_by_compatible(get_lk_overlayed_dtb(),-1,"cellwise,cw2015");
	if (offset < 0) {
		dprintf(CRITICAL, "[cw2015] Failed to find cellwise,cw2015 in dtb\n");
	}else{
		//batinfo
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, "batinfo", &len);
		if (len > 0 && data){
			dprintf(CRITICAL, "[cw2015] batinfo len(%d)\n",len);
			if (len != SIZE_BATINFO){
				dprintf(CRITICAL, "[cw2015] get bat batinfo fail len(%d) != SIZE_BATINFO(%d)\n",len,SIZE_BATINFO);
			}else{
				memcpy(config_info,data,len);
				dprintf(CRITICAL,"[cw2015]batinfo ");
				for (i=0;i<SIZE_BATINFO;i++){
					dprintf(CRITICAL,"cw2015_init lk config_info[%d] =%x\n",i,config_info[i]);
				}
				dprintf(CRITICAL,"\n");
			}
		}
		//i2c id
		offset = fdt_parent_offset(get_lk_overlayed_dtb(),offset);
		data = fdt_getprop(get_lk_overlayed_dtb(), offset, "id", &len);
		if (len > 0 && data){
			cw2015_i2c_idinfo = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[cw2015] i2c_id(%d)\n",cw2015_i2c_idinfo);
		}
	}
#endif
//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-end

#endif

	reg_val = 0x00;
    ret = cw2015_write_byte(REG_MODE, reg_val);
	ret = cw2015_read_byte(REG_MODE, &reg_val00);
	dprintf(CRITICAL,"cw2015 REG_MODE (0x%x)\n", reg_val00);
	if (ret < 0) 
	{
		dprintf(CRITICAL,"cw2015 MODE_NORMAL (0x%x)\n", ret);
        return ret;
	}
	
	ret = cw2015_read_byte(0x00, &reg_val00);
	dprintf(CRITICAL,"cw2015 get config (0x%x)\n", reg_val00);

    ret = cw2015_read_byte(REG_CONFIG, &reg_val);
    if (ret < 0)
    	return ret;

#if 0
    if ((reg_val & 0xf8) != ATHD) {
        reg_val &= 0x07;    /* clear ATHD */
        reg_val |= ATHD;    /* set ATHD */
        ret = cw2015_write_byte(REG_CONFIG, reg_val);
        if (ret < 0)
            return ret;
    }
#endif
    ret = cw2015_read_byte(REG_CONFIG, &reg_val);
    if (ret < 0) 
        return ret;

    if (!(reg_val & CONFIG_UPDATE_FLG)) {
		dprintf(CRITICAL,"update config flg is true, need update config\n");
        ret = cw_update_config_info();

		dprintf(CRITICAL,"cw2015 update %d\n", ret);
        if (ret < 0) {
			dprintf(CRITICAL,"%s : update config fail\n", __func__);
            return ret;
        }
    } else {
    	for(i = 0; i < SIZE_BATINFO; i++) { 
	        ret = cw2015_read_byte((REG_BATINFO + i), &reg_val);
	        if (ret < 0)
	        	return ret;
	                        
	        if (config_info[i] != reg_val)
	            break;
        }
        if (i != SIZE_BATINFO) {
			dprintf(CRITICAL,"cw2015 config didn't match...\n");
        	ret = cw_update_config_info();
            if (ret < 0){
                return ret;
            }
        }
    }
	
	
    for (i = 0; i < 30; i++) {
        ret = cw2015_read_byte(REG_SOC, &reg_val);
        if (ret < 0)
            return ret;
        else if (reg_val <= 100) 
		{
			
            break;
		}
        mdelay(120);
    }
	dprintf(CRITICAL,"cw2015 read REG_SOC(%d)(0x%x)\n",i,reg_val);
    if (i >= 30 ){
    	 reg_val = MODE_SLEEP;
         ret = cw2015_write_byte(REG_MODE, reg_val);
         dprintf(CRITICAL,"cw2015 input unvalid power error, cw2015 join sleep mode\n");
         return -1;
    } 

    return 0;
}

/*static int cw_por(struct cw_battery *cw_bat)
{
	int ret;
	unsigned char reset_val;
	
	reset_val = MODE_SLEEP; 			  
	ret = cw_write(cw_bat->client, REG_MODE, &reset_val);
	if (ret < 0)
		return ret;
	reset_val = MODE_NORMAL;
	msleep(10);
	ret = cw_write(cw_bat->client, REG_MODE, &reset_val);
	if (ret < 0)
		return ret;
	ret = cw_init(cw_bat);
	if (ret) 
		return ret;
	return 0;
}*/
static int cw_quickstart(void)
{
        int ret = 0;
        u8 reg_val = MODE_QUICK_START;

        ret = cw2015_write_byte(REG_MODE, reg_val);     //(MODE_QUICK_START | MODE_NORMAL));  // 0x30
        if(ret < 0) {
                dprintf(CRITICAL, "cw2015 Error quick start1\n");
                return ret;
        }
        
        reg_val = MODE_NORMAL;
        ret = cw2015_write_byte(REG_MODE, reg_val);
        if(ret < 0) {
                dprintf(CRITICAL,"cw2015 Error quick start2\n");
                return ret;
        }
        return 1;
}

int cw2015_get_capacity(void)
{
	int cw_capacity = 200;
	int ret,i;
	unsigned char reg_val;
	int pmic_temp = -200;
	unsigned char temp_val;
	unsigned char temp_val1;
	
	ret = cw2015_read_byte(REG_SOC, &reg_val);
	if (ret < 0){
		dprintf(CRITICAL,"Error:  capa reg_val = %d\n", reg_val);
		return ret;
	}

	//ret = cw2015_read_byte(REG_SOC1, &reg_val11);
	
	cw_capacity = reg_val;

	pmic_temp = get_tbat_volt(3);
	
	if ((pmic_temp > 8) && (cw_capacity == 0))
	{
		ret = cw2015_read_byte(REG_CONFIG, &temp_val);
		if (ret < 0) 
		  return ret;
	  
	  if ((temp_val & 0xf0) == LOW_TEMPERATURE_POWER_OFF)
		{
			/*ret = cw2015_read_byte(REG_MODE, &temp_val1);
			if (ret < 0) 
				return ret;*/
		
			cw_quickstart();
			for (i = 0; i < 30; i++) {
				ret = cw2015_read_byte(REG_SOC, &reg_val);
				if (ret < 0)
					return ret;
				else if (reg_val <= 0x64) 
					break;
				mdelay(120);
			}
			
			temp_val &= 0x07;    /* clear ATHD */
			ret = cw2015_write_byte(REG_CONFIG, temp_val);
			if (ret < 0) 
				return ret;
			cw_capacity = reg_val;
			dprintf(CRITICAL,"cw2015 low temper capacity=%d\n",cw_capacity);
		}
	}
	
	if ((pmic_temp > 8) && (cw_capacity > 0))
	{
		ret = cw2015_read_byte(REG_CONFIG, &temp_val1);
		if (ret < 0) 
		  return ret;
	  
		if ((temp_val1 & 0xf0) == LOW_TEMPERATURE_POWER_OFF)
		{
			ret = cw2015_read_byte(REG_CONFIG, &temp_val);
			if (ret < 0) 
				return ret;
			
			temp_val &= 0x07;    /* clear ATHD */
			temp_val |= LOW_TEMPERATURE_POWER_OFF;    /* set ATHD */
			ret = cw2015_write_byte(REG_CONFIG, temp_val);
			if (ret < 0) 
				return ret;
			dprintf(CRITICAL,"cw2015 low temp(%d) flag clear(cap=%d)\n",pmic_temp,cw_capacity);
		}
	}
	
	dprintf(CRITICAL,"hjian get cw2015 = %d \n", cw_capacity);
	return cw_capacity;
}

/*This function called when get voltage from cw2015*/
int cw2015_get_voltage(void)
{    
    int ret;
    unsigned char reg_val[2];
    u16 value16, value16_1, value16_2, value16_3;
    int voltage;
    
    ret = cw2015_read_byte(REG_VCELL, reg_val);
    if(ret < 0) {
        return ret;
    }
    value16 = (reg_val[0] << 8) + reg_val[1];

    ret = cw2015_read_byte(REG_VCELL, reg_val);
    if(ret < 0) {
          return ret;
    }
    value16_1 = (reg_val[0] << 8) + reg_val[1];

    ret = cw2015_read_byte(REG_VCELL, reg_val);
    if(ret < 0) {
        return ret;
    }
    value16_2 = (reg_val[0] << 8) + reg_val[1];

    if(value16 > value16_1) {     
        value16_3 = value16;
        value16 = value16_1;
        value16_1 = value16_3;
    }

    if(value16_1 > value16_2) {
    value16_3 =value16_1;
    value16_1 =value16_2;
    value16_2 =value16_3;
    }

    if(value16 >value16_1) {     
    value16_3 =value16;
    value16 =value16_1;
    value16_1 =value16_3;
    }            

    voltage = value16_1 * 312 / 1024;


    return voltage;
}

