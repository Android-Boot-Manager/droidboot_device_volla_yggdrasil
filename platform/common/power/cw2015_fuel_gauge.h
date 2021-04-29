/*****************************************************************************
*
* Filename:
* ---------
*   cw2015_fuel_guau.h
*
* Project:
* --------
*   Android
*
* Description:
* ------------
*   cw2015 header file
*
* Author:
* -------
*
****************************************************************************/

#ifndef _CW2015_SW_H_
#define _CW2015_SW_H_


/**********************************************************
  *
  *   [Extern Function] 
  *
  *********************************************************/
//CON0----------------------------------------------------
extern int cw2015_init(void);
extern int cw2015_get_capacity(void);
extern int cw2015_get_voltage(void);
//CON1----------------------------------------------------

//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-start
#if defined(CONFIG_MTK_CW2015_BATTERY_ID_AUXADC)
struct cw2015batinfo {
   int bat_first_startrange;
   int bat_first_endrange;
   int bat_second_startrange;
   int bat_second_endrange;
   int bat_third_startrange;
   int bat_third_endrange;
   int bat_channel_num;
   int bat_id;
};

struct cw2015batinfo cw2015fuelguage;
static char *fuelguage_name[] = {
	"batinfo_first", "batinfo_second", "batinfo_third","batinfo_default"
};
#endif
//prize-add-sunshuai-2015 Multi-Battery Solution-20200222-end

#endif // _CW2015_SW_H_

