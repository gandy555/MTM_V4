#ifndef __CONTROLLER_GPIO_H__
#define __CONTROLLER_GPIO_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
#define MEM_DEV	"/dev/mem"

enum {
	GPIOA = 0,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG,
	GPIOH,
	GPIOJ
};

enum {
	GPIO_IN = 0,
	GPIO_OUT,
	GPIO_AF1,
	GPIO_AF2
};

/* GPIOJ */
#define GPIO_RIGHT_BOTTOM		0
#define GPIO_RIGHT_MIDDLE		1
#define GPIO_RIGHT_TOP			2
#define GPIO_LEFT_BOTTOM		3
#define GPIO_LEFT_MIDDLE		4
#define GPIO_LEFT_TOP			5

/* GPIOF */
#define GPIO_VOL_UP				0	
#define GPIO_VOL_DOWN			1	
#define GPIO_DOOR_DETECT		3

/* GPIOB */
#define GPIO_BACKLIGHT			0	

/* GPIOG */
#define GPIO_RS485_CTRL			2	
#define GPIO_RELAY				6	
#define GPIO_LED				7	
#define GPIO_AMP				8	

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern void init_gpio_controller(void);
extern void gpio_config(u32 _port, u32 _pin, u32 _func);
extern void gpio_set(u32 _port, u32 _pin);
extern u8 gpio_get(u32 _port, u32 _pin);
extern void gpio_clr(u32 _port, u32 _pin);

#endif //__CONTROLLER_GPIO_H__
