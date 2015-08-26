#ifndef __SETUP_DATA_H__
#define __SETUP_DATA_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define SETUP_DATA_MAGIC				0x4D544D04	//MTM{0x04}		2014-12-21
#define SETUP_DATA_PATH					"/app/"
#define SETUP_DATA_FILE					"setup.dat"

#define SETUP_VOLUME_DEF				7
#define SETUP_ADC_VALUE_DEF			780			// KCM 구리수택 현장 Default

#define SETUP_TEMP_DEF					123
#define SETUP_WEATHER_DEF				0	

typedef struct {
	u32 magic;
	u8 wallpad_type;
	u8 ui_option;
	u8 key_option;
	u32 volume;
	u32 adc_value;			// microwave adc 감지 설정값
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	int current_temp;
	u32 weather_left;		// 왼쪽 날씨 (아이콘 종류)
	int temp_low_left;		//왼쪽 최저온도 (소수점 1자리, 나누기 10해서 써야함)
	int temp_high_left;		//왼쪽 최고온도 (소수점 1자리, 나누기 10해서 써야함)	!!SNS버전에서 온도는 정수!!
	u32 weather_right;		//오른쪽 날씨 (아이콘 종류)
	int temp_low_right;		//오른쪽 최고온도 (소수점 1자리, 나누기 10해서 써야함)
	int temp_high_right;		//오른쪽 최고온도 (소수점 1자리, 나누기 10해서 써야함)
	u8 light_stat;			//일괄소등상태	0=점등(소등해제), 1=소등
	u8 gas_stat;				//가스밸브상태	0=열림(사용중), 1=차단
	u8 security_stat;			//보안설정상태	0=해제, 1=설정
	u8 park_status;									//상태
	u8 park_floor[MAX_PARKING_FLOOR_NAME];		//층명칭
	u8 park_zone[MAX_PARKING_ZONE_NAME];		//구역명칭
	u8 park_id[MAX_PARKING_CAR_NUM];			//차량번호
	u8 elevator_status;
	s8 elevator_floor;		//양수(1층~127층), 음수(B1층~Bx층), 0=에러
	u8 door_opened;
} __attribute__ ((packed)) app_status_t;	

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void init_setup_data(app_status_t *_setup);
#endif //__SETUP_DATA_H__
