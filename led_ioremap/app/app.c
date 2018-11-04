#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "ioctl.h"

//./a.out on/off 1~4
//./a.out stat
int main(int argc, char **argv)
{
	int ret;
	char buf[4] = {0};
	int fd;
	int num, cmd;
	int i;

	if ((argc != 3) && (argc != 2)) {
		fprintf(stderr, "Usage: %s on/off 1~4\n", argv[0]);
		fprintf(stderr, "Usage: %s stat\n", argv[0]);
		exit(1);
	}


	fd = open("/dev/leds", O_RDWR | O_NDELAY);
	assert(fd > 0);

	if (argc == 3) {
		if (!strncmp("on", argv[1], 2)) {
			cmd = LEDON;	
		} else if (!strncmp("off", argv[1], 3)) {
			cmd = LEDOFF;
		} else {
			fprintf(stderr, "Usage: %s on/off 1~4\n", argv[0]);
			fprintf(stderr, "Usage: %s stat\n", argv[0]);
			exit(1);
		}

		num = atoi(argv[2]);

		ret = ioctl(fd, cmd, num);
		assert(ret == 0);
	} else {
		if (!strncmp("stat", argv[1], 4)) {
			ret = ioctl(fd, LEDSTAT, buf);
			assert(ret == 0);
			for (i = 0; i < 4; i++) {
				printf("led %d is %s.\n",
						i+1, buf[i]?"on":"off");
			}
		} else {
			fprintf(stderr, "Usage: %s on/off 1~4\n", argv[0]);
			fprintf(stderr, "Usage: %s stat\n", argv[0]);
			exit(1);
		}
	}

	close(fd);

	return 0;
}

