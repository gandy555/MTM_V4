/******************************************************************************
 * Filename:
 *   view_elevator.c
 *
 * Description:
 *   The elevator display the related information 
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-26
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "main.h"
#include "view_elevator.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
enum {
	ICON_IMG_ARROW_BLANK = 0,
	ICON_IMG_ARROW_UP_1,
	ICON_IMG_ARROW_UP_2,
	ICON_IMG_ARROW_UP_3,
	ICON_IMG_ARROW_DOWN_1,
	ICON_IMG_ARROW_DOWN_2,
	ICON_IMG_ARROW_DOWN_3,
	MAX_ICON_IMG_ARROW,
};

static obj_img_t *g_elev_bg_h;
static obj_img_t *g_elev_img_h;
static obj_img_t *g_icon_img_h;
static obj_img_t *g_floor_img_h;
static obj_icon_t *g_arrow_icon_h;

static u8 g_elev_status = 0;
static s8 g_elev_floor = 0;
static workqueue_list_t *g_elevator_workqueue;
#define ELEVATOR_WQ_H	g_elevator_workqueue
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_elevator_init(void);

//------------------------------------------------------------------------------
// Function Name  : update_elevator_info()
// Description    :
//------------------------------------------------------------------------------
void update_elevator_info(void)
{
	g_elev_status = g_app_status.elevator_status;
	g_elev_floor = g_app_status.elevator_floor;
}

//------------------------------------------------------------------------------
// Function Name  : diff_elevator_info()
// Description    :
//------------------------------------------------------------------------------
u8 diff_elevator_info(void)
{
	if (g_elev_status != g_app_status.elevator_status)
		return 1;
	
	if (g_elev_floor != g_app_status.elevator_floor)
		return 1;

	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_entry()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_entry(void)
{
	char temp_str[64] = {0,};
	char *side_txt;
	
	PRINT_FUNC_CO();

	update_elevator_info();

	ui_draw_image(g_elev_bg_h);

	ui_draw_image(g_icon_img_h);

	ui_draw_image(g_elev_img_h);

	ui_draw_image(g_floor_img_h);
	
	ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터 상태 확인중 입니다");
	//ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터를 호출하였습니다");
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_draw()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_draw(void)
{
	char temp_str[64] = {0,};
	char *side_txt;
	
	if (diff_elevator_info() == 0)
		return;
	
	update_elevator_info();
	
	/* contents of left side */
	side_txt = g_weather_short_text_tbl[l_weather_idx];
	ui_draw_text(65, 51, 96, 32, 24, RGB(1,19,73), TXT_ALIGN_CENTER, side_txt);

	sprintf(temp_str, "%2d/%2d\0",
		g_app_status.temp_low_left / 10, g_app_status.temp_high_left / 10);
	ui_draw_text(17, 100, 112, 32, 24, WHITE, TXT_ALIGN_RIGHT, temp_str);
	
	ui_draw_icon_image(g_l_icon_h, l_weather_idx);

	/* contents of right side */
	memset(temp_str, 0, sizeof(temp_str));
	side_txt = g_weather_short_text_tbl[r_weather_idx];
	ui_draw_text(647, 51, 96, 32, 24, RGB(162,199,213), TXT_ALIGN_CENTER, side_txt);

	sprintf(temp_str, "%2d/%2d\0",
		g_app_status.temp_low_right / 10, g_app_status.temp_high_right / 10);
	ui_draw_text(599, 100, 112, 32, 24, WHITE, TXT_ALIGN_RIGHT, temp_str);
	
	ui_draw_icon_image(g_r_icon_h, r_weather_idx);

	/* gas status */
	ui_draw_icon_image(g_gas_icon_h, g_app_status.gas_stat);
	
	/* parking information */
	ui_draw_image(g_park_bg_h);
	parking_list_get_item(0, &parking_info);		
	if (parking_info.status == PARKING_STATUS_IN) {
		memset(temp_str, 0, sizeof(temp_str));
		sprintf(temp_str, "%s-%s", parking_info.floor, parking_info.zone);
		ui_draw_text(73, 436, 180, 32, 24, WHITE, TXT_ALIGN_LEFT, temp_str);
	}
	
	/* version information */
	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "[%s-%s %s ]", RELEASE_VERSION, RELEASE_DATE, RELEASE_PRODUCT);
	ui_draw_text(0, 400, strlen(temp_str)*6+4, 14, 12, BLACK, TXT_ALIGN_CENTER);
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_exit()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_exit(void)
{
	PRINT_FUNC_CO();

	workqueue_delete_all(ELEVATOR_WQ_H);
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_key()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_key(u32 _type, u32 _code)
{
	DBG_MSG_CO(CO_BLUE, "<%s> type: %d, code: %d\r\n", _type, _code);
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_init()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_init(void)
{
	obj_img_t *bg_h = g_elev_bg_h;
	obj_img_t *icon_img_h = g_icon_img_h;
	obj_img_t *elev_h = g_elev_img_h;	
	obj_img_t *floor_h = g_floor_img_h;
	obj_icon_t *arrow_h = g_arrow_icon_h;

	ELEVATOR_WQ_H = workqueue_create("ELEVATOR VIEW");
	
	// back ground image
	bg_h = ui_create_img_obj(0, 0, g_scr_info.cols, g_scr_info.rows,
				"/app/img/blank_bg.png");

	// elevator icon img
	icon_img_h = ui_create_img_obj(58, 50, 158, 158, 
					"/app/img/icon_elevator.png");
	
	// elevator img
	elev_h = ui_create_img_obj(510, 175, 207, 258,
				"/app/img/elevator_image.png");

	// floor img
	floor_h = ui_create_img_obj(339, 240, 143, 127,
				"/app/img/elevator_display.png");
	
	// arrow icon 
	arrow_h = ui_create_icon_obj(232, 210, 86, 190);
	if (arrow_h) {
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_BLANK,
			"/app/img/el_png/arrow_back.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_UP_1,
			"/app/img/el_png/arrow_up_1.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_UP_2,
			"/app/img/el_png/arrow_up_2.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_UP_3,
			"/app/img/el_png/arrow_up_3.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_DOWN_1,
			"/app/img/el_png/arrow_down_1.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_DOWN_2,
			"/app/img/el_png/arrow_down_2.png");
		ui_load_icon_img(arrow_h, ICON_IMG_ARROW_DOWN_3,
			"/app/img/el_png/arrow_down_3.png");
	}

	ui_register_view(VIEW_ID_ELEVATOR,
		view_elevator_entry, view_elevator_draw,
		view_elevator_exit, view_elevator_key);
}

