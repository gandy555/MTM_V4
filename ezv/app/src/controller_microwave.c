/******************************************************************************
 * Filename:
 *   controller_microwave.c
 *
 * Description:
 *   read from microwave sensor & report to host control manager
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
#include "controller_microwindow.h"
/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
#define SAMPLE_COUNT					14
#define MIN_FILTER						2		//3
#define MAX_FILTER						2		//3
#define MEASURE_PERIOD					1000	//1000[us]
#define MEASURE_REPORT_TERM			500		// 500[ms]

#define ADC_INPUT_PIN 					_IOW('S', 0x0c, unsigned long)

static int g_microwave_fd;
static pthread_t g_micro_wave_h;
static u32 g_adc_buffer[SAMPLE_COUNT] = {0,};
static u32 g_samp_cnt = 0;
static u32 g_adc_sum = 0;
static u32 g_adc_threshold = 0;
static workqueue_list_t *g_mw_wq;	
/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
int init_microwave_controller(u32 _thr);
void mw_set_threshold(u32 _thr);

//------------------------------------------------------------------------------
// Function Name  : fn_qsort_intcmp()
// Description    : 
//------------------------------------------------------------------------------
static int fn_qsort_intcmp(const void *a, const void *b)
{
	return(*(int *)a - *(int *)b);
}

//------------------------------------------------------------------------------
// Function Name  : is_sampling_completed()
// Description    : 
//------------------------------------------------------------------------------
static int is_sampling_completed(void)
{	
	if (g_samp_cnt >= SAMPLE_COUNT) 
		return 1;

	return 0;
}

//------------------------------------------------------------------------------
// Function Name  : save_sample_data()
// Description    : 
//------------------------------------------------------------------------------
static void save_sample_data(void)
{
	u32 adc_value = 0;
	
	if (read(g_microwave_fd, &adc_value, sizeof(u32)) == sizeof(u32)) {
		g_adc_buffer[g_samp_cnt++] = adc_value;
		g_adc_sum += adc_value;
		g_samp_cnt++;
	}
}

//------------------------------------------------------------------------------
// Function Name  : calc_sample_average()
// Description    : 
//------------------------------------------------------------------------------
static u32 calc_sample_average(void)
{
	u32 calc_val;
	int i;

	qsort(g_adc_buffer, SAMPLE_COUNT, sizeof(u32), fn_qsort_intcmp);

	for (i = 0; i < MIN_FILTER; i++) {
		g_adc_sum -= g_adc_buffer[i];					//¾Õ¿¡¼­ »­
	}
	
	for (i = 0; i < MAX_FILTER; i++) {
		g_adc_sum -= g_adc_buffer[SAMPLE_COUNT-1-i];	//µÚ¿¡¼­ »­
	}

	calc_val = (g_adc_sum / (SAMPLE_COUNT - MAX_FILTER - MIN_FILTER));

	g_samp_cnt = 0;
	g_adc_sum = 0;

	return calc_val;
}

//------------------------------------------------------------------------------
// Function Name  : init_microwave_controller()
// Description    : 
//------------------------------------------------------------------------------
static void *microwave_controller_thread(void *arg)
{
	u32 calc_val = 0;
	u32 p_time = 0;
	mtm_msg_t msg;
	
	DBG_MSG("<%s> Start\r\n", __func__);

	MSG_INIT(msg);
	msg.msg_id = MSG_EVENT_MICROWAVE;

	for (;;) {
		if (is_sampling_completed()) {
			calc_val = calc_sample_average();
			if ((get_elapsed_time(p_time) >= MEASURE_REPORT_TERM) 
				&& (calc_val > g_adc_threshold)) {
				msg_send(&msg);
				p_time = get_mono_time();
			}
		} else {
			save_sample_data();
		}
		
		usleep(MEASURE_PERIOD);
	}

	DBG_MSG("<%s> End\r\n", __func__);

	pthread_exit(NULL); 
}

//------------------------------------------------------------------------------
// Function Name  : mw_set_threshold()
// Description    : 
//------------------------------------------------------------------------------
void mw_set_threshold(u32 _thr)
{
	g_adc_threshold = _thr;
}

//------------------------------------------------------------------------------
// Function Name  : init_microwave_controller()
// Description    : 
//------------------------------------------------------------------------------
int init_microwave_controller(u32 _adc)
{
	int res = -1;
	
	g_microwave_fd = open(ADC_DEV, O_RDWR);
	if (g_microwave_fd < 0) {
		DBG_MSG("<%s> open failed(%s)\r\n", __func__, strerror(errno));
		return -1;
	}
	ioctl(g_microwave_fd, ADC_INPUT_PIN, 0);

	res = pthread_create(&g_micro_wave_h, NULL, microwave_controller_thread, NULL);
	if (res < 0) {
		DBG_MSG("<%s> thread create failed(%s)\r\n", __func__, strerror(errno));
		return -1;
	}

	mw_set_threshold(_adc);	
	
	return 0;
}

