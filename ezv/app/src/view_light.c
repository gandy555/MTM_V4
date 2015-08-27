/*
*/
#include "common.h"
#include "context_light.h"
#include "main.h"

//
// Construction/Destruction
//
CContextLight::CContextLight(GR_WINDOW_ID wid, GR_GC_ID gc)
	: CContext(wid, gc)
{
	m_isWallPadStatus = FALSE;
}

CContextLight::~CContextLight()
{
}

//
// Member Function
//
void CContextLight::Init()
{
	CObject* pObject;
	CObjectIcon* pObjectIcon;
	UINT id;

	CContext::Init();

	// Blank ��� �̹���
	pObject = new CObjectImage(m_wid_parent, m_gc, 0, 0, g_scr_info.cols, g_scr_info.rows);
	if(pObject)
	{
		pObject->LoadImage(IMG_BACKGROUND, "/app/img/blank_bg.png");

		id = m_ObjectList.AddObject(pObject);
	}

	// ������ �̹���
	pObject = new CObjectIcon(m_wid_parent, m_gc, 58, 50, 158, 158);
	if(pObject)
	{
		pObjectIcon = (CObjectIcon*)pObject;
		pObjectIcon->AllocImageCount(IMG_ENUM_ICON_COUNT);
		pObject->LoadImage(IMG_ENUM_ICON_ON,	"/app/img/icon_light_on.png");
		pObject->LoadImage(IMG_ENUM_ICON_OFF,	"/app/img/icon_light_off.png");

		id = m_ObjectList.AddObject(pObject);
	}

	// ����ġ �̹���
	pObject = new CObjectIcon(m_wid_parent, m_gc, 312, 287, 299, 116);
	if(pObject)
	{
		pObjectIcon = (CObjectIcon*)pObject;
		pObjectIcon->AllocImageCount(IMG_ENUM_SWITCH_COUNT);
		pObject->LoadImage(IMG_ENUM_SWITCH_ON,	"/app/img/icon_switch_on.png");
		pObject->LoadImage(IMG_ENUM_SWITCH_OFF,	"/app/img/icon_switch_off.png");

		id = m_ObjectList.AddObject(pObject);
	}
}

void CContextLight::DeInit()
{
	CContext::DeInit();
	
	m_ObjectList.RemoveAll();
}

//������ �����ִٴ°��� �ϰ��ҵ� SW�� OFF��ٴ� ��
void CContextLight::Draw(UINT nContextNum)
{
	CObjectIcon* pObjectIcon = NULL;

	if(m_gc==0) return;

	DBGMSGC(DBG_LIGHT, "++ [%d]\r\n", nContextNum);

	//���
	m_ObjectList.Draw(LIGHT_OBJ_BG);

	//������
	pObjectIcon = (CObjectIcon*)m_ObjectList.FindObjectByID(LIGHT_OBJ_ICON);
	if(pObjectIcon)
	{
		pObjectIcon->Draw(g_app_status.light_stat ? IMG_ENUM_ICON_ON : IMG_ENUM_ICON_OFF);
	}

	//�ؽ�Ʈ
	vm_draw_text(144, 329, 150, 32, 24, WHITE,
		TXT_HALIGN_RIGHT|TXT_VALIGN_MIDDLE, "�ϰ��ҵ�����");
	vm_draw_text(625, 329, 150, 32, 24, WHITE,
		TXT_HALIGN_LEFT|TXT_VALIGN_MIDDLE, "�ϰ��ҵ�");

	switch(nContextNum)
	{
	case 0:
		//�ؽ�Ʈ
		vm_draw_text(240, 108, 500, 40, 32, WHITE,
			TXT_HALIGN_LEFT|TXT_VALIGN_MIDDLE, "���е� ���¿�û��..");
		break;
	case 1:
	case 2:
		//�ؽ�Ʈ
		if (g_app_status.light_stat) {
			vm_draw_text(240, 108, 500, 40, 32, WHITE,
				TXT_HALIGN_LEFT|TXT_VALIGN_MIDDLE, "�ϰ��ҵ�Ǿ����ϴ�");
		} else {
			vm_draw_text(240, 108, 500, 40, 32, WHITE,
				TXT_HALIGN_LEFT|TXT_VALIGN_MIDDLE, "�ϰ��ҵ� �����Ǿ����ϴ�");
		}
		break;
	}

	//����ġ
	pObjectIcon = (CObjectIcon*)m_ObjectList.FindObjectByID(LIGHT_OBJ_SWITCH);
	if(pObjectIcon)
	{
		pObjectIcon->Draw(g_app_status.light_stat ? IMG_ENUM_SWITCH_ON : IMG_ENUM_SWITCH_OFF);
	}

	DBGMSGC(DBG_LIGHT, "--\r\n");
}

void CContextLight::Proc(UINT nContextNum)
{
	char szWaveFile[2][14] = { "light_on\0", "light_off\0" };	//ON=�ҵ�����, OFF=�ҵ�
	char szWaveFilePath[128] = {0,};

	DBGMSGC(DBG_LIGHT, "++ [%d]\r\n", nContextNum);

	switch(nContextNum)
	{
	case 0:
		if (g_pWallPad) {
			m_isWallPadStatus = 1;
			g_pWallPad->RequestLightStatus();
		}
		break;
	case 1:
	case 2:
		sprintf(szWaveFilePath, "/app/sound/%s.wav\0", szWaveFile[g_app_status.light_stat]);
		play_wav_file(szWaveFilePath);
	//	g_gpio_mtm.SetGpioOutPin(GPIO_RELAY, (g_app_status.light_stat) ? LOW : HIGH);	//LOW=�ҵ�, HIGH=�ҵ�����
		break;
	}

	DBGMSGC(DBG_LIGHT, "--\r\n");
}

void CContextLight::TimerProc(UINT idTimer)
{
	switch(idTimer)
	{
	case RESPONSE_TIMER:
		if(g_pWallPad)
		{
			// light_stat: 0=����(�ҵ�����) --> TRUE=�ҵ� 
			//			   1=�ҵ�           --> FALSE=����(�ҵ�����)
			g_pWallPad->RequestLightSet( (g_app_status.light_stat) ? FALSE : TRUE );
		}
		break;
	}
}

void CContextLight::RecvProc(UCHAR *pPacket)
{
	PMTM_HEADER pHdr = (PMTM_HEADER)pPacket;

	if(pPacket==NULL) return;

	DBGMSGC(DBG_LIGHT, "++\r\n");

	//DBGDMP(DBG_LIGHT, pPacket, 21, TRUE);

	switch(pHdr->type)
	{
	case MTM_DATA_TYPE_WEATHER:
		break;
	case MTM_DATA_TYPE_PARKING:
		break;
	case MTM_DATA_TYPE_ELEVATOR:
		break;
	case MTM_DATA_TYPE_GAS:
		break;
	case MTM_DATA_TYPE_LIGHT:
		if (g_isBackLightOn) {
			if (m_isWallPadStatus)
				ChangeContext(1);
			else
				ChangeContext(2);
		}
		m_isWallPadStatus = FALSE;
		break;
	case MTM_DATA_TYPE_SECURITY:
		break;
	}

	DBGMSGC(DBG_LIGHT, "--\r\n");
}

void CContextLight::ButtonDown(UINT usGpioFlag, UINT usEventEnum)
{
	DBGMSGC(DBG_LIGHT, "++\r\n");

	if(usEventEnum == MTM_GPIO_BUTTON_DOWN)
	{
	}
	else if(usEventEnum == MTM_GPIO_BUTTON_LONG)
	{
#if 0	
		if( CHK_FLAG(usGpioFlag, BIT_FLAG(GPIO_FRONT_RIGHT_TOP)|BIT_FLAG(GPIO_FRONT_RIGHT_BOTTOM)) ||
			CHK_FLAG(usGpioFlag, BIT_FLAG(GPIO_REAR_VOL_UP)|BIT_FLAG(GPIO_REAR_VOL_DOWN)) )
		{
			g_state.ChangeState(STATE_SETUP);
		}
#endif		
	}

	DBGMSGC(DBG_LIGHT, "--\r\n");
}

void CContextLight::ButtonUp(UINT usGpioFlag, UINT usEventEnum)
{
	DBGMSGC(DBG_LIGHT, "++\r\n");

	if(usEventEnum == MTM_GPIO_BUTTON_UP)
	{
		switch(usGpioFlag)
		{
		case GPIO_FLAG_FRONT_LEFT_TOP:		//Weather
			g_state.ChangeState(STATE_WEATHER);
			break;
		case GPIO_FLAG_FRONT_LEFT_MIDDLE:	//Elevator
			g_state.ChangeState(STATE_ELEVATOR);
			break;
		case GPIO_FLAG_FRONT_LEFT_BOTTOM:	//Parking
			g_state.ChangeState(STATE_PARKING);
			break;
		case GPIO_FLAG_FRONT_RIGHT_TOP:		//Gas
			g_state.ChangeState(STATE_GAS);
			break;
		case GPIO_FLAG_FRONT_RIGHT_MIDDLE:	//Light
			if(g_pWallPad)
			{
				// light_stat: 0=����(�ҵ�����) --> TRUE=�ҵ� 
				//			   1=�ҵ�           --> FALSE=����(�ҵ�����)
				g_pWallPad->RequestLightSet( (g_app_status.light_stat) ? FALSE : TRUE );

				// ���� Ű�� ���°��� MTM�� ��ü��! - ������
				// ������¿� ��������
				g_gpio_mtm.SetGpioOutPin(GPIO_RELAY, (g_app_status.light_stat) ? HIGH : LOW);	//LOW=�ҵ�, HIGH=�ҵ�����

				// ��� �̷�� ���ٰ� �����ϰ� ���¸� ������Ŵ (���е��� ������ ������ ���������� �̷�� ���� �ʴ� ��������)
				// ���������� ���е尡 �����Ѵٸ� ������� ����� �ݿ���
				g_app_status.light_stat = (g_app_status.light_stat==1) ? 0 : 1;
				
				// ȭ�鰻��
				ChangeContext(2);
			}
			break;
		case GPIO_FLAG_FRONT_RIGHT_BOTTOM:	//Security
			g_state.ChangeState(STATE_SECURITY);
			break;
		}
	}

	DBGMSGC(DBG_LIGHT, "--\r\n");
}

