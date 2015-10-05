#ifndef __SERIAL_H__
#define __SERIAL_H__

/******************************************************************************
 * Variable Type Definition
 ******************************************************************************/
typedef void* (*serial_listener)(void *_param);

/******************************************************************************
 * Function Export
 ******************************************************************************/
extern int serial_open(const char *_dev, u32 _baud);
extern void serial_close(int _fd);
extern int serial_write(int _fd, const u8 *_data, u32 _size);
extern int serial_read(int _fd, u8 *_data, u32 _size);
extern int serial_register_listener(serial_listener _handler);

#endif //__SERIAL_H__

