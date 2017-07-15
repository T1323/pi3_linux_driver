/*
 * 
 * define ioctl cmd of lcm1602
 * 
 */
 
 #ifndef _LCM1602_IOCTL_H
 #define _LCM1602_IOCTL_H
 
 #include <linux/ioctl.h>
 
#define LCM_IOC_TYPE 'D'

#define CMD_CLEAR_DISPLAY	1
#define CMD_RETURN_HOME		2
#define CMD_DISPLAY_ON      0x0C
#define CMD_DISPLAY_OFF     0x08

#define LCM_W_MOD _IOW(LCM_IOC_TYPE, 1, char)

 #endif
 
 
 
