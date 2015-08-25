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
int init_microwave_controller(void);
void microwave_set_threshold(u32 _thr);

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
		g_adc_sum -= g_adc_buffer[i];					//앞에서 뺌
	}
	
	for (i = 0; i < MAX_FILTER; i++) {
		g_adc_sum -= g_adc_buffer[SAMPLE_COUNT-1-i];	//뒤에서 뺌
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
	mtm_msg_t msg;
	
	DBG_MSG("<%s> Start\r\n", __func__);

	MSG_INIT(msg);
	msg.msg_id = MSG_EVENT_MICROWAVE;

	for (;;) {
		if (is_sampling_completed()) {
			calc_val = calc_sample_average();
			if (calc_val > g_adc_threshold) {
				if (GetElapsedTick(ulTick) >= MEASURE_REPORT_TERM)
				{
					msg_send(&msg);
					ulTick = GetTickCount();
				}
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
// Function Name  : microwave_set_threshold()
// Description    : 
//------------------------------------------------------------------------------
void microwave_set_threshold(u32 _thr)
{
	g_adc_threshold = _thr;
}

//------------------------------------------------------------------------------
// Function Name  : init_microwave_controller()
// Description    : 
//------------------------------------------------------------------------------
int init_microwave_controller(void)
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

	return 0;
}



BOOL CMicrowave::DoAdcSamplingStep()
{
	UINT uAdcValue = 0;
	BOOL fRet = FALSE;

	if(m_fdAdc < 0) return FALSE;
	if(g_samp_cnt >= SAMPLE_COUNT) return TRUE;

//	if(g_samp_cnt==0) g_adc_sum = 0;

	if(read(m_fdAdc, &uAdcValue, sizeof(UINT)) == sizeof(UINT))
	{
		g_adc_buffer[g_samp_cnt] = uAdcValue;
		g_adc_sum += uAdcValue;
		g_samp_cnt++;

		if(g_samp_cnt==SAMPLE_COUNT) fRet = TRUE;
	}

//	printf("%s: step=%d, value=%d, measure=%d\r\n", __func__, g_samp_cnt, uAdcValue, g_adc_sum);

	return fRet;
}

UINT CMicrowave::GetAdcSamplingResult()
{
	UINT uAdcSampleResult;
	int i;

	//샘플링된 ADC값을 정렬하고 
	qsort(g_adc_buffer, SAMPLE_COUNT, sizeof(UINT), fn_qsort_intcmp);

	//정렬된 배열에서 MAX_FILTER, MIN_FILTER 갯수만큼 합계에서 제외한뒤 평균냄
	for(i=0; i<MIN_FILTER; i++)
	{
		g_adc_sum -= g_adc_buffer[i];					//앞에서 뺌
	}
	for(i=0; i<MAX_FILTER; i++)
	{
		g_adc_sum -= g_adc_buffer[SAMPLE_COUNT-1-i];	//뒤에서 뺌
	}

	uAdcSampleResult = (g_adc_sum / (SAMPLE_COUNT - MAX_FILTER - MIN_FILTER));

	g_samp_cnt = 0;
	g_adc_sum = 0;

//	printf("%s: AdcSampleResult=%d\r\n", __func__, uAdcSampleResult);

	return uAdcSampleResult;
}

UINT CMicrowave::GetAdcSamplingValue()
{
	UINT i, adc_value[SAMPLE_COUNT], adc_measure=0, adc_error=0;

	if(m_fdAdc < 0) return 0;

	//ADC값을 SAMPLE_COUNT만큼 샘플링
	for(i=0; i<SAMPLE_COUNT; i++)
	{
		if(read(m_fdAdc, &(adc_value[i]), sizeof(UINT)) == sizeof(UINT))
		{
			adc_measure += adc_value[i];
		}
		else
		{
			adc_error++;	//ADC값 읽기 실패횟수
			if(i)
			{
				adc_value[i] = adc_value[i-1];	//못읽을 경우 이전값으로 보정
				adc_measure += adc_value[i];
				DBGMSG(DBG_MICROWAVE, "%s: ADC Read Error Adjusted\r\n", __func__);
			}
		}

		usleep(MEASURE_PERIOD);
	}

	//샘플링된 ADC값을 정렬하고 
	qsort(adc_value, SAMPLE_COUNT, sizeof(UINT), fn_qsort_intcmp);

	//정렬된 배열에서 MAX_FILTER, MIN_FILTER 갯수만큼 합계에서 제외한뒤 평균냄
	for(i=0; i<MIN_FILTER; i++)
	{
		adc_measure -= adc_value[i];					//앞에서 뺌
	}
	for(i=0; i<MAX_FILTER; i++)
	{
		adc_measure -= adc_value[SAMPLE_COUNT-1-i];		//뒤에서 뺌
	}

	adc_measure = (adc_measure / (SAMPLE_COUNT - MAX_FILTER - MIN_FILTER));

	return adc_measure;
}

void* CMicrowave::MicrowaveHandler(void *pParam)
{
	CMicrowave *pThis = (CMicrowave *)pParam;
	UINT uValue, ulTick;

	DBGMSG(DBG_MICROWAVE, "%s: Start\r\n", __func__);

	ulTick = GetTickCount();

	while(pThis->m_fRunning)
	{
		if(pThis->m_isMeasureEnable)
		{
		#if 0
			uValue = pThis->GetAdcSamplingValue();
			if(uValue > g_app_status.adc_value)
			{
			//	post_event(GR_EVENT_TYPE_USER, EVT_MICROWAVE_DETECT, uValue, 0);
			}
		#else
			if(pThis->DoAdcSamplingStep())
			{
				uValue = pThis->GetAdcSamplingResult();
				if(uValue > g_app_status.adc_value)
				{
					if(GetElapsedTick(ulTick) >= MEASURE_REPORT_TERM)
					{
						g_message.SendMessage(MSG_MICROWAVE_EVENT);
						ulTick = GetTickCount();
					}
				}
			}
		#endif
		}

		usleep(MEASURE_PERIOD);
	}

	DBGMSG(DBG_MICROWAVE, "%s: End\r\n", __func__);

	pthread_exit(NULL); 
}
