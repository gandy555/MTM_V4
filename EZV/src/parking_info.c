/******************************************************************************
 * Filename:
 *   parking_info.c
 *
 * Description:
 *   management the parking information
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-07-28
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include <pthread.h>
#include "parking_info.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
struct parking_node{
	parking_info_t curr;
	struct parking_node* prev;
	struct parking_node* next;
};

struct parking_info_list {
	struct parking_node *head;
	struct parking_node *tail;
	int cnt;
};

struct parking_info_list g_parking_info_list = {
	NULL,
	NULL,
	0,
};

pthread_mutex_t g_parking_mutex;
//------------------------------------------------------------------------------
// Function Name  : parking_list_dump()
// Description    : 
//------------------------------------------------------------------------------
static void parking_list_dump(void)
{
	struct parking_info_list* plist = &g_parking_info_list;
	struct parking_node* node;
	int i;

	printf("<%s>\r\n", __func__);				
	
	if (plist != NULL) {
		node = plist->head;
		printf("---------------------------------------------\r\n");
		for (i = 0; i < plist->cnt; i++) {			
			printf("ID : %02d%02d%02d%02d\r\n", node->curr.car_num[0], 
					node->curr.car_num[1], node->curr.car_num[2], node->curr.car_num[3]);
		
			printf("floor : %s, zone : %s\r\n", node->curr.floor, node->curr.zone);
			printf("---------------------------------------------\r\n");			
			node = node->next;
		}
	}
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_find_node()
// Description    : 
//------------------------------------------------------------------------------
static struct parking_node* parking_list_find_node(char* _card_id)
{
	struct parking_info_list* plist = &g_parking_info_list;
	struct parking_node* node;
	int i;

	if (plist != NULL) {
		node = plist->head;
		for (i = 0; i < plist->cnt; i++) {
			if (memcmp(node->curr.car_num, _card_id, MAX_CAR_NUM_SIZE) == 0) {
				return node;
			}
			node = node->next;
		}
	}

	return (struct parking_node *)NULL;
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_add_tail()
// Description    : 
//------------------------------------------------------------------------------
static void parking_list_add_tail(struct parking_node* _node)
{
	struct parking_info_list* plist = &g_parking_info_list;

	_node->next = NULL;
	_node->prev = NULL;
	if (plist->head == NULL) {
		plist->tail = _node;
		plist->head = _node;
	} else {
		_node->prev = plist->tail;
		plist->tail->next = _node;
		plist->tail = _node;
	}
	plist->cnt += 1;
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_add_head()
// Description    : 
//------------------------------------------------------------------------------
static void parking_list_add_head(struct parking_node* _node)
{
	struct parking_info_list* plist = &g_parking_info_list;

	_node->prev = NULL;
	_node->next = NULL;
	if (plist->head == NULL) {
		plist->head = _node;
		plist->tail = _node;
	} else {
		plist->head->prev = _node;	
		_node->next = plist->head;
		plist->head = _node;
	}

	plist->cnt += 1;
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_delete()
// Description    : 
//------------------------------------------------------------------------------
static int parking_list_delete(struct parking_node* _node)
{
	struct parking_info_list* plist = &g_parking_info_list;
		
	if (_node == NULL) {
		printf("<%s> delete failed(%d)\r\n", __func__, plist->cnt);
		return 1;
	}
	
	if (plist->head == _node) {
		if (_node->next != NULL) {
			plist->head = _node->next;
			plist->head->prev = NULL;
		} else {
			plist->head = NULL;
			plist->tail = NULL;
		}
		plist->cnt -= 1;
		free(_node);			
	} else {		
		if (plist->tail == _node)
			plist->tail = _node->prev;

		if (_node->prev != NULL)
			_node->prev->next = _node->next;
		if (_node->next != NULL)
			_node->next->prev = _node->prev;
		
		plist->cnt -= 1;
		free(_node);
	}
	printf("<%s> delete success(%d)\r\n", __func__, plist->cnt);
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_make_node()
// Description    : 
//------------------------------------------------------------------------------
static struct parking_node* parking_list_make_node(parking_info_t* _info)
{
	struct parking_node* pNew = NULL;
	
	pNew = (struct parking_node*)malloc(sizeof(struct parking_node));
	if (pNew != NULL) {
		memset(pNew, 0, sizeof(struct parking_node));
		memcpy(&pNew->curr, _info, sizeof(parking_info_t));
	}
	return pNew;
}

//------------------------------------------------------------------------------
// Function Name  : byte_2_ascii()
// Description    : 
//------------------------------------------------------------------------------
static char byte_2_ascii(UCHAR _byte)
{
	char res = 0;

	res = _byte + 0x30;
	
	return res;	
}

//------------------------------------------------------------------------------
// Function Name  : parking_item_to_str()
// Description    : 
//------------------------------------------------------------------------------
int parking_item_to_str(int _id, char* _src, char* _dst)
{
	int i, res = 0;
	
	switch (_id) {
	case PARKING_ITM_ID:
		memcpy(_dst, _src, 4);
		break;
	case PARKING_ITM_FLOOR:
		sprintf(_dst, "%c%d", _src[0], _src[1]);
		break;
	case PARKING_ITM_ZONE:
		_dst[0] = _src[0];
		for (i = 1; i < 6; i++) {
			if (_src[i] == 0xFF)
				break;
			_dst[i] = byte_2_ascii(_src[i]);
		}
		break;
	default:
		printf("<%s> Invalid ID (%d)", __func__, _id);
		res = 1;
		break;
	}

	return res;	
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_update()
// Description    : 
//------------------------------------------------------------------------------
void parking_list_update(parking_info_t* _info)
{
	struct parking_info_list* plist = &g_parking_info_list;
	struct parking_node* pNew = NULL;

	printf("<%s>\r\n", __func__);

	pthread_mutex_lock(&g_parking_mutex);
	pNew = parking_list_find_node(_info->car_num);
	if (pNew != NULL) { 	// if node is exist then delete it
		printf("This Car is Aleady existed\r\n");
		parking_list_delete(pNew);
	}
	
	pNew = parking_list_make_node(_info);
	if (pNew == NULL) {
		printf("** ERROR OCCURED malloc failed **\r\n");
		return;
	}
	
	if (plist->cnt >= MAX_PARKING_LIST) {
		printf("discard tail !!\r\n");
		parking_list_delete(plist->tail);
	}

	// new node is must highest 
	parking_list_add_head(pNew);
	
	parking_list_dump();
	pthread_mutex_unlock(&g_parking_mutex);	
}

//------------------------------------------------------------------------------
// Function Name  : parking_list_get_item()
// Description    : 
//------------------------------------------------------------------------------
int parking_list_get_item(int _idx, parking_info_t* _dst)
{
	struct parking_info_list* plist = &g_parking_info_list;
	struct parking_node* pNew = NULL;
	int i, found = 0;

	memset(_dst, 0, sizeof(parking_info_t));
	
	if (plist->head == NULL) {
		printf("<%s> not founded !!(%d)\r\n", __func__, _idx);
		_dst->status = 0xff;
		return -1;
	}
	
	pthread_mutex_lock(&g_parking_mutex);	
	pNew = plist->head;
	for (i = 0; i < _idx; i++) {
		if ((pNew->next == NULL) || (pNew->next == pNew)) {
			break;
		}
		pNew = pNew->next;
	}
	if (i == _idx)
		memcpy(_dst, &pNew->curr, sizeof(parking_info_t));
	pthread_mutex_unlock(&g_parking_mutex);	

	if (i != _idx) {
		printf("<%s> not founded !!(%d)\r\n", __func__, _idx);
		_dst->status = 0xff;
		return -1;
	}

	printf("<%s> found !!(%d)\r\n", __func__, _idx);
	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : init_parking_info()
// Description    : 
//------------------------------------------------------------------------------
void init_parking_info(void)
{
	memset(&g_parking_info_list, 0, sizeof(struct parking_info_list));
	pthread_mutex_init(&g_parking_mutex, NULL);
}

