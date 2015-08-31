#ifndef __MAIN_H__
#define __MAIN_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#include "type.h"

enum {
	MTM_DATA_EV_STATUS_STOP,
	MTM_DATA_EV_STATUS_UP,
	MTM_DATA_EV_STATUS_DOWN,
	MTM_DATA_EV_STATUS_ARRIVE,
	MTM_DATA_EV_STATUS_ERROR,
	MTM_DATA_EV_STATUS_COUNT
};

// major.minor.revision
#define RELEASE_VERSION	"v0.01.A"
#define RELEASE_DATE		"20150812"
#define RELEASE_PRODUCT	"for EZV"

typedef void (*key_handler)(u16 type, u16 code);
/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void register_key_handler(u8 _id, key_handler _handler);
extern void register_workqueue(u32 _period, work_handler _handler);
extern void unregister_workqueue(work_handler _handler);
extern void ui_switch_to(u8 _id);
#endif //__MAIN_H__

