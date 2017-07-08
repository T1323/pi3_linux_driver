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

#define DEVFILE "/dev/lcm_dev0"

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

int main()
{
	int fd, ret;
	char str[32];
	
	
	fd = open(DEVFILE, O_RDWR);
	
	sleep(2);
	
	sprintf(str, "hello pi3!");
	lseek64(fd, 0, SEEK_SET);
	
	printf("len %d\n", strlen(str));
	
	//write(fd, str, strlen(str) - 1);
	runningWords(fd, str, strlen(str) - 1);
	
	sleep(2);
	
	close(fd);
}
