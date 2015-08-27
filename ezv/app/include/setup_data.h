#ifndef __SETUP_DATA_H__
#define __SETUP_DATA_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define SETUP_DATA_MAGIC				0x4D544D04	//MTM{0x04}		2014-12-21
#define SETUP_DATA_PATH					"/app/"
#define SETUP_DATA_FILE					"setup.dat"

#define SETUP_VOLUME_DEF				7
#define SETUP_ADC_VALUE_DEF			780			// KCM �������� ���� Default

#define SETUP_TEMP_DEF					123
#define SETUP_WEATHER_DEF				0	

typedef struct {
	u32 magic;
	u8 wallpad_type;
	u8 ui_option;
	u8 key_option;
	u32 volume;
	u32 adc_value;			// microwave adc ���� ������
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	int current_temp;
	u32 weather_left;		// ���� ���� (������ ����)
	int temp_low_left;		//���� �����µ� (�Ҽ��� 1�ڸ�, ������ 10�ؼ� �����)
	int temp_high_left;		//���� �ְ�µ� (�Ҽ��� 1�ڸ�, ������ 10�ؼ� �����)	!!SNS�������� �µ��� ����!!
	u32 weather_right;		//������ ���� (������ ����)
	int temp_low_right;		//������ �ְ�µ� (�Ҽ��� 1�ڸ�, ������ 10�ؼ� �����)
	int temp_high_right;		//������ �ְ�µ� (�Ҽ��� 1�ڸ�, ������ 10�ؼ� �����)
	u8 light_stat;			//�ϰ��ҵ����	0=����(�ҵ�����), 1=�ҵ�
	u8 gas_stat;				//����������	0=����(�����), 1=����
	u8 security_stat;			//���ȼ�������	0=����, 1=����
	u8 park_status;									//����
	u8 park_floor[MAX_PARKING_FLOOR_NAME];		//����Ī
	u8 park_zone[MAX_PARKING_ZONE_NAME];		//������Ī
	u8 park_id[MAX_PARKING_CAR_NUM];			//������ȣ
	u8 elevator_status;
	s8 elevator_floor;		//���(1��~127��), ����(B1��~Bx��), 0=����
	u8 door_opened;
} __attribute__ ((packed)) app_status_t;	

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void init_setup_data(app_status_t *_setup);
#endif //__SETUP_DATA_H__
