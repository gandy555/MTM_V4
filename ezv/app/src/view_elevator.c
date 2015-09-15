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

static u8 g_elev_called = 0;
static u8 g_elev_status = 0;
static s8 g_elev_floor = 0;
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

#define ARROW_DOWN_OFFSET	ICON_IMG_ARROW_UP_3
//------------------------------------------------------------------------------
// Function Name  : view_elevator_animation_arrow()
// Description    :
//------------------------------------------------------------------------------
static void view_elevator_animation_arrow(u32 _param)
{
	static u8 arrow_pos = 1;

	switch (g_elev_status) {
	case MTM_DATA_EV_STATUS_STOP:
		arrow_pos = 0;
		ui_draw_icon_image(g_arrow_icon_h, ICON_IMG_ARROW_BLANK);
		break;
	case MTM_DATA_EV_STATUS_UP:
		ui_draw_icon_image(g_arrow_icon_h, arrow_pos);
		break;
	case MTM_DATA_EV_STATUS_DOWN:
		ui_draw_icon_image(g_arrow_icon_h, ARROW_DOWN_OFFSET + arrow_pos);
		break;
	case MTM_DATA_EV_STATUS_ARRIVE:
		arrow_pos = 0;
		ui_draw_icon_image(g_arrow_icon_h, ICON_IMG_ARROW_BLANK);
		break;
	}
	
	if (++arrow_pos > 3)
		arrow_pos = 1;
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_key()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_key(u16 _type, u16 _code)
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
		hcm_req_elevator_call();
		break;
	case KEY_LEFT_BOTTOM:
		hcm_switch_ui(VIEW_ID_PARKING);
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_entry()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_entry(void)
{
	PRINT_FUNC_CO();

	hcm_req_elevator_call();
	
	update_elevator_info();

	ui_draw_image(g_elev_bg_h);

	ui_draw_image(g_icon_img_h);

	ui_draw_image(g_elev_img_h);

	ui_draw_image(g_floor_img_h);
	
	ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터 상태 확인중 입니다");
	//ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터를 호출하였습니다");
	
	hcm_register_workqueue(500, view_elevator_animation_arrow);
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_draw()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_draw(void)
{
	rect_t rect;
	
	if (diff_elevator_info() == 0)
		return;
	
	update_elevator_info();

	rect.x = 245;
	rect.y = 89;
	rect.w = 500;
	rect.h = 32;
	ui_draw_image_part(g_elev_bg_h, &rect, &rect);

	switch (g_elev_status) {
	case MTM_DATA_EV_STATUS_STOP:
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터가 정지해 있습니다");
		break;
	case MTM_DATA_EV_STATUS_UP:
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터가 이동중 입니다");
		break;
	case MTM_DATA_EV_STATUS_DOWN:
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터가 이동중 입니다");
		break;
	case MTM_DATA_EV_STATUS_ARRIVE:
		ui_draw_text(245, 89, 500, 32, 24, WHITE, TXT_ALIGN_LEFT, "엘레베이터가 도착 하였습니다");
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_elevator_exit()
// Description    :
//------------------------------------------------------------------------------
void view_elevator_exit(void)
{
	PRINT_FUNC_CO();
	
	hcm_unregister_workqueue(view_elevator_animation_arrow);
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

	ui_register_view(VIEW_ID_ELEVATOR, view_elevator_entry,
		view_elevator_draw, view_elevator_exit);

	hcm_register_key_handler(VIEW_ID_ELEVATOR, view_elevator_key);
}

