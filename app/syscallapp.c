#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define SZ 512

int myledcontrol(int num, int cmd)
{
	int ret;

	__asm__ __volatile__ (

		"mov r0, %[num]\n"	
		"mov r1, %[cmd]\n"

		"svc #(0x900000+378)\n"		
		
		"mov %[ret], r0\n"

		:[ret]"=&r"(ret)
		:[num]"r"(num),[cmd]"r"(cmd)
		:"r0","r1"
	);

	errno = 0 - ret;

	return ret;
}

//./a.out on/off 1~4
int main(int argc, char **argv)
{
	int ret;
	int num, cmd;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s on/off 1~4\n", argv[0]);
		exit(1);
	}

	printf("errno: %d\n", errno);

	if (!strncmp("on", argv[1], 2)) {
		cmd = 1;	
	} else if (!strncmp("off", argv[1], 3)) {
		cmd = 0;
	} else {
		fprintf(stderr, "Usage: %s on/off 1~4\n", argv[0]);
		exit(1);
	}

	num = atoi(argv[2]);

	ret = myledcontrol(num, cmd);
	if (ret < 0) {
		printf("ret: %d\n", ret);
		perror("myledcontrol");
		exit(1);
	}

	return 0;
}

