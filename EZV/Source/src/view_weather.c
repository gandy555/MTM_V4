/******************************************************************************
 * Filename:
 *   view_weather.c
 *
 * Description:
 *   view of weather contents
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-04
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "main.h"
#include "view_weather.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
// g_wid기준좌표: y-center=454, t=438, x-left=297, x-right=790, w=493, 
#define MENT_AREA_X				297
#define MENT_AREA_Y				438
#define MENT_AREA_WIDTH			490
#define MENT_AREA_HEIGHT			32
#define MENT_SCROLL_SPEED			1

#define MENT_PIXEL_MAP_WIDTH		700

#define MAX_MENT_BUFF_SIZE		52

char g_weather_long_text_tbl[IMG_ENUM_WEATHER_COUNT][MAX_MENT_BUFF_SIZE] = {
	"오늘은 날씨가 맑을 예정입니다\0",						// IMG_ENUM_WEATHER_SERENITY
	"오늘은 날씨가 구름이 조금 낄 예정입니다\0",				// IMG_ENUM_WEATHER_PARTLY_CLOUDY
	"오늘은 날씨가 흐릴 예정입니다\0",						// IMG_ENUM_WEATHER_CLOUDY
	"오늘은 소나기가 올 예정이오니 우산을 준비하세요\0",		// IMG_ENUM_WEATHER_SHOWER
	"오늘은 비가 올 예정이오니 우산을 준비하세요\0",			// IMG_ENUM_WEATHER_RAINNY
	"오늘은 뇌우가 칠 예정이오니 우산을 준비하세요\0",		// IMG_ENUM_WEATHER_THUNDERSTORM
	"오늘은 눈이 올 예정이오니 따뜻하게 입고 외출하세요\0"	// IMG_ENUM_WEATHER_SNOW
};

char g_weather_short_text_tbl[7][10] = {
	"맑음\0", "구름조금\0", "흐림\0", "소나기\0", "비\0", "뇌우\0", "눈\0" 
};
static obj_img_t *g_weather_bg_h;
static obj_img_t *g_park_bg_h;
static obj_icon_t *g_l_icon_h;
static obj_icon_t *g_r_icon_h;
static obj_icon_t *g_gas_icon_h;

static u8 g_curr_gas = 0;
static GR_WINDOW_ID	g_ment_window;
static GR_WINDOW_ID	g_pixmap_window;

static u8 gas_cut;
static u32 l_weather_idx;
static u32 r_weather_idx;
static int l_temp_lsb, l_temp_msb;
static int r_temp_lsb, r_temp_msb;
static parking_info_t parking_info;
static workqueue_list_t *g_weather_workqueue;
#define WEATHER_WQ_H	g_weather_workqueue
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void view_weather_init(void);

//------------------------------------------------------------------------------
// Function Name  : update_weather_info()
// Description    :
//------------------------------------------------------------------------------
void update_weather_info(void)
{
	l_weather_idx = g_app_status.weather_left;
	r_weather_idx = g_app_status.weather_right;

	l_temp_lsb = g_app_status.temp_low_left;
	l_temp_msb = g_app_status.temp_high_left;
	r_temp_lsb = g_app_status.temp_low_right;
	r_temp_msb = g_app_status.temp_high_right;

	gas_cut = g_app_status.gas_stat;

	parking_list_get_item(0, &parking_info);
}

//------------------------------------------------------------------------------
// Function Name  : diff_weather_info()
// Description    :
//------------------------------------------------------------------------------
u8 diff_weather_info(void)
{
	parking_info_t parking;

	if (l_weather_idx != g_app_status.weather_left)
		return 1;
	
	if (r_weather_idx != g_app_status.weather_right)
		return 1;

	if (l_temp_lsb != g_app_status.temp_low_left)
		return 1;
	
	if (l_temp_msb != g_app_status.temp_high_left)
		return 1;
	
	if (r_temp_lsb != g_app_status.temp_low_right)
		return 1;
	
	if (r_temp_msb != g_app_status.temp_high_right)
		return 1;

	if (gas_cut != g_app_status.gas_stat)
		return 1;

	parking_list_get_item(0, &parking);
	if (memcmp(parking, parking_info, sizeof(parking_info_t)) != 0)
		return 1;

	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : view_weather_entry()
// Description    :
//------------------------------------------------------------------------------
void view_weather_entry(void)
{
	char temp_str[64] = {0,};
	char *side_txt;
	
	PRINT_FUNC_CO();

	update_weather_info();
	
	ui_draw_text(83, 20, 60, 18, 18, WHITE, TXT_ALIGN_CENTER, "오  전");
	ui_draw_text(665, 20, 60, 18, 18, WHITE, TXT_ALIGN_CENTER, "오  후");

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
// Function Name  : view_weather_draw()
// Description    :
//------------------------------------------------------------------------------
void view_weather_draw(void)
{
	char temp_str[64] = {0,};
	char *side_txt;
	
	if (diff_weather_info() == 0)
		return;
	
	update_weather_info();
	
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
// Function Name  : view_weather_exit()
// Description    :
//------------------------------------------------------------------------------
void view_weather_exit(void)
{
	PRINT_FUNC_CO();

	workqueue_delete_all(WEATHER_WQ_H);
}

//------------------------------------------------------------------------------
// Function Name  : view_weather_key()
// Description    :
//------------------------------------------------------------------------------
void view_weather_key(u32 _type, u32 _code)
{
	DBG_MSG_CO(CO_BLUE, "<%s> type: %d, code: %d\r\n", _type, _code);
}

//------------------------------------------------------------------------------
// Function Name  : view_weather_init()
// Description    :
//------------------------------------------------------------------------------
void view_weather_init(void)
{
	obj_img_t *bg_h = g_weather_bg_h;
	obj_icon_t *l_icon_h = g_l_icon_h;
	obj_icon_t *r_icon_h = g_r_icon_h;
	obj_icon_t *gas_icon_h = g_gas_icon_h;

	WEATHER_WQ_H = workqueue_create("WEATHER VIEW");
	
	// back ground image
	bg_h = ui_create_img_obj(0, 0, g_scr_info.cols, g_scr_info.rows,
				"/app/img/main_bg_blank.png");

	// weather icon of left side
	l_icon_h = ui_create_icon_obj(1, 169, 300, 246);
	if (l_icon_h) {
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_SERENITY,
			"/app/img/weather/serenity.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_PARTLY_CLOUDY,
			"/app/img/weather/partly_cloudy.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_CLOUDY,
			"/app/img/weather/cloudy.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_SHOWER,
			"/app/img/weather/shower.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_RAINNY,
			"/app/img/weather/rainny.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_THUNDERSTORM,
			"/app/img/weather/thunderstorm.png");
		ui_load_icon_img(l_icon_h, IMG_ENUM_WEATHER_SNOW,
			"/app/img/weather/snow.png");
	}

	// weather icon of right side
	r_icon_h = ui_create_icon_obj(499, 169, 300, 246);
	if (r_icon_h) {
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_SERENITY,
			"/app/img/weather/serenity.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_PARTLY_CLOUDY,
			"/app/img/weather/partly_cloudy.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_CLOUDY,
			"/app/img/weather/cloudy.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_SHOWER,
			"/app/img/weather/shower.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_RAINNY,
			"/app/img/weather/rainny.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_THUNDERSTORM,
			"/app/img/weather/thunderstorm.png");
		ui_load_icon_img(r_icon_h, IMG_ENUM_WEATHER_SNOW,
			"/app/img/weather/snow.png");
	}

	// gas icon(on/off)
	gas_icon_h = ui_create_icon_obj(325, 245, 137, 137);	
	if (gas_icon_h) {
		ui_load_icon_img(gas_icon_h, IMG_ENUM_GAS_ON,
			"/app/img/main_icon/main_gas_on.png");		//적색 가스사용
		ui_load_icon_img(gas_icon_h, IMG_ENUM_GAS_OFF,
			"/app/img/main_icon/main_gas_off.png");		//녹색 가스차단
	}

	// parking icon
	bg_h = g_park_bg_h;
	bg_h = ui_create_img_obj(0, 419, 261, 61, "/app/img/main_bar_back.png");

	g_curr_gas = (g_app_status.gas_stat) ? TRUE : FALSE;

	// sub window for ment
	g_ment_window = GrNewWindow(g_wid, MENT_AREA_X, MENT_AREA_Y,
			MENT_AREA_WIDTH, MENT_AREA_HEIGHT, 0, 0, 0);
	if (g_ment_window) {
		g_pixmap_window = GrNewPixmap(MENT_PIXEL_MAP_WIDTH,
			MENT_AREA_HEIGHT, NULL);
	}

	ui_register_view(VIEW_ID_WEATHER,
		view_weather_entry, view_weather_draw,
		view_weather_exit, view_weather_key);
}

