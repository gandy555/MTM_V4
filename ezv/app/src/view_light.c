/******************************************************************************
 * Filename:
 *   view_light.c
 *
 * Description:
 *   The display related light information 
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
enum {
	ICON_IMG_LIGHT_ON = 0,
	ICON_IMG_LIGHT_OFF
};

enum {
	ICON_IMG_LIGHT_SW_ON = 0,
	ICON_IMG_LIGHT_SW_OFF
};

static u8 g_light_status = 0;

static obj_img_t *g_light_bg_h;
static obj_icon_t *g_light_icon_h;
static obj_icon_t *g_light_sw_icon_h;
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_light_init(void);

//------------------------------------------------------------------------------
// Function Name  : update_light_info()
// Description    :
//------------------------------------------------------------------------------
void update_light_info(void)
{
	g_light_status = g_app_status.light_stat;
}

//------------------------------------------------------------------------------
// Function Name  : diff_light_info()
// Description    :
//------------------------------------------------------------------------------
u8 diff_light_info(void)
{
	if (g_light_status != g_app_status.light_stat)
		return 1;
	
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : view_light_key()
// Description    :
//------------------------------------------------------------------------------
void view_light_key(u16 _type, u16 _code)
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
		// hcm_req_light_control();
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
		hcm_switch_ui(VIEW_ID_PARKING);
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_light_entry()
// Description    :
//------------------------------------------------------------------------------
void view_light_entry(void)
{
	PRINT_FUNC_CO();

	update_light_info();

	ui_draw_image(g_light_bg_h);

	if (g_light_status) {
		ui_draw_icon_image(g_light_icon_h, ICON_IMG_LIGHT_ON);
		ui_draw_icon_image(g_light_sw_icon_h, ICON_IMG_LIGHT_SW_ON);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "일괄소등되었습니다");
	} else {
		ui_draw_icon_image(g_light_icon_h, ICON_IMG_LIGHT_OFF);
		ui_draw_icon_image(g_light_sw_icon_h, ICON_IMG_LIGHT_SW_OFF);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "일괄소등 해제되었습니다");
	}
		
	ui_draw_text(144, 329, 150, 32, 24, WHITE, TXT_ALIGN_RIGHT, "일괄소등해제");
	ui_draw_text(625, 329, 150, 32, 24, WHITE, TXT_ALIGN_LEFT, "일괄소등");
}

//------------------------------------------------------------------------------
// Function Name  : view_light_draw()
// Description    :
//------------------------------------------------------------------------------
void view_light_draw(void)
{
	if (diff_light_info() == 0)
		return;
	
	update_light_info();

	if (g_light_status) {
		ui_draw_icon_image(g_light_icon_h, ICON_IMG_LIGHT_ON);
		ui_draw_icon_image(g_light_sw_icon_h, ICON_IMG_LIGHT_SW_ON);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "일괄소등되었습니다");
		// play_wave("/app/sound/light_off.wav\0");
	} else {
		ui_draw_icon_image(g_light_icon_h, ICON_IMG_LIGHT_OFF);
		ui_draw_icon_image(g_light_sw_icon_h, ICON_IMG_LIGHT_SW_OFF);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "일괄소등 해제되었습니다");
		// play_wave("/app/sound/light_on.wav\0");
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_light_exit()
// Description    :
//------------------------------------------------------------------------------
void view_light_exit(void)
{
	PRINT_FUNC_CO();
}

//------------------------------------------------------------------------------
// Function Name  : view_light_init()
// Description    :
//------------------------------------------------------------------------------
void view_light_init(void)
{	
	obj_img_t *bg_h = g_light_bg_h;
	obj_icon_t *light_icon_h = g_light_icon_h;
	obj_icon_t *sw_icon_h = g_light_sw_icon_h;
	
	// back ground image
	bg_h = ui_create_img_obj(0, 0, g_scr_info.cols, g_scr_info.rows,
				"/app/img/blank_bg.png");
	// light icon 
	light_icon_h = ui_create_icon_obj(58, 50, 158, 158);
	if (light_icon_h) {
		ui_load_icon_img(light_icon_h, ICON_IMG_LIGHT_ON,
			"/app/img/icon_light_on.png");
		ui_load_icon_img(light_icon_h, ICON_IMG_LIGHT_OFF,
			"/app/img/icon_light_off.png");
	}

	// switch icon 
	sw_icon_h = ui_create_icon_obj(312, 287, 299, 116);
	if (sw_icon_h) {
		ui_load_icon_img(sw_icon_h, ICON_IMG_LIGHT_SW_ON,
			"/app/img/icon_switch_on.png");
		ui_load_icon_img(sw_icon_h, ICON_IMG_LIGHT_SW_OFF,
			"/app/img/icon_switch_off.png");
	}

	ui_register_view(VIEW_ID_LIGHT, view_light_entry,
		view_light_draw, view_light_exit);

	hcm_register_key_handler(VIEW_ID_LIGHT, view_light_key);
}

