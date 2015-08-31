/******************************************************************************
 * Filename:
 *   main.c
 *
 * Description:
 *   main of application
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-07-28
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "ui_manager.h"
#include "parking_view.h"
#include "security_view.h"
#include "light_view.h"
#include "gas_view.h"
#include "setup_view.h"
#include "main.h"
/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
app_status_t g_app_status;
int g_main_loop = TRUE;
int g_backlight_on = TRUE;

//------------------------------------------------------------------------------
// Function Name  : backlight_on()
// Description    : 
//------------------------------------------------------------------------------
static void backlight_on(void)
{
	if (g_backlight_on)
		return;

	g_backlight_on = TRUE;
	gpio_set(GPIOB, GPIO_BACKLIGHT);
	gpio_set(GPIOG, GPIO_LED);
}

//------------------------------------------------------------------------------
// Function Name  : init_setup()
// Description    : load the configuration data from nand drive
//------------------------------------------------------------------------------
static void init_setup(void)
{
	BOOL fRet;

	init_setup_data(&g_app_status);
	
	g_app_status.light_stat = 0;
	g_app_status.gas_stat = 0;	
	g_app_status.security_stat	= 0;	

	set_volume(g_app_status.volume);
	g_app_status.wallpad_type = WALLPAD_TYPE_EZV;
}

//------------------------------------------------------------------------------
// Function Name  : init_gui()
// Description    : 
//------------------------------------------------------------------------------
static BOOL init_gui()
{
	PRINT_FUNC_CO();

	init_ui_manager();
	
	ui_draw_rect(200, 140, 400, 200, BLACK, TRUE);
	ui_draw_text(200, 140, 400, 200, 24, WHITE,
		TXT_HALIGN_CENTER|TXT_VALIGN_MIDDLE, "���α׷� �ε���...");

	//step=20 x 7 = 140
	ui_draw_rect(330, 260, 140, 20, WHITE, FALSE);

	view_weather_init();

	ui_draw_rect(330, 260, 20, 20, WHITE, TRUE);

	view_elevator_init();

	ui_draw_rect(330, 260, 40, 20, WHITE, TRUE);

	view_parking_init();

	ui_draw_rect(330, 260, 60, 20, WHITE, TRUE);

	view_security_init();

	ui_draw_rect(330, 260, 80, 20, WHITE, TRUE);

	view_light_init();
	
	ui_draw_rect(330, 260, 100, 20, WHITE, TRUE);

	view_gas_init();

	ui_draw_rect(330, 260, 120, 20, WHITE, TRUE);

	view_setup_init();
	
	ui_draw_rect(330, 260, 140, 20, WHITE, TRUE);

	return TRUE;
}

//------------------------------------------------------------------------------
// Function Name  : init_comm()
// Description    : 
//------------------------------------------------------------------------------
void init_comm()
{
	g_app_status.wallpad_type = WALLPAD_TYPE_EZV;
	init_comm_ezv();
}

//------------------------------------------------------------------------------
// Function Name  : user_signal_handler()
// Description    : 
//------------------------------------------------------------------------------
static void user_signal_handler(int sig)
{
	switch (sig) {
	case SIGINT:
		printf("--> SIGINT Occured\r\n");
		g_main_loop = FALSE;
		kill(getpid(), SIGKILL);
		break;
	case SIGTERM:
		printf("--> SIGTERM Occured\r\n");
		g_main_loop = FALSE;
		kill(getpid(), SIGKILL);
		break;
	case SIGABRT:
		printf("--> SIGABRT Occured\r\n");
		g_main_loop = FALSE;
		kill(getpid(), SIGKILL);
		break;
	case SIGSEGV:
		printf("--> SIGSEGV Occured\r\n");
		g_main_loop = FALSE;
		kill(getpid(), SIGKILL);
		break;
	default:
		printf("--> Unknown Signal(0x%x) Occured\r\n", sig);
		kill(getpid(), SIGKILL);
		break;
	}
}

//------------------------------------------------------------------------------
// Function Name  : register_user_signal()
// Description    : 
//------------------------------------------------------------------------------
static void register_user_signal()
{
	if (signal(SIGINT, user_signal_handler) == SIG_IGN) 
		signal(SIGINT, SIG_IGN);

	if (signal(SIGTERM, user_signal_handler) == SIG_IGN) 
		signal(SIGTERM, SIG_IGN);

	if (signal(SIGABRT, user_signal_handler) == SIG_IGN) 
		signal(SIGABRT, SIG_IGN);

	if (signal(SIGSEGV, user_signal_handler) == SIG_IGN) 
		signal(SIGSEGV, SIG_IGN);
}

//------------------------------------------------------------------------------
// Function Name  : user_signal_unregister()
// Description    : 
//------------------------------------------------------------------------------
static void user_signal_unregister()
{
	signal (SIGINT, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
	signal (SIGABRT, SIG_IGN);
	signal (SIGSEGV, SIG_IGN);
}

//------------------------------------------------------------------------------
// Function Name  : microwave_event_handler()
// Description    : 
//------------------------------------------------------------------------------
static void microwave_event_handler()
{
	if (g_backlight_on) {
		g_timer.ResetTimer(BACK_LIGHT_TIMER);
	} else {
		g_state.ChangeState(STATE_WEATHER);
	}
}

//------------------------------------------------------------------------------
// Function Name  : app_init()
// Description    : 
//------------------------------------------------------------------------------
static BOOL app_init()
{	
	PRINT_FUNC();

	register_user_signal();

	init_parking_info();
	
	init_setup();
	
	init_gui();

	init_msg();

	init_gpio_controller();

	init_key_controller();
	
	init_microwave_controller(g_app_status.adc_value);
	
	init_watchdog_controller();
	
	init_comm();
		
	gpio_clr(GPIOG, GPIO_AMP);	// AMP OFF

	wdt_set_period(MAX_WATCHDOG_PERIOD);
	
	wdt_enable();
	
	return TRUE;
}

//------------------------------------------------------------------------------
// Function Name  : main()
// Description    : 
//------------------------------------------------------------------------------
int main(int arg_gc, char *argv[])
{
	mtm_msg_t msg;
	u16 type, code;
		
	app_init();

	ui_switch_to(VIEW_ID_WEATHER);
	
	DBG_MSG("Starting Main Loop..\r\n");
	
	while (g_main_loop) {
		if (msg_rcv(&msg) > 0) {
			switch (msg.msg_id) {
			case MSG_EVENT_UI_SWITCH:
				ui_change_view(msg.param);
				break;
			case MSG_EVENT_MICROWAVE:
				break;
			case MSG_EVENT_DOOR:
				g_app_status.door_opened = msg.param;
				break;
			case MSG_DATA_WALLPAD_PKT:
				ui_draw_view();
				break;
			}
		}

		if (check_key_event(&type, &code))
			ui_operate_key(type, code);

		wdt_refresh();
		
		usleep(1000);
	}
	
	return 0;
}

