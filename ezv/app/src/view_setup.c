/******************************************************************************
 * Filename:
 *   view_setup.c
 *
 * Description:
 *   The display related setup data
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
#define SCR_WIDTH						800
#define SCR_HEIGHT						480

#define TITLE_WIDTH						300
#define TITLE_HEIGHT					80

#define BUTTON_WIDTH					120
#define BUTTON_HEIGHT					80

#define BUTTON_VCENTER_1				85		//g_scr_info.rows/4*1 - hButton/2 + 5;
#define BUTTON_VCENTER_2				240		//g_scr_info.rows/4*2    
#define BUTTON_VCENTER_3				400		//g_scr_info.rows/4*3 + hButton/2;

#define GRAPH_SPEED						2

static rect_t g_rect_bg;
static rect_t g_rect_title;
static rect_t g_rect_board;
static rect_t g_rect_button[6];
static rect_t g_rect_graph;
static GR_WINDOW_ID g_pixmap_board;
static GR_WINDOW_ID g_pixmap_graph;
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_setup_init(void);

//------------------------------------------------------------------------------
// Function Name  : view_setup_key()
// Description    :
//------------------------------------------------------------------------------
void view_setup_key(u16 _type, u16 _code)
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
// Function Name  : view_setup_entry()
// Description    :
//------------------------------------------------------------------------------
void view_setup_entry(void)
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
// Function Name  : view_setup_draw()
// Description    :
//------------------------------------------------------------------------------
void view_setup_draw(void)
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
// Function Name  : view_setup_exit()
// Description    :
//------------------------------------------------------------------------------
void view_setup_exit(void)
{
	PRINT_FUNC_CO();
}

//------------------------------------------------------------------------------
// Function Name  : view_setup_init()
// Description    :
//------------------------------------------------------------------------------
void view_setup_init(void)
{	
	g_rect_bg.x = 0;
	g_rect_bg.y = 0;
	g_rect_bg.w = g_scr_info.cols;
	g_rect_bg.h = g_scr_info.rows;

	g_rect_title.x = SCR_WIDTH/2-TITLE_WIDTH/2;
	g_rect_title.y = 20;
	g_rect_title.w = TITLE_WIDTH;
	g_rect_title.h = TITLE_HEIGHT;

	g_rect_board.x = BUTTON_WIDTH+20;
	g_rect_board.y =  TITLE_HEIGHT+40;
	g_rect_board.w = SCR_WIDTH - (BUTTON_WIDTH+20)*2;
	g_rect_board.h = BUTTON_VCENTER_3-(TITLE_HEIGHT+40)+BUTTON_HEIGHT/2;

	g_rect_button[0].x = 0;
	g_rect_button[0].y = BUTTON_VCENTER_1-BUTTON_HEIGHT/2;
	g_rect_button[0].w = BUTTON_WIDTH;
	g_rect_button[0].h = BUTTON_HEIGHT;

	g_rect_button[1].x = 0;
	g_rect_button[1].y = BUTTON_VCENTER_2-BUTTON_HEIGHT/2;
	g_rect_button[1].w = BUTTON_WIDTH;
	g_rect_button[1].h = BUTTON_HEIGHT;

	g_rect_button[2].x = 0;
	g_rect_button[2].y = BUTTON_VCENTER_3-BUTTON_HEIGHT/2;
	g_rect_button[2].w = BUTTON_WIDTH;
	g_rect_button[2].h = BUTTON_HEIGHT;

	g_rect_button[3].x = SCR_WIDTH-1-BUTTON_WIDTH;
	g_rect_button[3].y = BUTTON_VCENTER_1-BUTTON_HEIGHT/2;
	g_rect_button[3].w = BUTTON_WIDTH;
	g_rect_button[3].h = BUTTON_HEIGHT;

	g_rect_button[4].x = SCR_WIDTH-1-BUTTON_WIDTH;
	g_rect_button[4].y = BUTTON_VCENTER_2-BUTTON_HEIGHT/2;
	g_rect_button[4].w = BUTTON_WIDTH;
	g_rect_button[4].h = BUTTON_HEIGHT;

	g_rect_button[5].x = SCR_WIDTH-1-BUTTON_WIDTH;
	g_rect_button[5].y = BUTTON_VCENTER_3-BUTTON_HEIGHT/2;
	g_rect_button[5].w = BUTTON_WIDTH;
	g_rect_button[5].h = BUTTON_HEIGHT;

	g_rect_graph.x = g_rect_board.x+20;
	g_rect_graph.y = g_rect_board.y+20;
	g_rect_graph.w = g_rect_board.w-40;
	g_rect_graph.h = g_rect_board.h-40;

	g_pixmap_board = GrNewPixmap(g_rect_board.w-2, g_rect_board.h-2, NULL);	//for WallPad Serial Dump
	g_pixmap_graph = GrNewPixmap(g_rect_graph.w, g_rect_graph.h, NULL);	//for Microwave Graph
	
	ui_register_view(VIEW_ID_SETUP, view_setup_entry,
		view_setup_draw, view_setup_exit);

	hcm_register_key_handler(VIEW_ID_SETUP, view_setup_key);
}

