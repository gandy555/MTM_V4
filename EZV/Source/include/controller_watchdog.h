#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
/*
	계산상 4030[ms]정도의 주기까지 가능
	FCLK=399.65M, PCLK=FCLK/6, Prescale=255, ClockDiv=16 
	--> TicksPerSec=PCLK/256/16 = 16261

	16비트 타이머 이므로 65535 / 16261 = 4.03
*/
#define MAX_WATCHDOG_PERIOD		4000
#define TICKS_PER_SECOND		16261

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern u8 init_watchdog_controller(void);
extern void wdt_set_period(u32 _period);
extern void wdt_enable(void);
extern void wdt_disable(void);
extern void wdt_refresh(void);
extern void wdt_reboot(void);

#endif //__WATCHDOG_H__

