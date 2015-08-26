/******************************************************************************
 * Filename:
 *   controller_watchdog.c
 *
 * Description:
 *   control the watchdog function
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
#include "controller_watchdog.h"
/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
static int g_wdt_fd;
volatile WATCHreg	*g_wdt_reg;

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
u8 init_watchdog_controller(void);
void wdt_set_period(u32 _period);
void wdt_enable(void);
void wdt_disable(void);
void wdt_refresh(void);
void wdt_reboot(void);
//------------------------------------------------------------------------------
// Function Name  : init_watchdog_controller()
// Description    :
//------------------------------------------------------------------------------
u8 init_watchdog_controller(void)
{
	g_wdt_fd = open(MEM_DEV, O_RDWR|O_SYNC);
	if (g_wdt_fd < 0) 	{
		DBG_MSG("<%s> open failed(%s)\r\n", __func__, strerror(errno));
		return 0;
	}

	g_wdt_reg = (volatile WATCHreg *)mmap(
				0,						// start
				sizeof(WATCHreg),		// length
				PROT_READ|PROT_WRITE,	// prot
				MAP_SHARED,				// flag
				g_wdt_fd,				// fd
				WDT_TIMER_REG_PHYSICAL	// offset
				);

	return 1;
}

//------------------------------------------------------------------------------
// Function Name  : wdt_set_period()
// Description    :
//------------------------------------------------------------------------------
void wdt_set_period(u32 _period)
{
	if (g_wdt_reg) {
		if(_period > MAX_WATCHDOG_PERIOD) 
			_period = MAX_WATCHDOG_PERIOD;

		g_wdt_reg->rWTDAT = TICKS_PER_SECOND / 1000 * _period;
		g_wdt_reg->rWTCNT = TICKS_PER_SECOND / 1000 * _period;
	}
}

//------------------------------------------------------------------------------
// Function Name  : wdt_enable()
// Description    :
//------------------------------------------------------------------------------
void wdt_enable(void)
{
	u16 n_reg;

	if (g_wdt_reg) {
		n_reg = g_wdt_reg->rWTCON;

		n_reg |=  (255 << 8);	//Prescaler Value			0 ~ 255
		n_reg |=  (1 << 5);		//Watchdog Timer			0=Disable, 1=Enable
		n_reg &= ~(3 << 3);		//Clock Division Factor		0=16, 1=32, 2=64, 3=128
		n_reg |=  (1 << 0);		//Watchdog timer output for reset	0=Disable, 1=Enable

		g_wdt_reg->rWTCON = n_reg;
	}
}

//------------------------------------------------------------------------------
// Function Name  : wdt_disable()
// Description    :
//------------------------------------------------------------------------------
void wdt_disable(void)
{
	u16 n_reg;

	if (g_wdt_reg) {
		n_reg = g_wdt_reg->rWTCON;

	//	n_reg |=  (255 << 8);	//Prescaler Value			0 ~ 255
		n_reg &= ~(1 << 5);		//Watchdog Timer			0=Disable, 1=Enable
	//	n_reg &= ~(3 << 3);		//Clock Division Factor		0=16, 1=32, 2=64, 3=128
		n_reg &= ~(1 << 0);		//Watchdog timer output for reset	0=Disable, 1=Enable

		g_wdt_reg->rWTCON = n_reg;
	}
}

//------------------------------------------------------------------------------
// Function Name  : wdt_refresh()
// Description    :
//------------------------------------------------------------------------------
void wdt_refresh(void)
{
	if (g_wdt_reg) 
		g_wdt_reg->rWTCNT = g_wdt_reg->rWTDAT;
}

//------------------------------------------------------------------------------
// Function Name  : wdt_reboot()
// Description    :
//------------------------------------------------------------------------------
void wdt_reboot(void)
{
	wdt_set_period(10);
	wdt_enable();
	while(1);
}

