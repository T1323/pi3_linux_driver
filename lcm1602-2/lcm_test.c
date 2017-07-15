/*
 *
 * 
 * test program for lcm_dev
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "lcm1602_ioctl.h"

#define DEVFILE "/dev/lcm_dev0"

int main(int argc, char * argv[])
{
	int fd, ret;
	char str[32];
	char ioctl_cmd;
	
	fd = open(DEVFILE, O_RDWR);
	
	sprintf(str, "ready");
	write(fd, str, strlen(str));

	sleep(3);
	ioctl_cmd = CMD_CLEAR_DISPLAY;
	ioctl(fd, LCM_W_MOD, &ioctl_cmd);
	lseek(fd, 0, SEEK_SET);
	sprintf(str, "line 1");
	write(fd, str, strlen(str));
	
	sleep(3);
	lseek(fd, 16, SEEK_SET);
	sprintf(str, "line 2");
	write(fd, str, strlen(str));
	
	sleep(3);

	close(fd);
	
	return 0;
}
