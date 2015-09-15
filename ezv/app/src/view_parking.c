/******************************************************************************
 * Filename:
 *   view_parking.c
 *
 * Description:
 *   The display related parking information 
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-09-15
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "main.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
static parking_info_t g_parking_info;

static obj_img_t *g_parking_bg_h;
static obj_img_t *g_parking_img_h;
static obj_img_t *g_map_img_h;
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_parking_init(void);

//------------------------------------------------------------------------------
// Function Name  : diff_parking_info()
// Description    :
//------------------------------------------------------------------------------
u8 diff_parking_info(void)
{
	if (g_parking_info.status != g_app_status.park_status)
		return 1;

	if (memcmp(g_parking_info.floor, g_app_status.park_floor, MAX_PARKING_FLOOR_NAME))
		return 1;
	
	if (memcmp(g_parking_info.zone, g_app_status.park_zone, MAX_PARKING_ZONE_NAME))
		return 1;
	
	if (memcmp(g_parking_info.car_num, g_app_status.park_id, MAX_PARKING_CAR_NUM))
		return 1;
	
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : view_parking_key()
// Description    :
//------------------------------------------------------------------------------
void view_parking_key(u16 _type, u16 _code)
{
	DBG_MSG_CO(CO_BLUE, "<%s> type: %d, code: %d\r\n", _type, _code);

	if (_type == KEY_TYPE_LONG) {
		return;
	}
	
	switch (_code) {
	case KEY_RIGHT_TOP:
		hcm_switch_ui(VIEW_ID_GAS);
		break;
	case KEY_RIGHT_MIDDLE:
		hcm_switch_ui(VIEW_ID_LIGHT);
		break;
	case KEY_RIGHT_BOTTOM:
		hcm_switch_ui(VIEW_ID_SECURITY);
		break;
	case KEY_LEFT_TOP:
		hcm_switch_ui(VIEW_ID_WEATHER);
		break;
	case KEY_LEFT_MIDDLE:
		hcm_switch_ui(VIEW_ID_ELEVATOR);
		break;
	case KEY_LEFT_BOTTOM:
		hcm_req_parking_info();
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_parking_entry()
// Description    :
//------------------------------------------------------------------------------
void view_parking_entry(void)
{
	parking_info_t parking_info;
	char temp_str[64] = {0,};
	rect_t rect;
	int i;
	
	PRINT_FUNC_CO();

	hcm_req_parking_info();

	ui_draw_image(g_parking_bg_h);
	ui_draw_image(g_parking_img_h);
	ui_draw_image(g_map_img_h);
	
	if (g_parking_info.status == PARKING_STATUS_IN) {
		sprintf(temp_str, "[%s]층 [%s]에 주차 되었습니다",
			g_parking_info.floor, g_parking_info.zone);
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, temp_str);
	} else {
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT,
			"주차위치를 확인할 수 없습니다");
	}

	rect.x = 37;
	rect.y = 251;
	rect.w = 172;
	rect.h = 200;
	ui_draw_image_part(g_parking_bg_h, &rect, &rect);

	for (i = 0; i < MAX_PARKING_LIST; i++) {
		parking_list_get_item(i, &parking_info);
		if (parking_info.status != PARKING_STATUS_IN)
			break;
		memset(temp_str, 0, sizeof(temp_str));
		sprintf(temp_str, "%s -%s", parking_info.floor, parking_info.zone);
		ui_draw_text(37, 251+(36*i), 172, 32, 24, WHITE, TXT_ALIGN_LEFT, temp_str);
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_parking_draw()
// Description    :
//------------------------------------------------------------------------------
void view_parking_draw(void)
{
	parking_info_t parking_info;
	char temp_str[64] = {0,};
	rect_t rect;
	int i;
	
	if (diff_parking_info() == 0)
		return;
	
	ui_draw_image(g_parking_bg_h);
	ui_draw_image(g_parking_img_h);
	ui_draw_image(g_map_img_h);
	
	if (g_parking_info.status == PARKING_STATUS_IN) {
		sprintf(temp_str, "[%s]층 [%s]에 주차 되었습니다",
			g_parking_info.floor, g_parking_info.zone);
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, temp_str);
	} else {
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT,
			"주차위치를 확인할 수 없습니다");
	}

	rect.x = 37;
	rect.y = 251;
	rect.w = 172;
	rect.h = 200;
	ui_draw_image_part(g_parking_bg_h, &rect, &rect);

	for (i = 0; i < MAX_PARKING_LIST; i++) {
		parking_list_get_item(i, &parking_info);
		if (parking_info.status != PARKING_STATUS_IN)
			break;
		memset(temp_str, 0, sizeof(temp_str));
		sprintf(temp_str, "%s -%s", parking_info.floor, parking_info.zone);
		ui_draw_text(37, 251+(36*i), 172, 32, 24, WHITE, TXT_ALIGN_LEFT, temp_str);
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_parking_exit()
// Description    :
//------------------------------------------------------------------------------
void view_parking_exit(void)
{
	PRINT_FUNC_CO();
}

//------------------------------------------------------------------------------
// Function Name  : view_parking_init()
// Description    :
//------------------------------------------------------------------------------
void view_parking_init(void)
{	
	obj_img_t *bg_h = g_parking_bg_h;
	obj_img_t *park_img_h = g_parking_img_h;
	obj_img_t *map_img_h = g_map_img_h;
	
	// back ground image
	bg_h = ui_create_img_obj(0, 0, g_scr_info.cols, g_scr_info.rows,
				"/app/img/blank_bg.png");
	// parking icon 
	park_img_h = ui_create_img_obj(58, 50, 158, 158,
					"/app/img/icon_parking.png");

	// map image
	map_img_h = ui_create_img_obj(245, 195, 511, 257,
					"/app/img/parking_image1.png");

	ui_register_view(VIEW_ID_PARKING, view_parking_entry,
		view_parking_draw, view_parking_exit);

	hcm_register_key_handler(VIEW_ID_PARKING, view_parking_key);
}

