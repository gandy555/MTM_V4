#ifndef __DUO_UTILS_H__
#define __DUO_UTILS_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define DBG_MSG_ON

#define CO_RED		1
#define CO_GREEN	2
#define CO_YELLOW	3
#define CO_BLUE		4
#define CO_MAGENTA	5
#define CO_CYAN		6
#define CO_WHITE	7
#define CO_DEF		8

#define MAX_VOLUME_LEVEL	8

#if defined(DBG_MSG_ON)
#define DBG_MSG_CO(p1, p2, p3 ...)			dbg_msg_co(p1, p2, p3)
#define DBG_MSG(p1, p2 ...)					dbg_msg(p1, p2)
#define PRINT_FUNC()							dbg_msg("<%s>", __func__)
#define PRINT_FUNC_CO()						dbg_msg_co(CO_BLUE, "<%s>", __func__)
#else
#define DBG_MSG_CO(p1, p2, p3 ...)
#define DBG_MSG(p1, p2 ...)
#endif

/******************************************************************************
 * Function Export
 ******************************************************************************/
#if defined(DBG_MSG_ON)
extern void dbg_msg(const char *fmt, ...);
extern void dbg_msg_co(int color, const char *fmt, ...);
#endif
extern void set_volume(u32 _lvl);
#endif // __DUO_UTILS_H__

