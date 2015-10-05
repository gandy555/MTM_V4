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
#include <termios.h>
#include "main.h"
#include "controller_serial.h"

/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
static pthread_t g_serial_thread;

/******************************************************************************
 *
 * Functions Declaration
 *
 ******************************************************************************/
u8 serial_open(const char *_dev, u32 _baud);
void serial_close(int _fd);
int serial_write(int _fd, const u8 *_data, u32 _size);
int serial_read(int _fd, u8 *_data, u32 _size);
int serial_register_listener(serial_listener _handler);

//------------------------------------------------------------------------------
// Function Name  : serial_open()
// Description    :
//------------------------------------------------------------------------------
int serial_open(const char *_dev, u32 _baud)
{
	struct termios tio;
	int fd;
	
	if (_dev == NULL) {
		DBG_MSG("<%s> invalid dev name\r\n", __func__);
		return -1;
	}

	if ((strcmp(_dev, UART0_DEV) != 0) &&
		(strcmp(_dev, UART1_DEV) != 0) && (strcmp(_dev, UART2_DEV) != 0)) {
		DBG_MSG("<%s> unsupported dev name '%s'\r\n", __func__, _dev);
		return -1;
	}

	fd = open(_dev, O_RDWR);
	if (fd < 0) {
		DBG_MSG("<%s> '%s' device open failure\r\n", __func__, _dev);
		return fd;
	}

	memset(&tio, 0, sizeof(termios));
	tio.c_iflag = IGNPAR;				// non-parity
	tio.c_oflag = 0;					//
	tio.c_cflag = CS8 | CLOCAL | CREAD;	// NO-rts/cts	
	tio.c_cflag |= _baud;
	tio.c_lflag = 0;

	tio.c_cc[VTIME] = 1;				// timeout 0.1초 단위
	tio.c_cc[VMIN]  = 1;				// 최소 n 문자 받을 때까진 대기

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &tio );

	DBG_MSG("<%s> '%s' device open success\r\n", __func__, _dev);

	return fd;
}

//------------------------------------------------------------------------------
// Function Name  : serial_close()
// Description    :
//------------------------------------------------------------------------------
void serial_close(int _fd)
{
	if (_fd >= 0) 
		close(_fd);
}

//------------------------------------------------------------------------------
// Function Name  : serial_write()
// Description    :
//------------------------------------------------------------------------------
int serial_write(int _fd, const u8 *_data, u32 _size)
{
	int ret = ERROR;

	ret = write(_fd, _data, _size);
	if (ret == ERROR) {
		DBG_MSG("<%s> write failure : errno=%d %s\r\n",
			__func__, errno, strerror(errno));
		return ERROR;
	}

	return ret;
}

//------------------------------------------------------------------------------
// Function Name  : serial_read()
// Description    :
//------------------------------------------------------------------------------
int serial_read(int _fd, u8 *_data, u32 _size)
{
	int ret = ERROR;

	ret = read(_fd, _data, _size);
	if (ret == ERROR) {
		DBG_MSG("<%s> read failure : errno=%d %s\r\n", __func__, errno, strerror(errno));
		return ERROR;
	}

	return ret;
}

//------------------------------------------------------------------------------
// Function Name  : serial_register_listener()
// Description    :
//------------------------------------------------------------------------------
int serial_register_listener(serial_listener _handler)
{
	int res = 0;

	res = pthread_create(&g_serial_thread, NULL, _handler, NULL);
	if (res < 0) {
		DBG_MSG("<%s> pthread_create failure: error=%d %s\r\n", __func__, errno, strerror(errno));
		return FALSE;
	}

	return TRUE;
}

