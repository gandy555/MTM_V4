/******************************************************************************
 * Filename:
 *   setup_data.c
 *
 * Description:
 *   management for setup data (load & store)
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-02-03
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "duo_utils.h"
#include "setup_data.h"
#include "main.h"

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void init_setup_data(app_status_t *_setup);

//------------------------------------------------------------------------------
// Function Name  : set_default_setup()
// Description    : 
//------------------------------------------------------------------------------
static void set_default_setup(app_status_t *_setup)
{
	time_t now;
	struct tm *t;

	PRINT_FUNC_CO();
	
	now = time(NULL);
	t = localtime(&now);

	_setup->magic = SETUP_DATA_MAGIC;

	_setup->wallpad_type = WALLPAD_TYPE_DEFAULT;
	_setup->ui_option = UI_OPTION_DEFAULT;
	_setup->key_option = KEY_OPTION_ALL;
	_setup->volume = SETUP_VOLUME_DEF;
	_setup->adc_value = SETUP_ADC_VALUE_DEF;

	_setup->weather_year = t->tm_year;
	_setup->weather_month = t->tm_mon + 1;
	_setup->weather_day = t->tm_mday;
	_setup->weather_hour = t->tm_hour;
	_setup->weather_minute = t->tm_min;
	_setup->weather_second = t->tm_sec;

	_setup->current_temp 	= SETUP_TEMP_DEF;

	_setup->weather_left = SETUP_WEATHER_DEF;
	_setup->temp_low_left = SETUP_TEMP_DEF;
	_setup->temp_high_left = SETUP_TEMP_DEF;

	_setup->weather_right	= SETUP_WEATHER_DEF;
	_setup->temp_low_right = SETUP_TEMP_DEF;
	_setup->temp_high_right = SETUP_TEMP_DEF;

	/* 
		일괄소등 해제 
		가스밸브 사용중
		보안설정 해제
	*/
	_setup->light_stat = 0;	
	_setup->gas_stat = 0;
	_setup->security_stat = 0;

	_setup->status = 0;
	memset(&_setup->car_floor, 0, MAX_PARKING_FLOOR_NAME);	//층명칭
	memset(&_setup->car_zone,  0, MAX_PARKING_ZONE_NAME);	//구역명칭
	memset(&_setup->car_num,   0, MAX_PARKING_CAR_NUM);		//차량번호
}

//------------------------------------------------------------------------------
// Function Name  : load_setup_data()
// Description    : load the configuration data from nand drive
//------------------------------------------------------------------------------
static int load_setup_data(app_status_t *_setup)
{
	char path[128] = {0,};
	FILE *fd;
	int res = 1;

	sprintf(path, "%s%s\0", SETUP_DATA_PATH, SETUP_DATA_FILE);
	fd = fopen(path, "rb");
	if (fd == NULL) {
		DBG_MSG("<%s> %s Loading Failure\r\n", __func__, path);
		return FALSE;
	}

	/* for read magic */
	fread(_setup, 1, sizeof(app_status_t), fd);
	if (_setup->magic != SETUP_DATA_MAGIC) {
		DBG_MSG_CO(CO_RED, "<%s> UnMatch the Magic number(0x%x/0x%x)\r\n",
			__func__, _setup->magic, SETUP_DATA_MAGIC);
		res = 0;
	}

	return res;
}

//------------------------------------------------------------------------------
// Function Name  : save_setup_data()
// Description    : 
//------------------------------------------------------------------------------
static int save_setup_data(app_status_t *_setup)
{
	char path[128] = {0,};
	FILE *fd;
	int res = 1;

	PRINT_FUNC_CO();
	
	sprintf(path, "%s%s\0", SETUP_DATA_PATH, SETUP_DATA_FILE);
	fd = fopen(path, "wb");
	if (fd == NULL) {
		DBG_MSG_CO(CO_RED, "<%s> '%s' open failed!\r\n", __func__, path);
		return 0;
	}

	res = fwrite(_setup, 1, sizeof(app_status_t), fd);
	if (res != sizeof(app_status_t)) {
		DBG_MSG_CO(CO_RED, "<%s> Invalid Write Size(%d/%d)\r\n",
			__func__, res, sizeof(app_status_t));
		fclose(fd);
		return 0;
	}

	fclose(fd);
	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : init_setup_data()
// Description    : 
//------------------------------------------------------------------------------
void init_setup_data(app_status_t *_setup)
{	
	PRINT_FUNC_CO();
	
	if (!load_setup_data(_setup))
		set_default_setup(_setup);

	save_setup_data(_setup);
}

