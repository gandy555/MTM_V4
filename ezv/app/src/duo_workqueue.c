/******************************************************************************
 * Filename:
 *   tw_workqueue.c
 *
 * Description:
 *   workqueue
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-03-23
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <basedef.h>
#include <time.h>
#include <kutil.h>
#include <mqueue.h>
#include <ucld.h>
#include <fio/partition.h>
#include <fio/fw_part.h>
#include <ui/ambsh.h>
#include <comsvc/utils.h>
#include <app3/framework/defines.h>
#include <app3/apps/apps.h>
#include <app3/tw_utils/tw_utils.h>
#include "tw_workqueue.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
#define WORKQUEUE_DBG_ENABLE
#define WORKQUEUE_ERR_DBG_ENABLE

#if defined(WORKQUEUE_DBG_ENABLE)
#define WORKQUEUE_DBG(fmt, args ...)		DBG_MSG_CO(CO_BLUE, "[WORKQUEUE] " fmt, ## args)  
#else
#define WORKQUEUE_DBG(fmt, args ...)	
#endif

#if defined(WORKQUEUE_ERR_DBG_ENABLE)
#define WORKQUEUE_ERR_DBG(fmt, args ...)		DBG_MSG_CO(CO_RED, "[WORKQUEUE] " fmt, ## args)  
#else
#define WORKQUEUE_ERR_DBG(fmt, args ...)	
#endif

#define MAX_WORKQUEUE_LIST		100

#define HANDLE_ASSERT(h) {							\
	if (h == NULL)	{									\
		DBG_MSG_CO(CO_RED, "<%s> Invalid Handle", __func__);	\
		return 0;									\
	}												\
}

/******************************************************************************
 *
 * Functions Declaration
 *
 ******************************************************************************/
int workqueue_delete_node(workqueue_list_t *_list_h, 
					workqueue_node_t* _node);
workqueue_node_t* workqueue_find_node(workqueue_list_t *_list_h, 
					work_handler _work_func);
//------------------------------------------------------------------------------
// Function Name  : workqueue_append_node()
// Description    :
//------------------------------------------------------------------------------
static void workqueue_append_node(workqueue_list_t *_list_h, 
			workqueue_node_t* _node)
{
	_node->next = NULL;
	_node->prev = NULL;
	if (_list_h->head == NULL) {
		_list_h->head = _node;
		_list_h->tail = _node;
	} else {
		_node->prev = _list_h->tail;
		_list_h->tail->next = _node;
		_list_h->tail = _node;
	}
	_list_h->cnt += 1;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_make_node()
// Description    :
//------------------------------------------------------------------------------
static workqueue_node_t* workqueue_make_node(
								workqueue_info_t* _info)
{
	workqueue_node_t* new_node = NULL;
	
	new_node = (workqueue_node_t*)malloc(sizeof(workqueue_node_t));
	if (new_node != NULL) {
		memset(new_node, 0, sizeof(workqueue_node_t));
		memcpy(&new_node->curr, _info, sizeof(workqueue_info_t));
	}
	return new_node;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_work()
// Description    :
//------------------------------------------------------------------------------
static u8 workqueue_work(workqueue_list_t *_list_h, workqueue_node_t* _node)
{
	_node->curr.work_func(_node->curr.param);
	_node->curr.c_time = 0;
	if (_node->curr.retry_cnt != INFINITE) {
		if (--_node->curr.retry_cnt == 0) {
			if (_node->curr.fail_func != NULL)
				_node->curr.fail_func();
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_list_dump()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_list_dump(workqueue_list_t *_list_h)
{
	workqueue_node_t* node;
	int i;

	HANDLE_ASSERT(_list_h);
	
	node = _list_h->head;
	WORKQUEUE_DBG("=================================");
	WORKQUEUE_DBG("= %s =", _list_h->name);
	WORKQUEUE_DBG("=================================");
	for (i = 0; i < _list_h->cnt; i++) {			
		WORKQUEUE_DBG("--------------------------------------");
		WORKQUEUE_DBG("wait_time: %d", node->curr.wait_time);
		WORKQUEUE_DBG("retry_cnt: %d", node->curr.retry_cnt);
		WORKQUEUE_DBG("work_func: %x", node->curr.work_func);			
		WORKQUEUE_DBG("--------------------------------------");			
		node = node->next;
	}
	WORKQUEUE_DBG("=================================");

	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_find_node()
// Description    :
//------------------------------------------------------------------------------
workqueue_node_t* workqueue_find_node(workqueue_list_t *_list_h, 
					work_handler _work_func)
{
	workqueue_node_t* node;
	int i;

	HANDLE_ASSERT(_list_h);
	
	node = _list_h->head;
	for (i = 0; i < _list_h->cnt; i++) {
		if (node->curr.work_func == _work_func) {
			return node;
		}
		node = node->next;
	}

	return (workqueue_node_t *)NULL;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_delete_node()
// Description    :
//------------------------------------------------------------------------------
int workqueue_delete_node(workqueue_list_t *_list_h, 
		workqueue_node_t* _node)
{
	if (_node == NULL) {
		WORKQUEUE_ERR_DBG("<%s> failed(%s: %d)", __func__, 
			_list_h->name, _list_h->cnt);
		return 1;
	}

	if (!_list_h->cnt) {
		WORKQUEUE_ERR_DBG("<%s> failed(%s: %d)", __func__, 
			_list_h->name, _list_h->cnt);
		return 1;
	}
	
	if (_list_h->head == _node) {
		if (_node->next != NULL) {
			_list_h->head = _node->next;
			_list_h->head->prev = NULL;
		} else {
			_list_h->head = NULL;
			_list_h->tail = NULL;
		}
		_list_h->cnt -= 1;
		free(_node);			
	} else {		
		if (_list_h->tail == _node)
			_list_h->tail = _node->prev;

		if (_node->prev != NULL)
			_node->prev->next = _node->next;
		if (_node->next != NULL)
			_node->next->prev = _node->prev;
		
		_list_h->cnt -= 1;
		free(_node);
	}

	WORKQUEUE_DBG("<%s> %s(%d)", __func__, 
		_list_h->name, _list_h->cnt);
	
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_register()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_register(workqueue_list_t *_list_h, 
	workqueue_info_t *_wq_info)
{
	workqueue_node_t* new_node = NULL;
	
	WORKQUEUE_DBG("<%s> %s(%d)", __func__, 
		_list_h->name, _list_h->cnt);
	
	HANDLE_ASSERT(_list_h);
		
	if (_list_h->cnt >= MAX_WORKQUEUE_LIST) {
		WORKQUEUE_ERR_DBG("list is full");
		return 0;
	}
	
	new_node = workqueue_make_node(_wq_info);
	if (new_node == NULL) {
		WORKQUEUE_ERR_DBG("make failed");
		return 0;
	}
	
	workqueue_append_node(_list_h, new_node);

	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_register_default()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_register_default(workqueue_list_t *_list_h, 
	u32 _param, work_handler _work_func)
{
	workqueue_info_t workqueue;
	
	HANDLE_ASSERT(_list_h);
	
	workqueue.c_time = 0;
	workqueue.wait_time = 3000;
	workqueue.retry_cnt = 1;
	workqueue.param = _param;
	workqueue.fail_func = NULL;
	workqueue.work_func = _work_func;
	
	return workqueue_register(_list_h, &workqueue);
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_register_delayed()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_register_delayed(workqueue_list_t *_list_h, u32 _period,
	u8 _cnt, u32 _param, fail_handler fail_func, work_handler work_func)
{
	workqueue_info_t workqueue;
	
	HANDLE_ASSERT(_list_h);
	
	workqueue.c_time = 0;
	workqueue.wait_time = _period;
	workqueue.retry_cnt = _cnt;
	workqueue.param = _param;
	workqueue.fail_func = fail_func;
	workqueue.work_func = work_func;
	
	return workqueue_register(_list_h, &workqueue);
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_register_delayed()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_register_undelayed(workqueue_list_t *_list_h, u32 _period,
	u8 _cnt, u32 _param, fail_handler fail_func, work_handler work_func)
{
	workqueue_info_t workqueue;
	
	HANDLE_ASSERT(_list_h);
	
	workqueue.c_time = _period;
	workqueue.wait_time = _period;
	workqueue.retry_cnt = _cnt;
	workqueue.param = _param;
	workqueue.fail_func = fail_func;
	workqueue.work_func = work_func;
	
	return workqueue_register(_list_h, &workqueue);
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_unregister()
// Description    : 
//------------------------------------------------------------------------------
u8 workqueue_unregister(workqueue_list_t *_list_h, work_handler work_func)
{
	workqueue_node_t *node;

	HANDLE_ASSERT(_list_h);
	
	node = workqueue_find_node(_list_h, work_func);
	if (node != NULL) {
		workqueue_delete_node(_list_h, node);
		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_check()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_check(workqueue_list_t *_list_h, work_handler work_func)
{
	workqueue_node_t *node;

	HANDLE_ASSERT(_list_h);
	
	node = workqueue_find_node(_list_h, work_func);
	if (node != NULL)
		return 1;
	
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_delete_all()
// Description    :
//------------------------------------------------------------------------------
u8 workqueue_delete_all(workqueue_list_t *_list_h)
{
	workqueue_node_t *node = NULL, *n_node = NULL;
	int i, total_cnt;

	HANDLE_ASSERT(_list_h);
	WORKQUEUE_DBG("<%s> %s(%d)", __func__, 
		_list_h->name, _list_h->cnt);	
	
	node = _list_h->head;
	total_cnt = _list_h->cnt;
	for (i = 0; i < total_cnt; i++) {
		n_node = node->next;
		workqueue_delete_node(_list_h, node);
		node = n_node;
	}

	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_refresh_time()
// Description    :
//------------------------------------------------------------------------------
void workqueue_refresh_time(workqueue_node_t *_node)
{		
	_node->curr.c_time = _node->curr.wait_time;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_reset_time()
// Description    :
//------------------------------------------------------------------------------
void workqueue_reset_time(workqueue_node_t *_node)
{		
	_node->curr.c_time = 0;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_head_proc()
// Description    : proccessing sequential work queue
//------------------------------------------------------------------------------
void workqueue_head_proc(workqueue_list_t *_list_h)
{
	workqueue_node_t* node = _list_h->head;
	SYSTIM curr_time;
	int diff;

	if (node == NULL)
		return;

	get_tim(&curr_time);	
	if ((curr_time.ltime - _list_h->p_time.ltime) == 0) 
		return;
	
	diff = curr_time.ltime - _list_h->p_time.ltime;
	node->curr.c_time += diff;
	if (node->curr.c_time > node->curr.wait_time) {
		if (workqueue_work(_list_h, node))
			workqueue_delete_node(_list_h, node);
	}
	_list_h->p_time = curr_time;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_all_proc()
// Description    : proccessing all of work queue
//------------------------------------------------------------------------------
void workqueue_all_proc(workqueue_list_t *_list_h)
{
	workqueue_node_t *node, *n_node = NULL;
	SYSTIM curr_time;
	int diff, need_delete;

	node = _list_h->head;
	if (node == NULL)
		return;

	get_tim(&curr_time);
	if ((_list_h->p_time.utime == 0) && (_list_h->p_time.ltime == 0))
		_list_h->p_time = curr_time;
	
	if ((curr_time.ltime - _list_h->p_time.ltime) == 0) 
		return;
	
	diff = curr_time.ltime - _list_h->p_time.ltime;
	while (node != NULL) {
		need_delete = 0;
		node->curr.c_time += diff;
		n_node = node->next;		
		if (node->curr.c_time > node->curr.wait_time)
			need_delete = workqueue_work(_list_h, node);
		if (need_delete)
			workqueue_delete_node(_list_h, node);
		node = n_node;
	}
	_list_h->p_time = curr_time;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_create()
// Description    : create the workqueue handle 
//------------------------------------------------------------------------------
workqueue_list_t *workqueue_create(char *_name)
{
	workqueue_list_t *new_h = NULL;

	if ((_name != NULL) && (strlen(_name) >= MAX_NAME_SIZE)) {
		WORKQUEUE_ERR_DBG("Invalid handle name");
		return NULL;
	}
	
	new_h = malloc(sizeof(workqueue_list_t));	
	if (new_h != NULL) {
		memset(new_h, 0, sizeof(workqueue_list_t));
		if (_name != NULL)
			strcpy(new_h->name, _name);
		else
			strcpy(new_h->name, "UNKNOWN");
	}

	WORKQUEUE_DBG("<%s> %s", __func__, new_h->name);
	return new_h;
}

//------------------------------------------------------------------------------
// Function Name  : workqueue_destroy()
// Description    : destroy the workqueue handle 
//------------------------------------------------------------------------------
u8 workqueue_destroy(workqueue_list_t *_handle)
{	
	u8 res = 0;
	
	if (_handle != NULL) {
		WORKQUEUE_DBG("<%s> %s", __func__, _handle->name);				
		workqueue_delete_all(_handle);
		free(_handle);
		_handle = NULL;
		res = 1;
	}

	return res;
}

