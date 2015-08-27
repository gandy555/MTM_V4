/******************************************************************************
 * Filename:
 *   message.c
 *
 * Description:
 *   communication with host control manager
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-08-12
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "message.h"
/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
static key_t g_msg_key;

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
int init_msg(void);
int msg_send(mtm_msg_t *_msg);
int msg_rcv(mtm_msg_t *_msg);

//------------------------------------------------------------------------------
// Function Name  : init_msg()
// Description    :
//------------------------------------------------------------------------------
int init_msg(void)
{
	g_msg_key = msgget(MSG_KEY_MAIN, IPC_CREAT|0666);
	if (g_msg_key < 0) {
		DBG_MSG_CO(CO_RED, "msgget error=%d, %s\r\n", errno, strerror(errno));
	}

	return g_msg_key;
}

//------------------------------------------------------------------------------
// Function Name  : msg_send()
// Description    :
//------------------------------------------------------------------------------
int msg_send(mtm_msg_t *_msg)
{
	int ret;

	//msgsnd retun 0 on success
	ret = msgsnd(g_msg_key, (void *)_msg,
		sizeof(mtm_msg_t)-sizeof(long), IPC_NOWAIT);

	return ret;
}

//------------------------------------------------------------------------------
// Function Name  : msg_rcv()
// Description    :
//------------------------------------------------------------------------------
int msg_rcv(mtm_msg_t *_msg)
{
	return msgrcv(g_msg_key, (void *)_msg,
		sizeof(mtm_msg_t)-sizeof(long), MSG_TYPE_SYSTEM, IPC_NOWAIT);
}

