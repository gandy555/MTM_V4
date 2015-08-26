/******************************************************************************
 * Filename:
 *   duo_utils.c
 *
 * Description:
 *   collect the util functions
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-25
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
#define MAX_TICK	86400000

static char g_volume_tbl[9][6] = {
	"f0f0\0", "1818\0", "1212\0", "1010\0", "0808\0",
	"0707\0", "0606\0", "0303\0", "0000\0"
};

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void set_rect(RECT* pRect, int x, int y, int w, int h);
u32 get_mono_time(void);
u32 get_elapsed_time(u32 _p_time);
void set_volume(u32 _lvl);
void play_wav_file(const char* _file_name);
void dbg_msg(const char *fmt, ...);
void dbg_msg_co(int color, const char *fmt, ...);

//------------------------------------------------------------------------------
// Function Name  : set_rect()
// Description    :
//------------------------------------------------------------------------------
void set_rect(RECT* pRect, int x, int y, int w, int h)
{
	if (pRect) {
		pRect->x = x;
		pRect->y = y;
		pRect->w = w;
		pRect->h = h;
	}
}

//------------------------------------------------------------------------------
// Function Name  : get_mono_time()
// Description    :
//------------------------------------------------------------------------------
u32 get_mono_time(void)
{
	struct timespec ts;
	u32 c_tick;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	// tv_sec값을 1일단위(86400초)로 자른다음 ms로 환산하고
	// tv_usec값도 ms로 환산하여 더한값을 tick count로 한다
	c_tick = (ts.tv_sec % 86400)*1000 + ts.tv_usec/1000;

	return c_tick;
}

//------------------------------------------------------------------------------
// Function Name  : get_elapsed_time()
// Description    :
//------------------------------------------------------------------------------
u32 get_elapsed_time(u32 _p_time)
{
	u32 c_time, e_time;

	if (_p_time > MAX_TICK)
		return 0;	//Error Case

	c_time = get_mono_time();

	if (c_time >= _p_time)
		e_time = c_time - _p_time;
	else
		e_time = MAX_TICK - _p_time + c_time;

	return e_time;
}

//------------------------------------------------------------------------------
// Function Name  : set_volume()
// Description    : 
//------------------------------------------------------------------------------
void set_volume(u32 _lvl)
{
	char cmd[128] = {0,};

	if (_lvl <= MAX_VOLUME_LEVEL) {
		sprintf(cmd, "echo %s > /proc/driver/wm9713/04-AC97_HEADPHONE_VOL",
			g_volume_tbl[_lvl]);
		system(cmd);
	//	play_wavplay("/app/sound/touch.wav");
	}
}

//------------------------------------------------------------------------------
// Function Name  : set_volume()
// Description    : 
//------------------------------------------------------------------------------
void play_wav_file(const char* _file_name)
{
	char cmd[128] = {0,};

	system("killall -q wavplay\0");

	sprintf(cmd, "/app/bin/wavplay -d /dev/sound_dsp -q %s & \0", _file_name);

	system(cmd);
	printf("Wav Playing : %s\r\n", _file_name);
}

//------------------------------------------------------------------------------
// Function Name  : log_uptime()
// Description    :
//------------------------------------------------------------------------------
static unsigned long log_uptime(void)
{
	unsigned long utime;
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	utime = (t.tv_sec * 1000) 				// sec * 1000 -> milli
			+ (t.tv_nsec / 1000 / 1000);	// nano / 1000 / 1000 -> milli

	return utime;
}

//------------------------------------------------------------------------------
// Function Name  : dbg_msg()
// Description    :
//------------------------------------------------------------------------------
void dbg_msg(const char *fmt, ...)
{
	char _buf[256] = {0,};
	va_list args;
	int len;

	sprintf(_buf, "%s[%8u] ", col_str[CO_DEF], log_uptime());

	len = strlen(_buf);
	va_start(args, fmt);
	vsnprintf(_buf + len, 256 - len - 1, fmt, args);
	va_end(args);
}

//------------------------------------------------------------------------------
// Function Name  : dbg_msg_co()
// Description    :
//------------------------------------------------------------------------------
void dbg_msg_co(int color, const char *fmt, ...)
{
	char _buf[256] = {0,};
	va_list args;
	int len;

	switch (color) {
	case CO_RED:
	case CO_GREEN:
	case CO_YELLOW:
	case CO_BLUE:
	case CO_MAGENTA:
	case CO_CYAN:
	case CO_WHITE:
		sprintf(_buf, "%s[%8u] ", col_str[color], log_uptime());
		break;
	case CO_DEF:
	default:
		sprintf(_buf, "%s[%8u] ", col_str[CO_DEF], log_uptime());
		break;
	}

	len = strlen(_buf);
	va_start(args, fmt);
	vsnprintf(_buf + len, 256 - len - 1, fmt, args);
	va_end(args);
	fprintf(stdout, "%s%s\n", _buf, col_str[CO_DEF]);
}

