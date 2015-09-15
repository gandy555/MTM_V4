#ifndef __PARKING_INFO_H__
#define __PARKING_INFO_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
enum {
	PARKING_ITM_ID = 0,
	PARKING_ITM_FLOOR,
	PARKING_ITM_ZONE
};

enum {
	PARKING_STATUS_INVALID = 0,
	PARKING_STATUS_IN,
	PARKING_STATUS_OUT
};

#define MAX_PARKING_FLOOR_NAME			4
#define MAX_PARKING_ZONE_NAME			7
#define MAX_PARKING_CAR_NUM				5

#define MAX_PARKING_LIST		4
typedef struct {
	u8 status;
	char floor[MAX_PARKING_FLOOR_NAME];
	char zone[MAX_PARKING_ZONE_NAME];
	char car_num[MAX_PARKING_CAR_NUM];
} parking_info_t;

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void init_parking_info(void);
extern void parking_list_update(parking_info_t* _info);
extern int parking_list_get_item(int _idx, parking_info_t* _dst);
extern int parking_item_to_str(int _id, char* _src, char* _dst);
#endif // __PARKING_INFO_H__

