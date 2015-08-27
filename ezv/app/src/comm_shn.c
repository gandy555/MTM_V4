#include "common.h"
#include "main.h"

//
// Construction/Destruction
//
CWallPadShn::CWallPadShn()
{
}

CWallPadShn::~CWallPadShn()
{
}

//
// Interface Function
//
BOOL CWallPadShn::Run()
{
	BOOL fRet = FALSE;
	struct termios tio;

	// 9600bps, 8bit, 1stopbit, non-parity
	if( (fRet = m_serial.Open(UART1_DEV, B9600)) == FALSE )
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:SHN] Open Failure\r\n");
		return FALSE;
	}

	//for SHN (even parity)
	if(tcgetattr(m_serial.m_fdDev, &tio)==ERROR)
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:SHN] tcgetattr failure\r\n");
		return FALSE;
	}

	tio.c_cflag |= PARENB;
	tio.c_iflag = 0;

//  tio.c_cc[VTIME] = 1;				// timeout 0.1�� ����
//  tio.c_cc[VMIN]  = 1;				// �ּ� n ���� ���� ������ ���

	//for RS-485
	ioctl( m_serial.m_fdDev, 0x54F0, 0x0001 );
    tcflush  ( m_serial.m_fdDev, TCIFLUSH );

	//for SHN (even parity)
    tcsetattr( m_serial.m_fdDev, TCSANOW, &tio );

	//Listener Start
	if((fRet = m_serial.StartListener(SerialListener_SHN, this)) == FALSE)
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:SHN] StartListener Failure\r\n");
	}

	return fRet;
}

void CWallPadShn::Stop()
{
	m_serial.StopListener();

	m_serial.Close();
}

//����������û
void CWallPadShn::RequestWeatherInfo()
{
}

//�������ܿ�û
void CWallPadShn::RequestGasCut()
{
}

//�������¿�û
void CWallPadShn::RequestGasStatus()
{
}

//�ϰ��ҵ��û(isAllOff: TRUE=�ҵ�, FALSE=�ҵ�����)
void CWallPadShn::RequestLightSet(BOOL isAllOff)
{
}

//���������û(isSecurityOn: TRUE=�������, FALSE=�������)
void CWallPadShn::RequestSecuritySet(BOOL isSecurityOn)
{
}

//���������� ȣ��
void CWallPadShn::RequestElevatorCall()
{
}

//���������� ���¿�û
void CWallPadShn::RequestElevatorStatus()
{
}

//������ġ������û
void CWallPadShn::RequestParkingInfo()
{
}



//
// Member Function
//
int CWallPadShn::Write(UCHAR *pData, int size)
{
	int ret = ERROR;

	if(m_serial.IsOpen())
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:SHN] Write\r\n");
		DBGDMP(DBG_WALLPAD, pData, size, TRUE);

		//�������ϰ�� ���
		while(m_isRecv) { usleep(1000); }

		// 485 Enable
		g_gpio_mtm.SetGpioOutPin(GPIO_RS485_CTRL, LOW);

		// TX Enable�� ���� ��Ŷ�� �����ϱ���� 1.5ms ����
		usleep(1500);

		ret = m_serial.Write(pData, size);

		//������ ��Ŷ ������ TX Disable �ϱ���� 500us ����
		usleep(500);

		// 485 Disable
		g_gpio_mtm.SetGpioOutPin(GPIO_RS485_CTRL, HIGH);
	}

	return ret;
}

UCHAR CWallPadShn::CalcCheckSum(UCHAR* pBuffer, UINT size)
{
	UCHAR cs = 0;

	if(pBuffer)
	{
		while(size--) cs += *pBuffer++;
	}

	return cs;
}

//
// Thread(SerialListener) Context
//
void* CWallPadShn::SerialListener_SHN(void *pParam)
{
	CWallPadShn *pThis = (CWallPadShn *)pParam;
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

void CWallPadShn::RecvDataProc()
{
	UCHAR buffer[KCM_PACKET_SIZE] = {0,};
	UCHAR recv_bytes = 0, fcc_calc = 0;
	PKCM_PACKET pPacket = (PKCM_PACKET)&buffer[0];

	UCHAR buffer_mtm[MTM_PACKET_MAX] = {0,};
	PMTM_HEADER			pMtmHdr = (PMTM_HEADER)&buffer_mtm[0];
	PMTM_DATA_WEATHER	pMtmDataWeather  = (PMTM_DATA_WEATHER)&buffer_mtm[sizeof(MTM_HEADER)];
	parking_info_t *	pMtmDataParking  = (parking_info_t *)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_ELEVATOR	pMtmDataElevator = (PMTM_DATA_ELEVATOR)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_GAS		pMtmDataGas		 = (PMTM_DATA_GAS)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_LIGHT		pMtmDataLight	 = (PMTM_DATA_LIGHT)&buffer_mtm[sizeof(MTM_HEADER)];
	PMTM_DATA_SECURITY	pMtmDataSecurity = (PMTM_DATA_SECURITY)&buffer_mtm[sizeof(MTM_HEADER)];

	time_t now;
	struct tm *t;
	BOOL isMessage = FALSE;

	m_isRecv = TRUE;

	while(recv_bytes < KCM_PACKET_SIZE)
	{
		recv_bytes += m_serial.Read(&buffer[recv_bytes], KCM_PACKET_SIZE-recv_bytes);
	}

	m_isRecv = FALSE;

	DBGMSG(DBG_WALLPAD, "[WallPad:SHN] Read\r\n");
	DBGDMP(DBG_WALLPAD, buffer, recv_bytes, TRUE);

#if 0
	//��Ŷ���� �˻�
	if( (pPacket->preamble != KCM_PRE) || (pPacket->eot != KCM_EOT) )
	{
		DBGMSG(DBG_WALLPAD, "[WallPad:KCM] Invalid Format\r\n");
		return;
	}

	//üũ�� �˻�
	fcc_calc = CalcByteCheckSum(&buffer[2], 16);
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

	pMtmHdr->stx = MTM_PACKET_STX;

	//����ڵ� �˻�
	if( pPacket->adh == KCM_DEV_MTM )	//MTM ��������
	{
		switch(pPacket->opcode)
		{
		// MTM ��������
		case KCM_OP_BATCH_REQ:				//�ϰ���û(0x00)
			break;
		case KCM_OP_ALIVE_CHECK:			//Alive(0x3A)
			break;
		case KCM_OP_WEATHER_INFO:			//�ð��� ����(0x01)
			pMtmHdr->type = MTM_DATA_TYPE_WEATHER;
			pMtmHdr->len  = sizeof(MTM_DATA_WEATHER);

			now = time(NULL);
			t = localtime(&now);

			//D0 : ����ð�
			pMtmDataWeather->year	= t->tm_year + 1900;
			pMtmDataWeather->month	= t->tm_mon + 1;
			pMtmDataWeather->day	= t->tm_mday
			if(pPacket->data[0] < 24)
				pMtmDataWeather->hour = pPacket->data[0];	// 0~23, 0xff=�ð������˼�����
			else
				pMtmDataWeather->hour = t->tm_hour;
			pMtmDataWeather->minute	= t->tm_min;
			pMtmDataWeather->second	= t->tm_sec

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
				pMtmDataWeather->weather2 = IMG_ENUM_WEATHER_SERENITY;	//����
				break;
			}

			//D2 : �µ�		0x00~0x64	����(0~100��)
			//				0xFF~0x9C	����(-1~-100��)
			//				0x80		�˼�����
			if(pPacket->data[2] == 0x80)
			{
				pMtmDataWeather->temp2_low  = 0;
				pMtmDataWeather->temp2_high = 0;
			}
			else 
			{
				pMtmDataWeather->temp2_low  = (short)(char)(pPacket->data[2]);
				pMtmDataWeather->temp2_high = (short)(char)(pPacket->data[2]);
			}

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_WEATHER)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_OP_PARKING_INFO:			//��������(0x08)
			pMtmHdr->type = MTM_DATA_TYPE_PARKING;
			pMtmHdr->len  = sizeof(parking_info_t);

			//D0 : ������Ϲ�ȣ
			if(pPacket->data[0]==0)
				pMtmDataParking->status = parking_info_t_STATUS_NOINFO;
			else
				pMtmDataParking->status = PARKING_STATUS_IN;

			//D1~D7 : ������ġ����
			memcpy(pMtmDataParking->zone, &pPacket->data[1], 7);

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(parking_info_t)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_OP_DOOR_INFO:				//������ ��������(0x10)
			break;
		}
	}
	else	// USS��������
	{
		switch(pPacket->ash)
		{
		case KCM_DEV_WALLPAD:				//���е�(����)
			pMtmHdr->type = MTM_DATA_TYPE_SECURITY;
			pMtmHdr->len  = sizeof(MTM_DATA_SECURITY);

			if(pPacket->opcode = 0x11)		//����
				pMtmDataSecurity->status = MTM_DATA_SECURITY_ON;
			else if(pPacket->opcode = 0x12)	//��������
				pMtmDataSecurity->status = MTM_DATA_SECURITY_OFF;
			else							//����
				pMtmDataSecurity->status = MTM_DATA_SECURITY_ERROR;

			//D0 : U����   0=������, 1=�����, 2=���
			// --> ���뿩�� Ȯ���ʿ�!

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_SECURITY)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_GAS_VALVE:				//�������
			pMtmHdr->type = MTM_DATA_TYPE_GAS;
			pMtmHdr->len  = sizeof(MTM_DATA_GAS);

			if(pPacket->opcode = 0x01)		//��꿭��
				pMtmDataGas->status = MTM_DATA_GAS_OPEN;
			else if(pPacket->opcode = 0x02)	//������
				pMtmDataGas->status = MTM_DATA_GAS_CUT;
			else							//����
				pMtmDataGas->status = MTM_DATA_GAS_ERROR;

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_GAS)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_LIGHT:					//����
			pMtmHdr->type = MTM_DATA_TYPE_LIGHT;
			pMtmHdr->len  = sizeof(MTM_DATA_LIGHT);

			if(pPacket->opcode = 0x00)		//����
			{
				pMtmDataLight->status = MTM_DATA_LIGHT_OFF;
				for(i=0; i<7; i++)
				{
					if(pPacket->data[i])
					{
						pMtmDataLight->status = MTM_DATA_LIGHT_ON;
						break;
					}
				}
			}
			else							//����
			{
				pMtmDataLight->status = MTM_DATA_LIGHT_ERROR;
			}

			buffer_mtm[sizeof(MTM_HEADER)+sizeof(MTM_DATA_LIGHT)] = MTM_PACKET_ETX;

			isMessage = TRUE;

			break;
		case KCM_DEV_ELEVATOR:				//����������
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

			break;
		case KCM_DEV_BOILER:				//���Ϸ�
			break;
		case KCM_DEV_OUTLET:				//�ܼ�Ʈ
			break;
		}
	}

	if(isMessage)
	{
		g_message.SendMessageData(MSG_WALLPAD_DATA, buffer_mtm, pMtmHdr->len+5);
	}
#endif
}

