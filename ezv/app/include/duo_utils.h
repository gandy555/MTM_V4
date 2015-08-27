#ifndef __DUO_UTILS_H__
#define __DUO_UTILS_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define CO_RED		1
#define CO_GREEN	2
#define CO_YELLOW	3
#define CO_BLUE		4
#define CO_MAGENTA	5
#define CO_CYAN		6
#define CO_WHITE	7
#define CO_DEF		8

#define MAX_VOLUME_LEVEL	8

#define DBG_MSG_CO(p1, p2, p3 ...)			dbg_msg_co(p1, p2, p3)
#define DBG_MSG(p1, p2 ...)					dbg_msg(p1, p2)
#define PRINT_FUNC()							dbg_msg("<%s>", __func__)
#define PRINT_FUNC_CO()						dbg_msg_co(CO_BLUE, "<%s>", __func__)

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void set_rect(RECT* pRect, int x, int y, int w, int h);
extern u32 get_mono_time(void);
extern u32 get_elapsed_time(u32 _p_time);
extern void set_volume(u32 _lvl);
extern void play_wav_file(const char* _file_name);
extern void dbg_msg(const char *fmt, ...);
extern void dbg_msg_co(int color, const char *fmt, ...);

#endif // __DUO_UTILS_H__

