#include "common.h"
#include "context_setup.h"
#include "main.h"

//
// Construction/Destruction
//
CWallPadKcm::CWallPadKcm()
{
	m_isRecv = FALSE;
	m_nRetry = 0;
}

CWallPadKcm::~CWallPadKcm()
{
}

//
// Interface Function
//
BOOL CWallPadKcm::Run()
{
	BOOL fRet = FALSE;
	struct termios tio;

	// 9600bps, 8bit, 1stopbit, non-parity
	if( (fRet = m_serial.Open(UART1_DEV, B9600)) == FALSE )
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Open Failure\r\n");
		return FALSE;
	}

	//for RS-485
	ioctl( m_serial.m_fdDev, 0x54F0, 0x0001 );
    tcflush  ( m_serial.m_fdDev, TCIFLUSH );

	//Listener Start
	if((fRet = m_serial.StartListener(SerialListener_KCM, this)) == FALSE)
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] StartListener Failure\r\n");
	}

	return fRet;
}

void CWallPadKcm::Stop()
{
	m_serial.StopListener();

	m_serial.Close();
}

//����������û
#define WEATHER_REQ_APPLY

void CWallPadKcm::RequestWeatherInfo()
{
	KCM_PACKET packet = {0,};

#ifndef WEATHER_REQ_APPLY	//�������, �ϰ���û���
	if( (m_packet.opcode != KCM_OP_BATCH_REQ) && !CHK_FLAG(m_packet.data[0], KCM_BATCH_REQ_ALL) )
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_WALLPAD;			//0x01
		packet.adl			= 0;
		packet.ash			= KCM_DEV_MTM;				//0x53
		packet.asl			= 0;
		packet.opcode		= KCM_OP_BATCH_REQ;			//0x00
		packet.data[0]		= KCM_BATCH_REQ_ALL;		//����,����,������ �ϰ���û
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
#else
	if( m_packet.opcode != KCM_OP_WEATHER_REQ )
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_WALLPAD;			//0x01
		packet.adl			= 0;
		packet.ash			= KCM_DEV_MTM;				//0x53
		packet.asl			= 0;
		packet.opcode		= KCM_OP_WEATHER_REQ;		//0x31
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
#endif
}

//�������¿�û
void CWallPadKcm::RequestGasStatus()
{
}

//�������ܿ�û
void CWallPadKcm::RequestGasCut()
{
	KCM_PACKET packet = {0,};

	if(m_packet.opcode != KCM_OP_GAS_CUT_REQ)
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_GAS_VALVE;		//0x2C
		packet.adl			= 0;
		packet.ash			= KCM_DEV_USS;				//0x54
		packet.asl			= 0;
		packet.opcode		= KCM_OP_GAS_CUT_REQ;		//0x02
		packet.data[0]		= 0;
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
}

//�ϰ��ҵ��û(isAllOff: TRUE=�ҵ�, FALSE=�ҵ�����)
void CWallPadKcm::RequestLightSet(BOOL isAllOff)
{
	KCM_PACKET packet = {0,};

	if(m_packet.opcode != KCM_OP_ALL_OFF_REQ)
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_LIGHT;			//0x0E
		packet.adl			= 0xff;						//���� ��ü
		packet.ash			= KCM_DEV_USS;				//0x54
		packet.asl			= 0;
		packet.opcode		= KCM_OP_ALL_OFF_REQ;		//0x00
		if(isAllOff)
			memset(packet.data, 0x00, KCM_DATA_SIZE);
		else
			memset(packet.data, 0xff, KCM_DATA_SIZE);
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
}

//���������û(isSecurityOn: TRUE=�������, FALSE=�������)
void CWallPadKcm::RequestSecuritySet(BOOL isSecurityOn)
{
	KCM_PACKET packet = {0,};

	if( (m_packet.opcode != KCM_OP_SECURITY_ON) && (m_packet.opcode != KCM_OP_SECURITY_OFF) )
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_WALLPAD;			//0x01
		packet.adl			= 0;
		packet.ash			= KCM_DEV_USS;				//0x54
		packet.asl			= 0;
		packet.opcode		= (isSecurityOn) ? KCM_OP_SECURITY_ON : KCM_OP_SECURITY_OFF;	//ON=0x11, OFF=0x12
		packet.data[0]		= (isSecurityOn) ? 2 : 0;	//D0: U����			0=������, 1=U���Ȼ����, 2=���
		packet.data[1]		= (isSecurityOn) ? 1 : 0;	//D1: �������ܿ���	0=���ܾ���, 1=����
		packet.data[2]		= (isSecurityOn) ? 1 : 0;	//D2: �������ܿ���	0=���ܾ���, 1=����
		packet.data[3]		= (isSecurityOn) ? 3 : 3;	//D3: �������ܿ���	0=���ܾ���, 1=����OFF, 2=����
		packet.data[4]		= (isSecurityOn) ? 3 : 3;	//D4: �����������	0=���ܾ���, 1=����
		packet.data[5]		= (isSecurityOn) ? 1 : 0;	//D5: ����������ȣ��	0=ȣ�����, 1=����
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
}

//���������� ȣ��
void CWallPadKcm::RequestElevatorCall()
{
	KCM_PACKET packet = {0,};

	if(m_packet.opcode != KCM_OP_ELEVATOR_CALL)
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_ELEVATOR;			//0x44
		packet.adl			= 0;
		packet.ash			= KCM_DEV_USS;				//0x54
		packet.asl			= 0;
		packet.opcode		= KCM_OP_ELEVATOR_CALL;		//0x01
		packet.data[0]		= 0;
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
}

//���������� ���¿�û
void CWallPadKcm::RequestElevatorStatus()
{
}

//������ġ������û
void CWallPadKcm::RequestParkingInfo()
{
	KCM_PACKET packet = {0,};

	if( (m_packet.opcode != KCM_OP_BATCH_REQ) && !CHK_FLAG(m_packet.data[0], KCM_BATCH_REQ_PARKING) )
	{
		m_nRetry = 0;
	}

	if(m_nRetry < 3)
	{
		packet.preamble		= KCM_PRE;
		packet.hd			= KCM_HD;
		packet.cc			= KCM_CC_ACK_IS | m_nRetry++;
		packet.pcnt			= 0;
		packet.adh			= KCM_DEV_WALLPAD;			//0x01
		packet.adl			= 0;
		packet.ash			= KCM_DEV_MTM;				//0x53
		packet.asl			= 0;
		packet.opcode		= KCM_OP_BATCH_REQ;			//0x00
		packet.data[0]		= KCM_BATCH_REQ_PARKING;	//0x02
		packet.fcc			= CalcCheckSum((UCHAR *)&packet.hd, 16);
		packet.eot			= KCM_EOT;

		if(Write((UCHAR*)&packet, sizeof(KCM_PACKET)) > 0)
		{
			g_timer.SetTimer(RESPONSE_TIMER, 1, NULL, "Response Timer");
		}
	}
	else
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
		g_message.SendMessage(MSG_TIMER_EVENT, RETRY_TIMEOUT);
	}
}


//
// Member Function
//
int CWallPadKcm::Write(UCHAR *pData, int size)
{
	CContextSetup* pContextSetup = NULL;
	int ret = ERROR;
	ULONG ulTick, delay;

	if(m_serial.IsOpen())
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Try To Write\r\n");
		DBGDMP(DBG_WALLPAD, pData, size, TRUE);

		//�������ϰ�� ���
		ulTick = get_mono_time();
		while(m_isRecv) 
		{ 
			usleep(1000); 
			if(get_elapsed_time(ulTick) >= 300)
			{
				DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Wait for read done: Timeout!!\r\n");
				return -1;
			}
		}

		// 485 Enable
		g_gpio_mtm.SetGpioOutPin(GPIO_RS485_CTRL, LOW);

		// TX Enable�� ���� ��Ŷ�� �����ϱ���� 1.5[ms] ����
		usleep(10);

		ret = m_serial.Write(pData, size);
		if(ret == size)
		{
			memcpy((UCHAR *)&m_packet, pData, sizeof(KCM_PACKET));
		}
		else
		{
			DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Write Bytes Error, write=%d, expect=%d\e\n", ret, size);
		}

		//������ ��Ŷ ������ TX Disable �ϱ���� ���� (��Ŷ������ ���۽ð� + 1.5[ms])
		delay = (size * 1042) + 10;	// 9600[bps]���� 1start+8bit+1stop ���ۼӵ��� 1.04167[ms] �̹Ƿ� 21 byte�� 21.875[ms]
		usleep(delay);

		// 485 Disable
		g_gpio_mtm.SetGpioOutPin(GPIO_RS485_CTRL, HIGH);

	#if 1
		if(g_isContextSetupWallPad)
		{
			pContextSetup = (CContextSetup*)g_state.GetCurrContext();
			if(pContextSetup)
			{
				pContextSetup->PrintPacket(pData, size, CYAN, FALSE);
			}
		}
	#endif
	}

	return ret;
}

/*
UCHAR CWallPadKcm::CalcCheckSum(UCHAR* pBuffer, UINT size)
{
	UCHAR cs = 0;

	if(pBuffer)
	{
		while(size--) cs += *pBuffer++;
	}

	return cs;
}
*/

//
// Thread(SerialListener) Context
//
void* CWallPadKcm::SerialListener_KCM(void *pParam)
{
	CWallPadKcm *pThis = (CWallPadKcm *)pParam;
	CSerial *pSerial = &pThis->m_serial;

	struct timeval timeout;
	fd_set fdRead;
//	ULONG ulTick;
	int ret;

	DBGMSG(DBG_WALLPAD, "%s: Start\r\n", __func__);

	while(pSerial->m_fListenerRunning)
	{
		//select���� ������ �ʱ�ȭ�ǹǷ� Read Set�� �ٽ� ����� ��
		FD_ZERO(&fdRead);
		FD_SET(pSerial->m_fdDev, &fdRead);

		timeout.tv_sec = SELECT_TIMEOUT_SEC;
		timeout.tv_usec = SELECT_TIMEOUT_USEC;

		ret = select(pSerial->m_fdDev+1, &fdRead, NULL, NULL, &timeout);

		if(FD_ISSET(pSerial->m_fdDev, &fdRead))
		{
			pThis->RecvDataProc();
		}
	}

	DBGMSG(DBG_WALLPAD, "%s: End\r\n", __func__);

	pthread_exit(NULL); 
}

void CWallPadKcm::RecvDataProc()
{
//	UCHAR buffer[KCM_PACKET_SIZE] = {0,};
	UCHAR buffer[MAX_RECV_BUFFER] = {0,};
	int recv_bytes = 0, byte_to_read, ret;
	UCHAR fcc_calc = 0;
	PKCM_PACKET pPacket = (PKCM_PACKET)&buffer[0];

	UCHAR buffer_mtm[MTM_PACKET_MAX] = {0,};
	PMTM_HEADER			pMtmHdr = (PMTM_HEADER)&buffer_mtm[0];
	PMTM_DATA_WEATHER	pMtmDataWeather  = (PMTM_DATA_WEATHER)&buffer_mtm[sizeof(MTM_HEADER)];
	parking_info_t *	pMtmDataParking  = (parking_info_t *)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_ELEVATOR	pMtmDataElevator = (PMTM_DATA_ELEVATOR)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_GAS		pMtmDataGas		 = (PMTM_DATA_GAS)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_LIGHT		pMtmDataLight	 = (PMTM_DATA_LIGHT)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_SECURITY	pMtmDataSecurity = (PMTM_DATA_SECURITY)&buffer_mtm[sizeof(MTM_HEADER)];

	CContextSetup* pContextSetup = NULL;

	time_t now;
	struct tm *t;
	struct timeval tvNew;
	ULONG ulTick, color;

	BOOL isMessage = FALSE;
	UCHAR adh, adl;
	int i, temperature, index;

	m_isRecv = TRUE;

#if 0
	ulTick = get_mono_time();
	while(recv_bytes < KCM_PACKET_SIZE)
	{
		recv_bytes += m_serial.Read(&buffer[recv_bytes], KCM_PACKET_SIZE-recv_bytes);

		if(get_elapsed_time(ulTick) >= 500)
		{
			DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Read Timeout\r\n");
			DBGDMP(DBG_WALLPAD, buffer, recv_bytes, TRUE);
			return;
		}
	}
#else
	//Preamble Search
	ret = m_serial.Read(&buffer[0], 2);
	while(pPacket->preamble != KCM_PRE)
	{
		if(ret <= 0)
		{
			m_isRecv = FALSE;
			return;
		}
		buffer[0] = buffer[1];
		ret = m_serial.Read(&buffer[1], 1);
	}
	recv_bytes = 2;
	//Preamble������ ����Ÿ ����
	ulTick = get_mono_time();
	while(recv_bytes < KCM_PACKET_SIZE)
	{
		recv_bytes += m_serial.Read(&buffer[recv_bytes], KCM_PACKET_SIZE-recv_bytes);

		if(get_elapsed_time(ulTick) >= 200)
		{
			DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Read Timeout: %d bytes\r\n", recv_bytes);
			DBGDMP(DBG_WALLPAD, buffer, recv_bytes, TRUE);
			m_isRecv = FALSE;
			return;
		}
	}
#endif

	m_isRecv = FALSE;

	DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Read %d bytes\r\n", recv_bytes);
	DBGDMP(DBG_WALLPAD, buffer, recv_bytes, TRUE);

	//��Ŷ���� �˻� (HD�ʵ�� üũ ����)
	if( (pPacket->preamble != KCM_PRE) || (pPacket->eot != KCM_EOT) )
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Invalid Format\r\n");
		return;
	}

	//üũ�� �˻�
	fcc_calc = CalcCheckSum(&buffer[2], 16);
	if(fcc_calc != pPacket->fcc)
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Invalid FCC, calc=0x%x, data=0x%x\r\n", fcc_calc, pPacket->fcc);
		return;
	}

	//������ �˻� : WallPad �� USS Device�鿡�� MTM���� ���۵� ��Ŷ�� ó��
	if( (pPacket->adh != KCM_DEV_MTM) && (pPacket->adh != KCM_DEV_USS) )
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Out of Destination\r\n");
		return;
	}

	//������Ŷüũ
	if( CHK_FLAG(pPacket->cc, KCM_CC_ACK) )
	{
		m_nRetry = 0;
		g_timer.KillTimer(RESPONSE_TIMER);
	}

	pMtmHdr->stx = MTM_PACKET_STX;

	//����ڵ� �˻�
	if( pPacket->adh == KCM_DEV_MTM )	//MTM ��������
	{
		switch(pPacket->opcode)
		{
		// MTM ��������
		case KCM_OP_BATCH_REQ:				//�ϰ���û(0x00)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_BATCH_REQ\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_ACK;
			pMtmHdr->len  = 0;
			buffer_mtm[sizeof(MTM_HEADER)] = MTM_PACKET_ETX;
			isMessage = TRUE;
			break;
		case KCM_OP_ALIVE_CHECK:			//Alive(0x3A)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_ALIVE_CHECK\r\n");
			//D0 �� : 0~23, 0xff=�ð����� �˼�����
			//D1 �� : 0~59, 0xff=�ð����� �˼�����
			//D2 �� : 0~59, 0xff=�ð����� �˼�����
			//D3 ����µ� : ����(0~100��), ����(-1~-100��), 0x80=�˼�����
			if( (pPacket->data[0] != 0xff) && (pPacket->data[1] != 0xff) && (pPacket->data[2] != 0xff) )
			{
				//�ð�����(?)
				if(g_isTimeSync==FALSE)
				{
					g_isTimeSync=TRUE;

					now = time(NULL);
					t = localtime(&now);

					t->tm_hour = pPacket->data[0];
					t->tm_min  = pPacket->data[1];
					t->tm_sec  = pPacket->data[2];

					tvNew.tv_sec  = mktime(t);
					tvNew.tv_usec = 0;

					if(settimeofday(&tvNew, NULL)==ERROR)
					{
						DBGMSG(DBG_WALLPAD, "[WallPad:KCM] settimeofday: error : %d %s\r\n", errno, strerror(errno));
					}
					else
					{
						system("hwclock -w");
						g_timer.Refresh();
						DBGMSG(DBG_WALLPAD, "[WallPad:KCM] settimeofday: done\r\n");
					}
				}
			}
			if( pPacket->data[3] != 0x80 )
			{
				//�µ�����
			//	g_app_status.current_temp = (int)(char)pPacket->data[3];
				if(pPacket->data[3] == 0x80)
					temperature = 0;
				else if(pPacket->data[3] > 0x80)
					temperature = (pPacket->data[3] - 256);
				else
					temperature = (char)pPacket->data[3];
					
				g_app_status.temp_low_right  = temperature;
				g_app_status.temp_high_right = temperature;
			}
			break;
		case KCM_OP_WEATHER_INFO:			//�ð��� ����(0x01)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_WEATHER_INFO\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_WEATHER;
			pMtmHdr->len  = sizeof(MTM_DATA_WEATHER);

			now = time(NULL);
			t = localtime(&now);

			//D0 : ����ð�
			pMtmDataWeather->year	= t->tm_year + 1900;
			pMtmDataWeather->month	= t->tm_mon + 1;
			pMtmDataWeather->day	= t->tm_mday;
			if(pPacket->data[0] < 24)
				pMtmDataWeather->hour = pPacket->data[0];	// 0~23, 0xff=�ð������˼�����
			else
				pMtmDataWeather->hour = t->tm_hour;
			pMtmDataWeather->minute	= t->tm_min;
			pMtmDataWeather->second	= t->tm_sec;

			//D1 : ��������
			switch(pPacket->data[1])
			{
			case 0x01:	//����
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_SERENITY;	//����
				break;
			case 0x02:	//��������
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_PARTLY_CLOUDY;	//��������
				break;
			case 0x03:	//��������
			case 0x04:	//�帲
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_CLOUDY;	//�帲
				break;
			case 0x05:	//��
			case 0x06:	//�� �Ǵ� ��
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_RAINNY;	//��
				break;
			case 0x07:	//��
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_SNOW;		//����
				break;
			}

			//D2 : �µ�		0x00~0x64	����(0~100��)
			//				0xFF~0x9C	����(-1~-100��)
			//				0x80		�˼�����

			//��ȣ ĳ������ �ȵ�!! Y!
			if(pPacket->data[2] == 0x80)
				temperature = 0;
			else if(pPacket->data[2] > 0x80)
				temperature = (pPacket->data[2] - 256);
			else
				temperature = (char)pPacket->data[2];
				
			pMtmDataWeather->temp2_low  = temperature;
			pMtmDataWeather->temp2_high = temperature;

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_WEATHER)] = MTM_PACKET_ETX;

			isMessage = TRUE;

		//	���е尡 �ð����� �ִ� ����Ÿ�� ����Ʈ ��Ŵ
			if(CHK_FLAG(pPacket->cc, KCM_CC_ACK_IS))
			{
				g_app_status.weather_left    = g_app_status.weather_right;
				g_app_status.temp_low_left   = g_app_status.temp_low_right;
				g_app_status.temp_high_left  = g_app_status.temp_high_right;
			}

			g_app_status.weather_right   = pMtmDataWeather->weather2;
			g_app_status.temp_low_right  = pMtmDataWeather->temp2_low  * 10;
			g_app_status.temp_high_right = pMtmDataWeather->temp2_high * 10;

		//	DBGMSG(1, "Temp: 0x%02x --> %d --> %d --> %d\r\n", pPacket->data[2], temperature, pMtmDataWeather->temp2_high, g_app_status.temp_high_right);

			break;
		case KCM_OP_WEATHER_REQ:			//�ð��� ������ȸ(0x31)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_WEATHER_REQ\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_ACK;
			pMtmHdr->len  = 0;
			buffer_mtm[sizeof(MTM_HEADER)] = MTM_PACKET_ETX;
			isMessage = TRUE;
			break;
		case KCM_OP_PARKING_INFO:			//��������(0x08)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_PARKING_INFO\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_PARKING;
			pMtmHdr->len  = sizeof(parking_info_t);

			//D0 : ������Ϲ�ȣ
			if(pPacket->data[0]==0)
				pMtmDataParking->status = PARKING_STATUS_INVALID;
			else
				pMtmDataParking->status = PARKING_STATUS_IN;

			sprintf(pMtmDataParking->car_num, "%d\0", pPacket->data[0]);	//������ȣ

			//D1~D7 : ������ġ����
			memcpy(pMtmDataParking->floor, &pPacket->data[1], 2);
			memcpy(pMtmDataParking->zone,  &pPacket->data[3], 5);

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(parking_info_t)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			if (pMtmDataParking->status == PARKING_STATUS_IN) {
				g_app_status.status = pMtmDataParking->status;
				memcpy(g_app_status.park_floor, pMtmDataParking->floor,   MAX_PARKING_FLOOR_NAME);
				memcpy(g_app_status.park_zone,  pMtmDataParking->zone,    MAX_PARKING_ZONE_NAME);
				memcpy(g_app_status.park_id,   pMtmDataParking->car_num, MAX_PARKING_CAR_NUM);
				parking_list_update(pMtmDataParking);	
			}

			break;
		case KCM_OP_DOOR_INFO:				//������ ��������(0x10)
			DBGMSG(DBG_WALLPAD, "--> KCM_OP_DOOR_INFO\r\n");
			break;
		}
	}
	else	// USS��������
	{
		switch(pPacket->ash)
		{
		case KCM_DEV_WALLPAD:				//0x01, ���е�(����)
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_WALLPAD\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_SECURITY;
			pMtmHdr->len  = sizeof(MTM_DATA_SECURITY);

			if(pPacket->opcode == 0x11)		//����
			{
				pMtmDataSecurity->status = MTM_DATA_SECURITY_ON;
				g_app_status.security_stat = 1;
			}
			else if(pPacket->opcode == 0x12)	//��������
			{
				pMtmDataSecurity->status = MTM_DATA_SECURITY_OFF;
				g_app_status.security_stat = 0;
			}
			else							//����
			{
				pMtmDataSecurity->status = MTM_DATA_SECURITY_ERROR;
			}

			//D0 : U����   0=������, 1=�����, 2=���
			// --> ���뿩�� Ȯ���ʿ�!

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_SECURITY)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_GAS_VALVE:				//0x2C, �������
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_WALLPAD\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_GAS;
			pMtmHdr->len  = sizeof(MTM_DATA_GAS);

			if(pPacket->opcode == 0x01)		//��꿭��
			{
				pMtmDataGas->status = MTM_DATA_GAS_OPEN;
				g_app_status.gas_stat = 0;
			}
			else if(pPacket->opcode == 0x02)	//������
			{
				pMtmDataGas->status = MTM_DATA_GAS_CUT;
				g_app_status.gas_stat = 1;
			}
			else							//����
				pMtmDataGas->status = MTM_DATA_GAS_ERROR;

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_GAS)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_LIGHT:					//0x0E, ����
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_LIGHT\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_LIGHT;
			pMtmHdr->len  = sizeof(MTM_DATA_LIGHT);

		//	DBGMSG(1, "LIGHT RES DATA: ");
			if(pPacket->opcode == 0x00)		//����
			{
				pMtmDataLight->status = MTM_DATA_LIGHT_OFF;			//�ҵ�
				for(i=0; i<7; i++)
				{
					if(pPacket->data[i])
					{
					//	DBGMSG(1, "[%d]%02x ", i, pPacket->data[i]);
						pMtmDataLight->status = MTM_DATA_LIGHT_ON;	//����
						break;
					}
				}

				g_app_status.light_stat = (pMtmDataLight->status==MTM_DATA_LIGHT_OFF) ? 1 : 0;	//����: 0=�ҵ�����, 1=�ҵ�

			//	DBGMSG(1, "\r\nStatus: 0x%x -> 0x%x\r\n", pMtmDataLight->status, g_app_status.light_stat);

			//	g_gpio_mtm.SetGpioOutPin(GPIO_RELAY, (g_app_status.light_stat) ? LOW : HIGH);	//LOW=�ҵ�, HIGH=�ҵ�����
			}
			else							//����
			{
				pMtmDataLight->status = MTM_DATA_LIGHT_ERROR;
			//	DBGMSG(1, "Status: Error\r\n");
			}

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_LIGHT)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_ELEVATOR:				//0x44, ����������
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_ELEVATOR\r\n");
			pMtmHdr->type = MTM_DATA_TYPE_ELEVATOR;
			pMtmHdr->len  = sizeof(MTM_DATA_ELEVATOR);

			//OP: 0x01=ȣ��, 0xff=ȣ����� --> ������

			//D0: ����
			switch(pPacket->data[0])
			{
			case 0x00:	//����
				pMtmDataElevator->status = MTM_DATA_EV_STATUS_STOP;
				break;
			case 0x01:	//����
				pMtmDataElevator->status = MTM_DATA_EV_STATUS_DOWN;
				break;
			case 0x02:	//����
				pMtmDataElevator->status = MTM_DATA_EV_STATUS_UP;
				break;
			case 0x03:	//����
				pMtmDataElevator->status = MTM_DATA_EV_STATUS_ARRIVE;
				break;
			default:
				pMtmDataElevator->status = MTM_DATA_EV_STATUS_ERROR;
				break;
			}

			//D1: ������
			pMtmDataElevator->floor = (char)(pPacket->data[1]);

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_ELEVATOR)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			g_app_status.elevator_status = pMtmDataElevator->status;
			g_app_status.elevator_floor  = pMtmDataElevator->floor;

			break;
		case KCM_DEV_BOILER:				//0x36, ���Ϸ�
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_BOILER\r\n");
			break;
		case KCM_DEV_OUTLET:				//0x3B, �ܼ�Ʈ
			DBGMSG(DBG_WALLPAD, "--> KCM_DEV_OUTLET\r\n");
			break;
		}
	}

	if(g_isContextSetupWallPad)
	{
		pContextSetup = (CContextSetup*)g_state.GetCurrContext();
		if(pContextSetup)
		{
			switch(pPacket->adh)
			{
			case KCM_DEV_MTM:
			case KCM_DEV_USS:
				color = LTCYAN;
				break;
			default:
				color = LTGREEN;
				break;
			}
			pContextSetup->PrintPacket(buffer, KCM_PACKET_SIZE, color);
		}
	}

	//ACKȮ�θ��
	if( CHK_FLAG(pPacket->cc, KCM_CC_ACK_IS) )
	{
		usleep(10000);	//10[ms] ���� ��������

		//������/����� �ٲ�� FCC �ٽð���ؼ� ����
		adh = pPacket->adh;
		adl = pPacket->adl;
		pPacket->cc  = KCM_CC_ACK;
		pPacket->adh = pPacket->ash;
		pPacket->adl = pPacket->asl;
		pPacket->ash = adh;
		pPacket->asl = adl;
		pPacket->fcc = CalcCheckSum(&buffer[2], 16);
		Write((UCHAR*)pPacket, sizeof(KCM_PACKET));
	}

	if(isMessage)
	{
		if(g_isContextSetupWallPad)
			g_message.SendMessageData(MSG_WALLPAD_DATA, buffer, KCM_PACKET_SIZE);
		else
			g_message.SendMessageData(MSG_WALLPAD_DATA, buffer_mtm, pMtmHdr->len+5);
	}
}

