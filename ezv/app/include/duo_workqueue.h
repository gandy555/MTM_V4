#ifndef __TW_WORKQUEUE_H__
#define __TW_WORKQUEUE_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define INFINITE		0xFF
#define UNDELAYED		0
#define DELAYED			1

#define MAX_NAME_SIZE	20

typedef void (*work_handler)(u32 _param);
typedef void (*fail_handler)(void);

typedef struct workqueue_info {
	u32 c_time;
	u32 wait_time;
	u8 retry_cnt;
	u32 param;
	fail_handler fail_func;
	work_handler work_func;
} workqueue_info_t;

typedef struct workqueue_node{
	struct workqueue_info curr;
	struct workqueue_node* prev;
	struct workqueue_node* next;
} workqueue_node_t;

typedef struct {
	struct workqueue_node *head;
	struct workqueue_node *tail;
	u32 p_time;
	int cnt;
	char name[MAX_NAME_SIZE];
} workqueue_list_t;

/******************************************************************************
 * Function Export (low level functions)
 ******************************************************************************/
extern workqueue_node_t* workqueue_find_node(
		workqueue_list_t *_list_h, work_handler _work_func);
extern int workqueue_delete_node(
		workqueue_list_t *_list_h, workqueue_node_t* _node);

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern u8 workqueue_list_dump(workqueue_list_t *_list_h);
extern u8 workqueue_delete_all(workqueue_list_t *_list_h);
extern void workqueue_refresh_time(workqueue_node_t *_node);
extern void workqueue_reset_time(workqueue_node_t *_node);
extern int workqueue_head_proc(workqueue_list_t *_list_h);
extern int workqueue_all_proc(workqueue_list_t *_list_h);
extern u8 workqueue_register(
	workqueue_list_t *_list_h, workqueue_info_t *_wq_info);
extern u8 workqueue_register_delayed(workqueue_list_t *_list_h, u32 _period,
	u8 _cnt, u32 _param, fail_handler fail_func, work_handler work_func);
extern u8 workqueue_register_undelayed(workqueue_list_t *_list_h, u32 _period,
	u8 _cnt, u32 _param, fail_handler fail_func, work_handler work_func);
extern u8 workqueue_unregister(
	workqueue_list_t *_list_h, work_handler work_func);
extern u8 workqueue_check(
	workqueue_list_t *_list_h, work_handler work_func);
extern u32 workqueue_param(
	workqueue_list_t *_list_h, work_handler work_func);
extern u8 workqueue_register_default(workqueue_list_t *_list_h, 
	u32 _param, work_handler _work_func);
extern workqueue_list_t *workqueue_create(char *_name);
extern u8 workqueue_destroy(workqueue_list_t **_handle);
#endif // __TW_WORKQUEUE_H__

