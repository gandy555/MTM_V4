#ifndef __WALLPAD_HDT_H__
#define __WALLPAD_HDT_H__

class CWallPadHdt : public CWallPad
{
public:
	CWallPadHdt();
	~CWallPadHdt();

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
	UCHAR CalcCheckSum(UCHAR* pBuffer, UINT size);

	//Thread Context
	static void* SerialListener_HDT(void *pParam);
	void RecvDataProc();

	void WeatherInfoProc(UCHAR *pBuffer);
	void ParkingInfoProc(UCHAR *pBuffer);

	void UssPollingProc(UCHAR *pBuffer);
//	void UssControlProc(UCHAR *pBuffer);

	void ElevatorPollingProc(UCHAR *pBuffer);
//	void ElevatorControlProc(UCHAR *pBuffer);

	//
	// Member Variable
	//
	CSerial		m_serial;

	BOOL		m_isRecv;					//������ ���� Flag

	UCHAR		m_GasControlReserve;		//�������� ������
	UCHAR		m_LightControlReserve;		//�ϰ��ҵ� ������
	UCHAR		m_SecurityControlReserve;	//������� ������
	UCHAR		m_ElevatorControlReserve;	//���������Ϳ��� ������

	BOOL		m_isElevatorCall;
	BOOL		m_isElevatorArrive;
};

#endif //__WALLPAD_HDT_H__
