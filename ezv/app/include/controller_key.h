#ifndef __CTRL_KEY_H__
#define __CTRL_KEY_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
enum {
	KEY_RIGHT_BOTTOM = 0,
	KEY_RIGHT_MIDDLE,
	KEY_RIGHT_TOP,
	KEY_LEFT_BOTTOM,
	KEY_LEFT_MIDDLE,
	KEY_LEFT_TOP,
	KEY_VOL_UP,
	KEY_VOL_DOWN
};

enum {
	KEY_TYPE_SHORT = 0,
	KEY_TYPE_LONG
};

#define SET_KEY_CODE(c, b)	((c) |= (1 << b))
#define CLR_KEY_CODE(c, b)	((c) &= (~(1 << b)))
#define CHK_KEY_CODE(c, b)	((c) & (1 << b))

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void init_key_controller(void);
extern u8 check_key_event(u16 *_type, u16 *_code);

#endif	// __CTRL_KEY_H__

