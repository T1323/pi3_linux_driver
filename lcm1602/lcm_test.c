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
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#define DEVFILE "/dev/lcm_dev0"

#define LCM_IOC_TYPE 'D'
#define LCM_W_MOD _IOW(LCM_IOC_TYPE, 1, char)
#define LCM_R_MOD _IOR(LCM_IOC_TYPE, 2, char)
/*
void runningWords(int fd, char* str, int len)
{
	int loop = len;
	int i, j=0;
	
	do {
		j = 0;
		char tmp[32];
		for (i=(len - loop); i < len; i++, j++) {
			tmp[j] = *(str + i);
		}
		printf("%s %d\n", tmp, j);
		write(fd, tmp, j);
		sleep(1);
	} while(loop--);
}
*/
int main(int argc, char * argv[])
{
	int fd, ret;
	char mode = 1;
	char* str;
	const char *mod;
	
	if (argc < 3) {
		printf("not enough inputs\n");
		return 0;
	}
	
	mod = argv[1];
	str = argv[2];
	printf("intput string %s\n", str);
	fd = open(DEVFILE, O_RDWR);
	
	sleep(2);
	
	lseek64(fd, 0, SEEK_SET);
	
	sleep(2);
	mode = atoi(mod);
	ioctl(fd, LCM_W_MOD, &mode);
	
	write(fd, str, strlen(str));
/*
	do {
		sleep(1);
		ioctl(fd, LCM_R_MOD, &mode);
	}while(mode != 0xFF);
*/
	
	close(fd);
	
	return 0;
}
