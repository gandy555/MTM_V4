#ifndef __WALLPAD_KCM_H__
#define __WALLPAD_KCM_H__

class CWallPadKcm : public CWallPad
{
public:
	CWallPadKcm();
	~CWallPadKcm();

	//
	// Interface Function
	//
	virtual BOOL Run();
	virtual void Stop();

	virtual void RequestWeatherInfo();		//����������û

	virtual void RequestGasStatus();		//�������¿�û
	virtual void RequestGasCut();			//�������ܿ�û

	virtual void RequestLightSet(BOOL isAllOff);		//�ϰ��ҵ��û(isAllOff: TRUE=�ҵ�, FALSE=�ҵ�����)

	virtual void RequestSecuritySet(BOOL isSecurityOn);	//���������û(isSecurityOn: TRUE=�������, FALSE=�������)

	virtual void RequestElevatorCall();		//���������� ȣ��
	virtual void RequestElevatorStatus();	//���������� ���¿�û

	virtual void RequestParkingInfo();		//������ġ������û

	//
	// Member Function
	//
	int Write(UCHAR *pData, int size);
//	UCHAR CalcCheckSum(UCHAR* pBuffer, UINT size);

	//Thread Context
	static void* SerialListener_KCM(void *pParam);
	void RecvDataProc();



	//
	// Member Variable
	//
	CSerial		m_serial;

	BOOL		m_isRecv;					//������ ���� Flag

	KCM_PACKET	m_packet;
	UCHAR		m_nRetry;
};

#endif //__WALLPAD_KCM_H__
