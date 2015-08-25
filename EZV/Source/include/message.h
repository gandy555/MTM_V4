#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define MSG_KEY_MAIN					0xA000
#define MSG_TYPE_SYSTEM	123
//#define MSG_TYPE_UI			456

enum {
	MSG_EVENT_UI_SWITCH = 0,
	MSG_EVENT_MICROWAVE = 0,
	MSG_DATA_WALLPAD_PKT,
	MAX_MSG_SIZE
};

typedef struct {
	long	msg_type;
	int msg_id;
	void *buf_ptr;
	int buf_size;
	int param;
} mtm_msg_t;

#define MSG_INIT(m)	\
	do {					\
		memset(&m, 0, sizeof(mtm_msg_t));	\
		m.msg_type = MSG_TYPE_SYSTEM	\
	} while (0)
	
/******************************************************************************
 * Function Export
 ******************************************************************************/
extern int init_msg(void);
extern int msg_send(mtm_msg_t *_msg);
extern int msg_rcv(mtm_msg_t *_msg);

#endif //__MESSAGE_H__
