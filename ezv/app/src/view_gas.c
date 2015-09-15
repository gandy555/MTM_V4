/******************************************************************************
 * Filename:
 *   view_gas.c
 *
 * Description:
 *   The display related gas information 
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
	ICON_IMG_GAS_ON = 0,
	ICON_IMG_GAS_OFF
};

enum {
	ICON_IMG_GAS_SW_ON = 0,
	ICON_IMG_GAS_SW_OFF
};

static u8 g_gas_status = 0;

static obj_img_t *g_gas_bg_h;
static obj_icon_t *g_gas_icon_h;
static obj_icon_t *g_gas_sw_icon_h;
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_gas_init(void);

//------------------------------------------------------------------------------
// Function Name  : update_gas_info()
// Description    :
//------------------------------------------------------------------------------
void update_gas_info(void)
{
	g_gas_status = g_app_status.gas_stat;
}

//------------------------------------------------------------------------------
// Function Name  : diff_gas_info()
// Description    :
//------------------------------------------------------------------------------
u8 diff_gas_info(void)
{
	if (g_gas_status != g_app_status.gas_stat)
		return 1;
	
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : view_gas_key()
// Description    :
//------------------------------------------------------------------------------
void view_gas_key(u16 _type, u16 _code)
{
	DBG_MSG_CO(CO_BLUE, "<%s> type: %d, code: %d\r\n", _type, _code);

	if (_type == KEY_TYPE_LONG) {
		return;
	}
	
	switch (_code) {
	case KEY_RIGHT_TOP:
		hcm_req_gas_status();
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
		hcm_switch_ui(VIEW_ID_PARKING);
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_gas_entry()
// Description    :
//------------------------------------------------------------------------------
void view_gas_entry(void)
{
	PRINT_FUNC_CO();

	hcm_req_gas_status();

	update_gas_info();

	ui_draw_image(g_gas_bg_h);

	if (g_gas_status) {
		ui_draw_icon_image(g_gas_icon_h, ICON_IMG_GAS_OFF);
		ui_draw_icon_image(g_gas_sw_icon_h, ICON_IMG_GAS_SW_ON);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "가스를 차단하였습니다");
	} else {
		ui_draw_icon_image(g_gas_icon_h, ICON_IMG_GAS_ON);
		ui_draw_icon_image(g_gas_sw_icon_h, ICON_IMG_GAS_SW_OFF);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "가스를 사용중입니다");
	}
		
	ui_draw_text(144, 329, 150, 32, 24, WHITE, TXT_ALIGN_RIGHT, "가스사용");
	ui_draw_text(625, 329, 150, 32, 24, WHITE, TXT_ALIGN_LEFT, "가스차단");
}

//------------------------------------------------------------------------------
// Function Name  : view_gas_draw()
// Description    :
//------------------------------------------------------------------------------
void view_gas_draw(void)
{
	if (diff_gas_info() == 0)
		return;
	
	update_gas_info();

	if (g_gas_status) {
		ui_draw_icon_image(g_gas_icon_h, ICON_IMG_GAS_ON);
		ui_draw_icon_image(g_gas_sw_icon_h, ICON_IMG_GAS_SW_ON);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "가스를 차단하였습니다");
		// play_wave("/app/sound/gas_off.wav\0");
	} else {
		ui_draw_icon_image(g_gas_icon_h, ICON_IMG_GAS_OFF);
		ui_draw_icon_image(g_gas_sw_icon_h, ICON_IMG_GAS_SW_OFF);
		ui_draw_text(240, 108, 500, 40, 32, WHITE, TXT_ALIGN_LEFT, "가스를 사용중입니다");
		// play_wave("/app/sound/gas_on.wav\0");
	}
}

//------------------------------------------------------------------------------
// Function Name  : view_gas_exit()
// Description    :
//------------------------------------------------------------------------------
void view_gas_exit(void)
{
	PRINT_FUNC_CO();
}

//------------------------------------------------------------------------------
// Function Name  : view_gas_init()
// Description    :
//------------------------------------------------------------------------------
void view_gas_init(void)
{	
	obj_img_t *bg_h = g_gas_bg_h;
	obj_icon_t *gas_icon_h = g_gas_icon_h;
	obj_icon_t *sw_icon_h = g_gas_sw_icon_h;
	
	// back ground image
	bg_h = ui_create_img_obj(0, 0, g_scr_info.cols, g_scr_info.rows,
				"/app/img/blank_bg.png");
	// gas icon 
	gas_icon_h = ui_create_icon_obj(58, 50, 158, 158);
	if (gas_icon_h) {
		ui_load_icon_img(gas_icon_h, ICON_IMG_GAS_ON,
			"/app/img/icon_gas_on.png");
		ui_load_icon_img(gas_icon_h, ICON_IMG_GAS_OFF,
			"/app/img/icon_gas_off.png");
	}

	// switch icon 
	sw_icon_h = ui_create_icon_obj(312, 287, 299, 116);
	if (sw_icon_h) {
		ui_load_icon_img(sw_icon_h, ICON_IMG_GAS_SW_ON,
			"/app/img/icon_switch_on.png");
		ui_load_icon_img(sw_icon_h, ICON_IMG_GAS_SW_OFF,
			"/app/img/icon_switch_off.png");
	}

	ui_register_view(VIEW_ID_GAS, view_gas_entry,
		view_gas_draw, view_gas_exit);

	hcm_register_key_handler(VIEW_ID_GAS, view_gas_key);
}

