/******************************************************************************
 * Filename:
 *   app_obd_svc.c
 *
 * Description:
 *   obd service application
 *
 * Author:
 *   gandy
 *
 * Version : V0.1_15-02-03
 * ---------------------------------------------------------------------------
 * Abbreviation
 ******************************************************************************/
#include "main.h"
#include "controller_gpio.h"
/******************************************************************************
 *
 * Variable Declaration
 *
 ******************************************************************************/
#define PIN_BIT(p)			(1 << p)
#define CHK_PIN(r, b)			(r) & PIN_BIT(b)
#define SET_FUNC(b, f)		((f) << (b*2))

int g_gpio_fd;
volatile IOPreg *g_gpio_reg;

/******************************************************************************
 *
 * Public Functions Declaration
 *
 ******************************************************************************/
void init_gpio_controller(void);
void gpio_config(u32 _port, u32 _pin, u32 _func);
u8 gpio_get(u32 _port, u32 _pin);
void gpio_set(u32 _port, u32 _pin);
void gpio_clr(u32 _port, u32 _pin);

//------------------------------------------------------------------------------
// Function Name  : gpio_data_reg()
// Description    :
//------------------------------------------------------------------------------
static volatile u32* gpio_data_reg(u32 _port)
{	
	volatile u32 *reg;

	switch (_port) {
	case GPIOB:
		reg = &(g_gpio_reg->rGPBDAT);
		break;
	case GPIOC:
		reg = &(g_gpio_reg->rGPCDAT);
		break;
	case GPIOD:
		reg = &(g_gpio_reg->rGPDDAT);
		break;
	case GPIOE:
		reg = &(g_gpio_reg->rGPEDAT);
		break;
	case GPIOF:
		reg = &(g_gpio_reg->rGPFDAT);
		break;
	case GPIOG:
		reg = &(g_gpio_reg->rGPGDAT);
		break;
	case GPIOH:
		reg = &(g_gpio_reg->rGPHDAT);
		break;
	case GPIOJ:
		reg = &(g_gpio_reg->rGPJDAT);
		break;
	}

	return reg;
}

//------------------------------------------------------------------------------
// Function Name  : init_gpio_controller()
// Description    :
//------------------------------------------------------------------------------
void init_gpio_controller(void)
{	
	PRINT_FUNC_CO();
	
	g_gpio_fd = open(MEM_DEV, O_RDWR|O_SYNC);
	if (g_gpio_fd == -1) {
		DBG_MSG_CO(CO_RED, "open failed!(%s)\r\n", strerror(errno));
		return;
	}

	g_gpio_reg = (volatile IOPreg *)mmap(
				0,						// start
				sizeof(IOPreg),			// length
				PROT_READ|PROT_WRITE,	// prot
				MAP_SHARED,				// flag
				m_fdMem,				// fd
				GPIO_REG_PHYSICAL		// offset
				);
	if ((int)g_gpio_reg == -1) {
		DBG_MSG_CO(CO_RED, "mmap failed!(%s)\r\n", strerror(errno));
		close(g_gpio_fd);
		return;
	}

	gpio_config(GPIOJ, GPIO_RIGHT_BOTTOM, GPIO_IN);
	gpio_config(GPIOJ, GPIO_RIGHT_MIDDLE, GPIO_IN);
	gpio_config(GPIOJ, GPIO_RIGHT_TOP, GPIO_IN);
	gpio_config(GPIOJ, GPIO_LEFT_BOTTOM, GPIO_IN);
	gpio_config(GPIOJ, GPIO_LEFT_MIDDLE, GPIO_IN);
	gpio_config(GPIOJ, GPIO_LEFT_TOP, GPIO_IN);

	gpio_config(GPIOF, GPIO_VOL_UP, GPIO_IN);
	gpio_config(GPIOF, GPIO_VOL_DOWN, GPIO_IN);
	gpio_config(GPIOF, GPIO_DOOR_DETECT, GPIO_IN);

	gpio_config(GPIOB, GPIO_BACKLIGHT, GPIO_OUT);
	gpio_set(GPIOB, GPIO_BACKLIGHT);

	gpio_config(GPIOG, GPIO_RS485_CTRL, GPIO_OUT);
	gpio_set(GPIOG, GPIO_RS485_CTRL);

	gpio_config(GPIOG, GPIO_RELAY, GPIO_OUT);
	gpio_set(GPIOG, GPIO_RELAY);
	
	gpio_config(GPIOG, GPIO_LED, GPIO_OUT);
	gpio_set(GPIOG, GPIO_LED);

	gpio_config(GPIOG, GPIO_AMP, GPIO_OUT);
	gpio_clr(GPIOG, GPIO_AMP);
}

//------------------------------------------------------------------------------
// Function Name  : gpio_get()
// Description    :
//------------------------------------------------------------------------------
void gpio_config(u32 _port, u32 _pin, u32 _func)
{
	volatile u32 *ctl_r = NULL;

	switch (_port) {
	case GPIOA:
		ctl_r = &(g_gpio_reg->rGPACON);
		break;
	case GPIOB:
		ctl_r = &(g_gpio_reg->rGPBCON);
		break;
	case GPIOC:
		ctl_r = &(g_gpio_reg->rGPCCON);
		break;
	case GPIOD:
		ctl_r = &(g_gpio_reg->rGPDCON);
		break;
	case GPIOE:
		ctl_r = &(g_gpio_reg->rGPECON);
		break;
	case GPIOF:
		ctl_r = &(g_gpio_reg->rGPFCON);
		break;
	case GPIOG:
		ctl_r = &(g_gpio_reg->rGPGCON);
		break;
	case GPIOH:
		ctl_r = &(g_gpio_reg->rGPHCON);
		break;
	case GPIOJ:
		ctl_r = &(g_gpio_reg->rGPJCON);
		break;
	}

	*ctl_r |= SET_FUNC(_pin, _func);
}

//------------------------------------------------------------------------------
// Function Name  : gpio_get()
// Description    :
//------------------------------------------------------------------------------
u8 gpio_get(u32 _port, u32 _pin)
{
	volatile u32 *reg = gpio_data_reg(_port);
		
	if (CHK_PIN(*reg, _pin))
		return HIGH;

	return LOW;
}

//------------------------------------------------------------------------------
// Function Name  : gpio_set()
// Description    :
//------------------------------------------------------------------------------
void gpio_set(u32 _port, u32 _pin)
{
	volatile u32 *reg = gpio_data_reg(_port);

	*reg |= PIN_BIT(_pin);
}

//------------------------------------------------------------------------------
// Function Name  : gpio_get()
// Description    :
//------------------------------------------------------------------------------
void gpio_clr(u32 _port, u32 _pin)
{
	volatile u32 *reg = gpio_data_reg(_port);
	u32 data = PIN_BIT(_pin);
	
	*reg &= ~data;
}

