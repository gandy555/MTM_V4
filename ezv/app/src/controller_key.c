/******************************************************************************
 * Filename:
 *   controller_key.c
 *
 * Description:
 *   key monitoring module
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-31
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
#define LONG_KEY_DETECT_TICK			2000	//2ÃÊ

static pthread_t g_door_thread;
static pthread_mutex_t key_mutex = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void init_key_controller(void);

//------------------------------------------------------------------------------
// Function Name  : update_key_code()
// Description    :
//------------------------------------------------------------------------------
static void update_key_code(u8 _id, u16 *_code)
{
	u8 state = 0;

	switch (_id) {
	case KEY_RIGHT_BOTTOM:
		state = gpio_get(GPIOJ, GPIO_RIGHT_BOTTOM);
		break;
	case KEY_RIGHT_MIDDLE:
		state = gpio_get(GPIOJ, GPIO_RIGHT_MIDDLE);
		break;
	case KEY_RIGHT_TOP:
		state = gpio_get(GPIOJ, GPIO_RIGHT_TOP);
		break;
	case KEY_LEFT_BOTTOM:
		state = gpio_get(GPIOJ, GPIO_LEFT_BOTTOM);
		break;
	case KEY_LEFT_MIDDLE:
		state = gpio_get(GPIOJ, GPIO_LEFT_MIDDLE);
		break;
	case KEY_LEFT_TOP:
		state = gpio_get(GPIOJ, GPIO_LEFT_TOP);
		break;
	case KEY_VOL_UP:
		state = gpio_get(GPIOF, GPIO_VOL_UP);
		break;
	case KEY_VOL_DOWN:
		state = gpio_get(GPIOF, GPIO_VOL_DOWN);
		break;
	default:
		return;
	}
	
	if (state)
		SET_KEY_CODE(*_code, KEY_RIGHT_BOTTOM);
	else
		CLR_KEY_CODE(*_code, KEY_RIGHT_BOTTOM);	
}

//------------------------------------------------------------------------------
// Function Name  : door_thread()
// Description    :
//------------------------------------------------------------------------------
static void *door_thread(void *_para)
{
	u8 curr_door = 0, prev_door = 0;
	mtm_msg_t msg;

	MSG_INIT(msg);
	msg.msg_id = MSG_EVENT_DOOR;	
	for (;;) {
		curr_door = gpio_get(GPIOF, GPIO_DOOR_DETECT);

		if (curr_door != prev_door) {
			msg.param = curr_door;
			msg_send(&msg); 
		}
		prev_door = curr_door;

		usleep(50000);
	}
}

//------------------------------------------------------------------------------
// Function Name  : check_key_event()
// Description    :
//------------------------------------------------------------------------------
u8 check_key_event(u16 *_type, u16 *_code)
{
	u8 state, res = 0;
	u16 curr_code = 0;
	u32 elapsed_tick = 0;
	static u8 long_key_f = 0;
	static u16 prev_code = 0;
	static u32 start_tick = 0;
	
	update_key_code(KEY_RIGHT_BOTTOM, &curr_code);
	update_key_code(KEY_RIGHT_MIDDLE, &curr_code);
	update_key_code(KEY_RIGHT_TOP, &curr_code);

	update_key_code(KEY_LEFT_BOTTOM, &curr_code);
	update_key_code(KEY_LEFT_MIDDLE, &curr_code);
	update_key_code(KEY_LEFT_TOP, &curr_code);

	update_key_code(KEY_VOL_UP, &curr_code);
	update_key_code(KEY_VOL_DOWN, &curr_code);

	if (curr_code) {
		if (curr_code != prev_code) 
			start_tick = get_mono_time();

		elapsed_tick = get_elapsed_time(start_tick);
		if ((!long_key_f) && (elapsed_tick >= LONG_KEY_DETECT_TICK)) {
			long_key_f = 1;
			*_code = curr_code;
			*_type = KEY_TYPE_LONG;
			res = 1;
		}
	} else {
		if ((!long_key_f) && (prev_code)) {	// released key
			*_code = prev_code;
			*_type = KEY_TYPE_SHORT;
			res = 1;
		}
		long_key_f = 0;
	}
	
	prev_code = curr_code;

	return res;
}
	
//------------------------------------------------------------------------------
// Function Name  : init_key_controller()
// Description    :
//------------------------------------------------------------------------------
void init_key_controller(void)
{
	int res;

	res = pthread_create(&g_door_thread, NULL, door_thread, NULL);
	if (res) {
		DBG_MSG_CO(CO_RED, "<%s> thread create failed!\r\n", __func__);
	}
}

