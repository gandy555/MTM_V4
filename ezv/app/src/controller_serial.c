/******************************************************************************
 * Filename:
 *   controller_serial.c
 *
 * Description:
 *   controller of uart driver 
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-10-02
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"
#include "controller_serial.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
static int g_serial_fd;
static pthread_t g_serial_thread;

/******************************************************************************
 *
 * Functions Declaration
 *
 ******************************************************************************/
void controller_serial_init(void);
u8 serial_open(const char *_dev, u32 _baud);

//------------------------------------------------------------------------------
// Function Name  : controller_serial_init()
// Description    :
//------------------------------------------------------------------------------
void controller_serial_init(void)
{
	g_serial_fd = -1;
	g_serial_thread	= 0;
}

//------------------------------------------------------------------------------
// Function Name  : serial_open()
// Description    :
//------------------------------------------------------------------------------
u8 serial_open(const char *_dev, u32 _baud)
{
	struct termios tio;

	if (_dev == NULL) {
		DBG_MSG("<%s> invalid dev name\r\n", __func__);
		return 0;
	}

	if ((strcmp(_dev, UART0_DEV) != 0) &&
		(strcmp(_dev, UART1_DEV) != 0) && (strcmp(_dev, UART2_DEV) != 0)) {
		DBG_MSG("<%s> unsupported dev name '%s'\r\n", __func__, _dev);
		return FALSE;
	}

	g_serial_fd = open(_dev, O_RDWR);
	if (g_serial_fd < 0) {
		DBG_MSG("<%s> '%s' device open failure\r\n", __func__, _dev);
		return FALSE;
	}

	memset(&tio, 0, sizeof(termios));
	tio.c_iflag = IGNPAR;				// non-parity
	tio.c_oflag = 0;					//
	tio.c_cflag = CS8 | CLOCAL | CREAD;	// NO-rts/cts	
	tio.c_cflag |= _baud;
	tio.c_lflag = 0;

	tio.c_cc[VTIME] = 1;				// timeout 0.1초 단위
	tio.c_cc[VMIN]  = 1;				// 최소 n 문자 받을 때까진 대기

	tcflush  ( g_serial_fd, TCIFLUSH );
	tcsetattr( g_serial_fd, TCSANOW, &tio );

	DBG_MSG("<%s> '%s' device open success\r\n", __func__, _dev);

	return TRUE;
}

//------------------------------------------------------------------------------
// Function Name  : serial_close()
// Description    :
//------------------------------------------------------------------------------
void serial_close(void)
{
	if (g_serial_fd >= 0) {
		close(g_serial_fd);
		g_serial_fd = -1;
	}
}

//------------------------------------------------------------------------------
// Function Name  : serial_is_opened()
// Description    :
//------------------------------------------------------------------------------
u8 serial_is_opened(void)
{
	return (g_serial_fd != ERROR) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
// Function Name  : serial_write()
// Description    :
//------------------------------------------------------------------------------
int serial_write(const u8 *_data, u32 _size)
{
	int ret = ERROR;

	if (g_serial_fd >= 0) {
		ret = write(g_serial_fd, _data, _size);
		if (ret == ERROR) {
			DBG_MSG("<%s> write failure : errno=%d %s\r\n",
				__func__, errno, strerror(errno));
			return ERROR;
		}
	}

	return ret;
}

//------------------------------------------------------------------------------
// Function Name  : seria_read()
// Description    :
//------------------------------------------------------------------------------
int seria_read(u8 *_data, u32 _size)
{
	int ret = ERROR;

	if (g_serial_fd >= 0) {
		ret = read(g_serial_fd, _data, _size);
		if (ret == ERROR) {
			DBG_MSG("<%s> read failure : errno=%d %s\r\n", __func__, errno, strerror(errno));
			return ERROR;
		}
	}

	return ret;
}

BOOL CSerial::StartListener(serial_listener pfnHandler, void* pParam)
{
	int create_error;

	m_fListenerRunning = TRUE;

	if(pfnHandler)
		create_error = pthread_create(&g_serial_thread, NULL, pfnHandler, (pParam) ? pParam : this);
	else
		create_error = pthread_create(&g_serial_thread, NULL, SerialListener, this);

	if(create_error)
	{
		DBGMSG(DBG_ERROR, "%s: pthread_create failure: error=%d %s\r\n", __func__, errno, strerror(errno));
		m_fListenerRunning = FALSE;
		Close();
		return FALSE;
	}

	return TRUE;
}

void CSerial::StopListener()
{
	void* thread_result;
	int ret;

	if(m_fListenerRunning)
	{
		m_fListenerRunning = FALSE;

		ret = pthread_join(g_serial_thread, &thread_result);
		if(ret)
		{
			DBGMSG(DBG_ERROR, "%s: pthread_join failure: error=%d %s\r\n", __func__, errno, strerror(errno));
		}
	}
}

void* CSerial::SerialListener(void *pParam)
{
	CSerial *pThis = (CSerial *)pParam;
	unsigned char buffer[1024];
	struct timeval timeout = { 10, 0 };
	fd_set	fdRead;
	int ret;
	

	while(pThis->m_fListenerRunning)
	{
		FD_SET((unsigned int)pThis->g_serial_fd, &fdRead);
		ret = select(pThis->g_serial_fd+1, &fdRead, NULL, NULL, &timeout);
		if((ret != ERROR)&&(FD_ISSET(pThis->g_serial_fd, &fdRead))) 
		{
			memset(buffer, 0, 1024);
			ret = pThis->Read(buffer, 1024);
			if((ret != ERROR)&&(ret != 0))
			{
				buffer[ret] = 0;
				DBGMSG(DBG_SERIAL, "%s\r\n", (char *)buffer);
			}
		}
	}

	pthread_exit(NULL); 
}


